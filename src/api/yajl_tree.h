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

/**
 * \file yajl_tree.h
 *
 * Parses JSON data and returns the data in tree form.
 *
 * \author Florian Forster
 * \date August 2010
 */

#ifndef YAJL_TREE_H
#define YAJL_TREE_H 1

#include <stdint.h>
#include <inttypes.h>

#include <yajl/yajl_common.h>

/* Forward declaration, because "yajl_val_object_t" and "yajl_val_array"
 * contain "yajl_val" and "yajl_val" can be an object or an array. */
typedef struct yajl_val_s * yajl_val;

#define YAJL_NUMBER_INT_VALID    0x01
#define YAJL_NUMBER_DOUBLE_VALID 0x02
/** Structure describing a JSON number. */
typedef struct yajl_val_number_s
{
    /** Holds the raw value of the number, in string form. */
    char   *r;
    /** Holds the integer value of the number, if possible. */
    long long i;
    /** Holds the double value of the number, if possible. */
    double  d;
    /** Signals whether the \em value_int and \em value_double members are
     * valid. See \c YAJL_NUMBER_INT_VALID and \c YAJL_NUMBER_DOUBLE_VALID. */
    unsigned int flags;
} yajl_val_number;

/**
 * Structure describing a JSON object.
 *
 * \sa yajl_val_array_s
 */
typedef struct yajl_val_object_s
{
    /** Array of keys in the JSON object. */
    const char **keys;
    /** Array of values in the JSON object. */
    yajl_val *values;
    /** Number of key-value-pairs in the JSON object. */
    size_t len;
} yajl_val_object;

/**
 * Structure describing a JSON array.
 *
 * \sa yajl_val_object_s
 */
typedef struct yajl_val_array_s
{
    /** Array of elements in the JSON array. */
    yajl_val *values;
    /** Number of elements in the JSON array. */
    size_t len;
} yajl_val_array;

typedef enum {
    yajl_t_string = 1,
    yajl_t_number = 2,
    yajl_t_object = 3,
    yajl_t_array = 4,
    yajl_t_true = 5,
    yajl_t_false = 6,
    yajl_t_null = 7,
    yajl_t_any = 8
} yajl_type;

/**
 * Struct describing a general JSON value.
 *
 * Each value is one of the seven types above. For "string", "number",
 * "object", and "array" additional data is available in the "data" union. Use
 * the "YAJL_IS_*" and "YAJL_TO_*" macros below to check for the correct type
 * and cast the struct.
 *
 * \sa yajl_val_string, yajl_val_number, yajl_val_object,
 * yajl_val_array
 */
struct yajl_val_s
{
    /** Type of the value contained. Use the "YAJL_IS_*" macors to check for a
     * specific type. */
    uint8_t type;
    /** Type-specific data. Use the "YAJL_TO_*" macros to access these
     * members. */
    union
    {
        char * string;
        yajl_val_number number;
        yajl_val_object object;
        yajl_val_array  array;
    } data;
};

/**
 * Parse a string.
 *
 * Parses an null-terminated string containing JSON data and returns a pointer
 * to the top-level value (root of the parse tree).
 *
 * \param input              Pointer to a null-terminated string containing
 *                           JSON data.
 * \param error_buffer       Pointer to a buffer in which an error message will
 *                           be stored if \em yajl_tree_parse fails, or
 *                           \c NULL. The buffer will be initialized before
 *                           parsing, so its content will be destroyed even if
 *                           \em yajl_tree_parse succeeds.
 * \param error_buffer_size  Size of the memory area pointed to by
 *                           \em error_buffer_size. If \em error_buffer_size is
 *                           \c NULL, this argument is ignored.
 *
 * \returns Pointer to the top-level value or \c NULL on error. The memory
 * pointed to must be freed using \em yajl_tree_free. In case of an error, a
 * null terminated message describing the error in more detail is stored in
 * \em error_buffer if it is not \c NULL.
 */
YAJL_API yajl_val yajl_tree_parse (const char *input,
                                   char *error_buffer, size_t error_buffer_size);

/**
 * Free a parse tree.
 *
 * Recursively frees a pointer returned by "yajl_tree_parse".
 *
 * \param v Pointer to a JSON value returned by "yajl_tree_parse". Passing NULL
 * is valid and results in a no-op.
 */
YAJL_API void yajl_tree_free (yajl_val v);

/**
 * Access a nested value.
 */
YAJL_API yajl_val yajl_tree_get(yajl_val parent, const char ** path, yajl_type type);

/* Various convenience macros to check the type of a `yajl_val` */
#define YAJL_IS_STRING(v) (((v) != NULL) && ((v)->type == yajl_t_string))
#define YAJL_IS_NUMBER(v) (((v) != NULL) && ((v)->type == yajl_t_number))
#define YAJL_IS_INTEGER(v) (YAJL_IS_NUMBER(v) && ((v)->data.flags & YAJL_NUMBER_INT_VALID))
#define YAJL_IS_DOUBLE(v) (YAJL_IS_NUMBER(v) && ((v)->data.flags & YAJL_NUMBER_DOUBLE_VALID))
#define YAJL_IS_OBJECT(v) (((v) != NULL) && ((v)->type == yajl_t_object))
#define YAJL_IS_ARRAY(v)  (((v) != NULL) && ((v)->type == yajl_t_array ))
#define YAJL_IS_TRUE(v)   (((v) != NULL) && ((v)->type == yajl_t_true  ))
#define YAJL_IS_FALSE(v)  (((v) != NULL) && ((v)->type == yajl_t_false ))
#define YAJL_IS_NULL(v)   (((v) != NULL) && ((v)->type == yajl_t_null  ))

/** Given a yajl_val_string return a ptr to the bare string it contains,
 *  or NULL if the value is not a string. */
#define YAJL_GET_STRING(v) (YAJL_IS_STRING(v) ? (v)->data.string : NULL)

/** Get the string representation of a number.  You should check type first,
 *  perhaps using YAJL_IS_NUMBER */
#define YAJL_GET_NUMBER(v) ((v)->data.number.r)

/** Get the double representation of a number.  You should check type first,
 *  perhaps using YAJL_IS_DOUBLE */
#define YAJL_GET_DOUBLE(v) ((v)->data.number.d)

/** Get the 64bit (long long) integer representation of a number.  You should
 *  check type first, perhaps using YAJL_IS_INTEGER */
#define YAJL_GET_INTEGER(v) ((v)->data.number.i)

/** Get a pointer to a yajl_val_object or NULL if the value is not an object. */
#define YAJL_GET_OBJECT(v) (YAJL_IS_OBJECT(v) ? &(v)->data.object : NULL)

/** Get a pointer to a yajl_val_array or NULL if the value is not an object. */
#define YAJL_GET_ARRAY(v)  (YAJL_IS_ARRAY(v)  ? &(v)->data.array  : NULL)

#endif /* YAJL_TREE_H */
