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
    char   *value_raw;
    /** Holds the integer value of the number, if possible. */
    int64_t value_int;
    /** Holds the double value of the number, if possible. */
    double  value_double;
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
    yajl_val *keys;
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

#define YAJL_TYPE_STRING 1
#define YAJL_TYPE_NUMBER 2
#define YAJL_TYPE_OBJECT 3
#define YAJL_TYPE_ARRAY  4
#define YAJL_TYPE_TRUE   5
#define YAJL_TYPE_FALSE  6
#define YAJL_TYPE_NULL   7

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
YAJL_API yajl_val yajl_tree_get(yajl_val parent, const char ** path, int type);

/* Various convenience macros to check the type of a `yajl_val` */
#define YAJL_IS_STRING(v) (((v) != NULL) && ((v)->type == YAJL_TYPE_STRING))
#define YAJL_IS_NUMBER(v) (((v) != NULL) && ((v)->type == YAJL_TYPE_NUMBER))
#define YAJL_IS_OBJECT(v) (((v) != NULL) && ((v)->type == YAJL_TYPE_OBJECT))
#define YAJL_IS_ARRAY(v)  (((v) != NULL) && ((v)->type == YAJL_TYPE_ARRAY ))
#define YAJL_IS_TRUE(v)   (((v) != NULL) && ((v)->type == YAJL_TYPE_TRUE  ))
#define YAJL_IS_FALSE(v)  (((v) != NULL) && ((v)->type == YAJL_TYPE_FALSE ))
#define YAJL_IS_NULL(v)   (((v) != NULL) && ((v)->type == YAJL_TYPE_NULL  ))

/**
 * Convert value to string.
 *
 * Returns a pointer to a yajl_val_string or NULL if the value is not a
 * string.
 */
#define YAJL_TO_STRING(v) (YAJL_IS_STRING(v) ? (v)->data.string : NULL)

/**
 * Convert value to number.
 *
 * Returns a pointer to a yajl_val_number or NULL if the value is not a
 * number.
 */
#define YAJL_TO_NUMBER(v) (YAJL_IS_NUMBER(v) ? &(v)->data.number : NULL)

/**
 * Convert value to object.
 *
 * Returns a pointer to a yajl_val_object or NULL if the value is not an
 * object.
 */
#define YAJL_TO_OBJECT(v) (YAJL_IS_OBJECT(v) ? &(v)->data.object : NULL)

/**
 * Convert value to array.
 *
 * Returns a pointer to a yajl_val_array or NULL if the value is not an
 * array.
 */
#define YAJL_TO_ARRAY(v)  (YAJL_IS_ARRAY(v)  ? &(v)->data.array  : NULL)

#endif /* YAJL_TREE_H */
