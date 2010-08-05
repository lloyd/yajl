/*
 * Copyright (C) 2010  Florian Forster  <ff at octo.it>
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 * 
 *  3. Neither the name of Lloyd Hilaiel nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */ 

#include <stdlib.h>
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
  yajl_value_t *key;
  yajl_value_t *value;
  stack_elem_t *next;
};

struct context_s
{
  stack_elem_t *stack;
  yajl_value_t *root;
};
typedef struct context_s context_t;

static yajl_value_t *value_alloc (uint8_t type) /* {{{ */
{
  yajl_value_t *v;

  v = malloc (sizeof (*v));
  if (v == NULL)
    return (NULL);

  memset (v, 0, sizeof (*v));
  v->type = type;

  return (v);
} /* }}} yajl_value_t *value_alloc */

static int context_push (context_t *ctx, yajl_value_t *v) /* {{{ */
{
  stack_elem_t *stack;

  stack = malloc (sizeof (*stack));
  if (stack == NULL)
    return (ENOMEM);
  memset (stack, 0, sizeof (*stack));

  assert ((ctx->stack == NULL)
      || (v->type == VALUE_TYPE_OBJECT)
      || (v->type == VALUE_TYPE_ARRAY));

  stack->value = v;
  stack->next = ctx->stack;
  ctx->stack = stack;

  return (0);
} /* }}} int context_push */

static yajl_value_t *context_pop (context_t *ctx) /* {{{ */
{
  stack_elem_t *stack;
  yajl_value_t *v;

  if (ctx->stack == NULL)
    return (NULL);

  stack = ctx->stack;
  ctx->stack = stack->next;

  v = stack->value;

  free (stack);

  return (v);
} /* }}} yajl_value_t *context_pop */

static int object_add_keyval (yajl_value_t *obj, /* {{{ */
    yajl_value_t *key, yajl_value_t *value)
{
  yajl_value_object_t *o;
  yajl_value_t **tmp;

  if ((obj == NULL) || (key == NULL) || (value == NULL))
    return (EINVAL);

  if ((obj->type != VALUE_TYPE_OBJECT) || (key->type != VALUE_TYPE_STRING))
    return (EINVAL);

  o = &obj->data.object;
  tmp = realloc (o->keys, sizeof (*o->keys) * (o->children_num + 1));
  if (tmp == NULL)
    return (ENOMEM);
  o->keys = tmp;

  tmp = realloc (o->values, sizeof (*o->values) * (o->children_num + 1));
  if (tmp == NULL)
    return (ENOMEM);
  o->values = tmp;

  o->keys[o->children_num] = key;
  o->values[o->children_num] = value;
  o->children_num++;

  return (0);
} /* }}} int object_add_keyval */

static int array_add_value (yajl_value_t *array, /* {{{ */
    yajl_value_t *value)
{
  yajl_value_array_t *a;
  yajl_value_t **tmp;

  if ((array == NULL) || (value == NULL))
    return (EINVAL);

  if (array->type != VALUE_TYPE_ARRAY)
    return (EINVAL);

  a = &array->data.array;
  tmp = realloc (a->children, sizeof (*a->children) * (a->children_num + 1));
  if (tmp == NULL)
    return (ENOMEM);
  a->children = tmp;
  a->children[a->children_num] = value;
  a->children_num++;

  return (0);
} /* }}} int array_add_value */

static int context_add_value (context_t *ctx, yajl_value_t *v) /* {{{ */
{
  if (ctx->stack == NULL)
  {
    assert (ctx->root == NULL);
    ctx->root = v;
    return (0);
  }
  else if (ctx->stack->value->type == VALUE_TYPE_OBJECT)
  {
    if (ctx->stack->key == NULL)
    {
      if (v->type != VALUE_TYPE_STRING)
        return (EINVAL);

      ctx->stack->key = v;
      return (0);
    }
    else /* if (ctx->key != NULL) */
    {
      yajl_value_t *key;

      key = ctx->stack->key;
      ctx->stack->key = NULL;
      return (object_add_keyval (ctx->stack->value, key, v));
    }
  }
  else if (ctx->stack->value->type == VALUE_TYPE_ARRAY)
  {
    return (array_add_value (ctx->stack->value, v));
  }
  else
  {
    return (EINVAL);
  }
} /* }}} int context_add_value */

