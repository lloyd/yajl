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

#ifndef YAJL_TREE_H
#define YAJL_TREE_H 1

#include <stdint.h>
#include <inttypes.h>

#include <yajl/yajl_common.h>

struct yajl_value_s;
typedef struct yajl_value_s yajl_value_t;

struct yajl_value_string_s
{
  char *value;
};
typedef struct yajl_value_string_s yajl_value_string_t;

struct yajl_value_number_s
{
  char *value;
};
typedef struct yajl_value_number_s yajl_value_number_t;

struct yajl_value_object_s
{
  yajl_value_t **keys;
  yajl_value_t **values;
  size_t children_num;
};
typedef struct yajl_value_object_s yajl_value_object_t;

struct yajl_value_array_s
{
  yajl_value_t **children;
  size_t children_num;
};
typedef struct yajl_value_array_s yajl_value_array_t;

#define VALUE_TYPE_STRING 1
#define VALUE_TYPE_NUMBER 2
#define VALUE_TYPE_OBJECT 3
#define VALUE_TYPE_ARRAY  4
#define VALUE_TYPE_TRUE   5
#define VALUE_TYPE_FALSE  6
#define VALUE_TYPE_NULL   7

struct yajl_value_s
{
  uint8_t type;
  union
  {
    yajl_value_string_t string;
    yajl_value_number_t number;
    yajl_value_object_t object;
    yajl_value_array_t  array;
  } data;
};

YAJL_API yajl_value_t *yajl_tree_parse (const char *input);
YAJL_API void yajl_tree_free (yajl_value_t *v);

#endif /* YAJL_TREE_H */
/* vim: set sw=2 sts=2 et : */
