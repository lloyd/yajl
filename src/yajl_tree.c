/*
 * Copyright (c) 2010-2011  Florian Forster  <ff at octo.it>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "api/yajl_tree.h"
#include "api/yajl_parse.h"

#define STATUS_CONTINUE 1
#define STATUS_ABORT    0

struct stack_elem_s;
typedef struct stack_elem_s stack_elem_t;
struct stack_elem_s
{
    char * key;
    yajl_val value;
    stack_elem_t *next;
};

struct context_s
{
    stack_elem_t *stack;
    yajl_val root;
    char *errbuf;
    size_t errbuf_size;
};
typedef struct context_s context_t;

#define RETURN_ERROR(ctx,retval,...) do {                               \
        if ((ctx)->errbuf != NULL)                                      \
            snprintf ((ctx)->errbuf, (ctx)->errbuf_size, __VA_ARGS__);  \
        return (retval);                                                \
    } while (0)                                                         \

static yajl_val value_alloc (uint8_t type)
{
    yajl_val v;

    v = malloc (sizeof (*v));
    if (v == NULL) return (NULL);
    memset (v, 0, sizeof (*v));
    v->type = type;

    return (v);
}

static void yajl_object_free (yajl_val v)
{
    yajl_val_object *o;
    size_t i;

    o = YAJL_TO_OBJECT (v);
    if (o == NULL) return;

    for (i = 0; i < o->len; i++)
    {
        free((char *) o->keys[i]);
        o->keys[i] = NULL;
        yajl_tree_free (o->values[i]);
        o->values[i] = NULL;
    }

    free (o->keys);
    free (o->values);
    free (v);
}

static void yajl_array_free (yajl_val v)
{
    yajl_val_array *a;
    size_t i;

    a = YAJL_TO_ARRAY (v);
    if (a == NULL) return;

    for (i = 0; i < a->len; i++)
    {
        yajl_tree_free (a->values[i]);
        a->values[i] = NULL;
    }

    free (a->values);
    free (v);
}

/*
 * Parsing nested objects and arrays is implemented using a stack. When a new
 * object or array starts (a curly or a square opening bracket is read), an
 * appropriate value is pushed on the stack. When the end of the object is
 * reached (an appropriate closing bracket has been read), the value is popped
 * off the stack and added to the enclosing object using "context_add_value".
 */
static int context_push(context_t *ctx, yajl_val v)
{
    stack_elem_t *stack;

    stack = malloc (sizeof (*stack));
    if (stack == NULL)
        RETURN_ERROR (ctx, ENOMEM, "Out of memory");
    memset (stack, 0, sizeof (*stack));

    assert ((ctx->stack == NULL)
            || YAJL_IS_OBJECT (v)
            || YAJL_IS_ARRAY (v));

    stack->value = v;
    stack->next = ctx->stack;
    ctx->stack = stack;

    return (0);
}

static yajl_val context_pop(context_t *ctx)
{
    stack_elem_t *stack;
    yajl_val v;

    if (ctx->stack == NULL)
        RETURN_ERROR (ctx, NULL, "context_pop: "
                      "Bottom of stack reached prematurely");

    stack = ctx->stack;
    ctx->stack = stack->next;

    v = stack->value;

    free (stack);

    return (v);
}

static int object_add_keyval(context_t *ctx,
                             yajl_val obj, char *key, yajl_val value)
{
    yajl_val_object *o;
    const char **tmpk;
    yajl_val *tmpv;

    /* We're checking for NULL in "context_add_value" or its callers. */
    assert (ctx != NULL);
    assert (obj != NULL);
    assert (key != NULL);
    assert (value != NULL);

    /* We're assuring that "obj" is an object in "context_add_value". */
    o = YAJL_TO_OBJECT (obj);
    assert (o != NULL);

    tmpk = realloc (o->keys, sizeof (*o->keys) * (o->len + 1));
    if (tmpk == NULL)
        RETURN_ERROR (ctx, ENOMEM, "Out of memory");
    o->keys = tmpk;

    tmpv = realloc (o->values, sizeof (*o->values) * (o->len + 1));
    if (tmpv == NULL)
        RETURN_ERROR (ctx, ENOMEM, "Out of memory");
    o->values = tmpv;

    o->keys[o->len] = key;
    o->values[o->len] = value;
    o->len++;

    return (0);
}