static int handle_string (void *ctx, /* {{{ */
    const unsigned char *string, unsigned int string_length)
{
  yajl_value_t *v;
  yajl_value_string_t *s;

  v = value_alloc (VALUE_TYPE_STRING);
  if (v == NULL)
    return (STATUS_ABORT);
  s = &v->data.string;

  s->value = malloc (string_length + 1);
  if (s->value == NULL)
  {
    free (v);
    return (STATUS_ABORT);
  }
  memcpy (s->value, string, string_length);
  s->value[string_length] = 0;

  return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
} /* }}} int handle_string */

static int handle_number (void *ctx, /* {{{ */
    const char *string, unsigned int string_length)
{
  yajl_value_t *v;
  yajl_value_number_t *n;

  v = value_alloc (VALUE_TYPE_STRING);
  if (v == NULL)
    return (STATUS_ABORT);
  n = &v->data.number;

  n->value = malloc (string_length + 1);
  if (n->value == NULL)
  {
    free (v);
    return (STATUS_ABORT);
  }
  memcpy (n->value, string, string_length);
  n->value[string_length] = 0;

  return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
} /* }}} int handle_number */

static int handle_start_map (void *ctx) /* {{{ */
{
  yajl_value_t *v;
  yajl_value_object_t *o;

  v = value_alloc (VALUE_TYPE_OBJECT);
  if (v == NULL)
    return (STATUS_ABORT);

  o = &v->data.object;
  o->keys = NULL;
  o->values = NULL;
  o->children_num = 0;

  return ((context_push (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
} /* }}} int handle_start_map */

static int handle_end_map (void *ctx) /* {{{ */
{
  yajl_value_t *v;

  v = context_pop (ctx);
  if (v == NULL)
    return (STATUS_ABORT);

  return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
} /* }}} int handle_end_map */

static int handle_start_array (void *ctx) /* {{{ */
{
  yajl_value_t *v;
  yajl_value_array_t *a;

  v = value_alloc (VALUE_TYPE_ARRAY);
  if (v == NULL)
    return (STATUS_ABORT);

  a = &v->data.array;
  a->children = NULL;
  a->children_num = 0;

  return ((context_push (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
} /* }}} int handle_start_array */

static int handle_end_array (void *ctx) /* {{{ */
{
  yajl_value_t *v;

  v = context_pop (ctx);
  if (v == NULL)
    return (STATUS_ABORT);

  return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
} /* }}} int handle_end_array */

static int handle_boolean (void *ctx, int boolean_value) /* {{{ */
{
  yajl_value_t *v;

  v = value_alloc (boolean_value ? VALUE_TYPE_TRUE : VALUE_TYPE_FALSE);
  if (v == NULL)
    return (STATUS_ABORT);

  return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
} /* }}} int handle_boolean */

static int handle_null (void *ctx) /* {{{ */
{
  yajl_value_t *v;

  v = value_alloc (VALUE_TYPE_NULL);
  if (v == NULL)
    return (STATUS_ABORT);

  return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
} /* }}} int handle_null */

yajl_value_t *yajl_tree_parse (const char *input) /* {{{ */
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

  yajl_parser_config parser_config =
  {
    /* allowComments = */ 1,
    /* checkUTF8     = */ 0
  };

  context_t ctx =
  {
    /* key   = */ NULL,
    /* stack = */ NULL
  };

  yajl_handle handle;
  yajl_status status;

  handle = yajl_alloc (&callbacks, &parser_config,
      /* alloc funcs = */ NULL, &ctx);

  status = yajl_parse (handle,
      (unsigned char *) input,
      (unsigned int) strlen (input));
  if (status != yajl_status_ok)
    return (NULL);

  status = yajl_parse_complete (handle);
  if (status != yajl_status_ok)
    return (NULL);

  yajl_free (handle);
  return (ctx.root);
} /* }}} yajl_value_t *yajl_tree_parse */

/* vim: set sw=2 sts=2 et fdm=marker : */
