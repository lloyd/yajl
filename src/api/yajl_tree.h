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

/* Forward declaration, because "yajl_value_object_t" and "yajl_value_array_t"
 * contain "yajl_value_t" and "yajl_value_t" can be an object or an array. */
struct yajl_value_s;
typedef struct yajl_value_s yajl_value_t;

/** Structure describing a JSON string. */
struct yajl_value_string_s
{
  /** Null terminated string. */
  char *value;
};
typedef struct yajl_value_string_s yajl_value_string_t;

#define YAJL_NUMBER_INT_VALID    0x01
#define YAJL_NUMBER_DOUBLE_VALID 0x02
/** Structure describing a JSON number. */
struct yajl_value_number_s
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
};
typedef struct yajl_value_number_s yajl_value_number_t;

/**
 * Structure describing a JSON object.
 *
 * \sa yajl_value_array_s
 */
struct yajl_value_object_s
{
  /** Array of keys in the JSON object. */
  yajl_value_t **keys;
  /** Array of values in the JSON object. */
  yajl_value_t **values;
  /** Number of key-value-pairs in the JSON object. */
  size_t children_num;
};
typedef struct yajl_value_object_s yajl_value_object_t;

/**
 * Structure describing a JSON array.
 *
 * \sa yajl_value_object_s
 */
struct yajl_value_array_s
{
  /** Array of elements in the JSON array. */
  yajl_value_t **values;
  /** Number of elements in the JSON array. */
  size_t values_num;
};
typedef struct yajl_value_array_s yajl_value_array_t;

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
 * \sa yajl_value_string_t, yajl_value_number_t, yajl_value_object_t,
 * yajl_value_array_t
 */
struct yajl_value_s
{
  /** Type of the value contained. Use the "YAJL_IS_*" macors to check for a
   * specific type. */
  uint8_t type;
  /** Type-specific data. Use the "YAJL_TO_*" macros to access these
   * members. */
  union
  {
    yajl_value_string_t string;
    yajl_value_number_t number;
    yajl_value_object_t object;
    yajl_value_array_t  array;
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
YAJL_API yajl_value_t *yajl_tree_parse (const char *input,
    char *error_buffer, size_t error_buffer_size);

/**
 * Free a parse tree.
 *
 * Recursively frees a pointer returned by "yajl_tree_parse".
 *
 * \param v Pointer to a JSON value returned by "yajl_tree_parse". Passing NULL
 * is valid and results in a no-op.
 */
YAJL_API void yajl_tree_free (yajl_value_t *v);

/**
 * Access a nested value.
 */
YAJL_API yajl_value_t * yajl_tree_get(yajl_value_t * parent,
                                      const char ** path,
                                      int type);

/**
 * Checks if value is a string.
 *
 * Returns true if the value is a string, false otherwise.
 */
#define YAJL_IS_STRING(v) (((v) != NULL) && ((v)->type == YAJL_TYPE_STRING))

/**
 * Checks if value is a number.
 *
 * Returns true if the value is a number, false otherwise.
 */
#define YAJL_IS_NUMBER(v) (((v) != NULL) && ((v)->type == YAJL_TYPE_NUMBER))

/**
 * Checks if value is an object.
 *
 * Returns true if the value is a object, false otherwise.
 */
#define YAJL_IS_OBJECT(v) (((v) != NULL) && ((v)->type == YAJL_TYPE_OBJECT))

/**
 * Checks if value is an array.
 *
 * Returns true if the value is a array, false otherwise.
 */
#define YAJL_IS_ARRAY(v)  (((v) != NULL) && ((v)->type == YAJL_TYPE_ARRAY ))

/**
 * Checks if value is true.
 *
 * Returns true if the value is a boolean and true, false otherwise.
 */
#define YAJL_IS_TRUE(v)   (((v) != NULL) && ((v)->type == YAJL_TYPE_TRUE  ))

/**
 * Checks if value is false.
 *
 * Returns true if the value is a boolean and false, false otherwise.
 */
#define YAJL_IS_FALSE(v)  (((v) != NULL) && ((v)->type == YAJL_TYPE_FALSE ))

/**
 * Checks if value is null.
 *
 * Returns true if the value is null, false otherwise.
 */
#define YAJL_IS_NULL(v)   (((v) != NULL) && ((v)->type == YAJL_TYPE_NULL  ))

/**
 * Convert value to string.
 *
 * Returns a pointer to a yajl_value_string_t or NULL if the value is not a
 * string.
 */
#define YAJL_TO_STRING(v) (YAJL_IS_STRING(v) ? (v)->data.string.value : NULL)

/**
 * Convert value to number.
 *
 * Returns a pointer to a yajl_value_number_t or NULL if the value is not a
 * number.
 */
#define YAJL_TO_NUMBER(v) (YAJL_IS_NUMBER(v) ? &(v)->data.number : NULL)

/**
 * Convert value to object.
 *
 * Returns a pointer to a yajl_value_object_t or NULL if the value is not an
 * object.
 */
#define YAJL_TO_OBJECT(v) (YAJL_IS_OBJECT(v) ? &(v)->data.object : NULL)

/**
 * Convert value to array.
 *
 * Returns a pointer to a yajl_value_array_t or NULL if the value is not an
 * array.
 */
#define YAJL_TO_ARRAY(v)  (YAJL_IS_ARRAY(v)  ? &(v)->data.array  : NULL)

#endif /* YAJL_TREE_H */
/* vim: set sw=2 sts=2 et : */