static int array_add_value (context_t *ctx,
                            yajl_val array, yajl_val value)
{
    yajl_val_array *a;
    yajl_val *tmp;

    /* We're checking for NULL pointers in "context_add_value" or its
     * callers. */
    assert (ctx != NULL);
    assert (array != NULL);
    assert (value != NULL);

    /* "context_add_value" will only call us with array values. */
    a = YAJL_TO_ARRAY (array);
    assert (a != NULL);

    tmp = realloc (a->values, sizeof (*a->values) * (a->len + 1));
    if (tmp == NULL)
        RETURN_ERROR (ctx, ENOMEM, "Out of memory");
    a->values = tmp;
    a->values[a->len] = value;
    a->len++;

    return (0);
}

/*
 * Add a value to the value on top of the stack or the "root" member in the
 * context if the end of the parsing process is reached.
 */
static int context_add_value (context_t *ctx, yajl_val v)
{
    /* We're checking for NULL values in all the calling functions. */
    assert (ctx != NULL);
    assert (v != NULL);

    /*
     * There are three valid states in which this function may be called:
     *   - There is no value on the stack => This is the only value. This is the
     *     last step done when parsing a document. We assign the value to the
     *     "root" member and return.
     *   - The value on the stack is an object. In this case store the key on the
     *     stack or, if the key has already been read, add key and value to the
     *     object.
     *   - The value on the stack is an array. In this case simply add the value
     *     and return.
     */
    if (ctx->stack == NULL)
    {
        assert (ctx->root == NULL);
        ctx->root = v;
        return (0);
    }
    else if (YAJL_IS_OBJECT (ctx->stack->value))
    {
        if (ctx->stack->key == NULL)
        {
            if (!YAJL_IS_STRING (v))
                RETURN_ERROR (ctx, EINVAL, "context_add_value: "
                              "Object key is not a string (%#04"PRIx8")",
                              v->type);

            ctx->stack->key = v->data.string;
            v->data.string = NULL;
            free(v);
            return (0);
        }
        else /* if (ctx->key != NULL) */
        {
            char * key;

            key = ctx->stack->key;
            ctx->stack->key = NULL;
            return (object_add_keyval (ctx, ctx->stack->value, key, v));
        }
    }
    else if (YAJL_IS_ARRAY (ctx->stack->value))
    {
        return (array_add_value (ctx, ctx->stack->value, v));
    }
    else
    {
        RETURN_ERROR (ctx, EINVAL, "context_add_value: Cannot add value to "
                      "a value of type %#04"PRIx8" (not a composite type)",
                      ctx->stack->value->type);
    }
}

static int handle_string (void *ctx,
                          const unsigned char *string, size_t string_length)
{
    yajl_val v;

    v = value_alloc (YAJL_TYPE_STRING);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->data.string = malloc (string_length + 1);
    if (v->data.string == NULL)
    {
        free (v);
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");
    }
    memcpy(v->data.string, string, string_length);
    v->data.string[string_length] = 0;

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_number (void *ctx, const char *string, size_t string_length)
{
    yajl_val v;
    yajl_val_number *n;
    char *endptr;

    v = value_alloc (YAJL_TYPE_NUMBER);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");
    n = YAJL_TO_NUMBER (v);

    n->value_raw = malloc (string_length + 1);
    if (n->value_raw == NULL)
    {
        free (v);
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");
    }
    memcpy (n->value_raw, string, string_length);
    n->value_raw[string_length] = 0;

    n->flags = 0;

    endptr = NULL;
    errno = 0;
    n->value_int = (int64_t) strtoll (n->value_raw, &endptr, /* base = */ 10);
    if ((errno == 0) && (endptr != NULL) && (*endptr == 0))
        n->flags |= YAJL_NUMBER_INT_VALID;

    endptr = NULL;
    errno = 0;
    n->value_double = strtod (n->value_raw, &endptr);
    if ((errno == 0) && (endptr != NULL) && (*endptr == 0))
        n->flags |= YAJL_NUMBER_DOUBLE_VALID;

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_start_map (void *ctx)
{
    yajl_val v;
    yajl_val_object *o;

    v = value_alloc (YAJL_TYPE_OBJECT);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    o = YAJL_TO_OBJECT (v);
    o->keys = NULL;
    o->values = NULL;
    o->len = 0;

    return ((context_push (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_end_map (void *ctx)
{
    yajl_val v;

    v = context_pop (ctx);
    if (v == NULL)
        return (STATUS_ABORT);

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_start_array (void *ctx)
{
    yajl_val v;
    yajl_val_array *a;

    v = value_alloc (YAJL_TYPE_ARRAY);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    a = YAJL_TO_ARRAY (v);
    a->values = NULL;
    a->len = 0;

    return ((context_push (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_end_array (void *ctx)
{
    yajl_val v;

    v = context_pop (ctx);
    if (v == NULL)
        return (STATUS_ABORT);

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_boolean (void *ctx, int boolean_value)
{
    yajl_val v;

    v = value_alloc (boolean_value ? YAJL_TYPE_TRUE : YAJL_TYPE_FALSE);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_null (void *ctx)
{
    yajl_val v;

    v = value_alloc (YAJL_TYPE_NULL);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

/*
 * Public functions
 */
yajl_val yajl_tree_parse (const char *input,
                          char *error_buffer, size_t error_buffer_size)
{
    static const yajl_callbacks callbacks =
        {
            /* null        = */ handle_null,
            /* boolean     = */ handle_boolean,
            /* integer     = */ NULL,
            /* double      = */ NULL,
            /* number      = */ handle_number,
            /* string      = */ handle_string,
            /* start map   = */ handle_start_map,
            /* map key     = */ handle_string,
            /* end map     = */ handle_end_map,
            /* start array = */ handle_start_array,
            /* end array   = */ handle_end_array
        };

    context_t ctx =
        {
            /* key         = */ NULL,
            /* stack       = */ NULL,
            /* errbuf      = */ error_buffer,
            /* errbuf_size = */ error_buffer_size
        };

    yajl_handle handle;
    yajl_status status;

    if (error_buffer != NULL)
        memset (error_buffer, 0, error_buffer_size);

    handle = yajl_alloc (&callbacks, NULL, &ctx);
    yajl_config(handle, yajl_allow_comments, 1);

    status = yajl_parse(handle,
                        (unsigned char *) input,
                        strlen (input));
    status = yajl_complete_parse (handle);
    if (status != yajl_status_ok) {
        if (error_buffer != NULL && error_buffer_size > 0) {
            snprintf(
                error_buffer, error_buffer_size,
                (char *) yajl_get_error(handle, 1,
                                        (const unsigned char *) input,
                                        strlen(input)));
        }
        yajl_free (handle);
        return NULL;
    }

    yajl_free (handle);
    return (ctx.root);
}

yajl_val yajl_tree_get(yajl_val n, const char ** path, int type)
{
    if (!path) return NULL;
    while (n && *path) {
        unsigned int i;

        if (n->type != YAJL_TYPE_OBJECT) return NULL;
        for (i = 0; i < n->data.object.len; i++) {
            if (!strcmp(*path, n->data.object.keys[i])) {
                n = n->data.object.values[i];
                break;
            }
        }
        if (i == n->data.object.len) return NULL;
        path++;
    }
    return n;
}

void yajl_tree_free (yajl_val v)
{
    if (v == NULL) return;

    if (YAJL_IS_STRING(v))
    {
        free(v->data.string);
        free(v);
    }
    else if (YAJL_IS_NUMBER(v))
    {
        yajl_val_number *n = YAJL_TO_NUMBER(v);

        free(n->value_raw);
        free(v);
    }
    else if (YAJL_TO_OBJECT(v))
    {
        yajl_object_free(v);
    }
    else if (YAJL_TO_ARRAY(v))
    {
        yajl_array_free(v);
    }
    else /* if (YAJL_TYPE_TRUE or YAJL_TYPE_FALSE or YAJL_TYPE_NULL) */
    {
        free(v);
    }
}
