/*
 * Copyright (c) 2012, Daniel Calandria <dcalandria@gmail.com>
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
 * \file yajl_decl.h
 * Preprocessor macros to provide a declarative interface to JSON parsing
 */


#include <yajl/yajl_parse.h>
#include <string.h>
#include <stdlib.h>

#ifndef __YAJL_DECL_H__
#define __YAJL_DECL_H__

#ifdef __cplusplus
extern "C" {
#endif
  
#ifndef YAJL_MAX_KEY_LENGTH
#define YAJL_MAX_KEY_LENGTH    64  /* default max key length */
#endif

#define YAJL_MIN_CAPACITY      2
  
typedef struct _yajl_decl_context
{  
  void (*callback) (void*, const void*, int );
  void *ptr;  
  char field_name[YAJL_MAX_KEY_LENGTH];

  /* array support */
  void *array_base;
  void *array_cursor;
  int array_level;
  unsigned int array_element_size;
  unsigned int array_capacity;
  unsigned int array_size;
  void (*set_value) ( void *, int, const void *, int );
  
  /* pointer to next element */
  struct _yajl_decl_context *stack;  
} yajl_decl_context;


typedef struct
{
  void (*callback) (void*, const void*, int );
  void *ptr;
  yajl_decl_context *stack;
  yajl_handle yajl_handle;
} yajl_decl_handle;


char* yajl_strndup ( const char*, size_t );  /* strndup isn't ANSI C*/

void yajl_decl_begin_parse ( yajl_decl_handle* );
void yajl_decl_end_parse ( yajl_decl_handle* );
yajl_decl_context* yajl_decl_context_create  (yajl_decl_handle*);
void yajl_decl_context_destroy ( yajl_decl_context* context);

#define YAJL_OBJECT_BEGIN(_NAME_)					\
  static void _decl_callback_##_NAME_ ( void*, const void*, int );	\
  static yajl_decl_handle _decl_handle_##_NAME_ = {			\
    _decl_callback_##_NAME_,						\
    NULL,								\
    NULL,								\
    NULL,								\
  };									\
  static void _decl_callback_##_NAME_					\
  ( void *param, const void *data, int size )				\
  {									\
    yajl_decl_handle *handle = (yajl_decl_handle*) param;		\
    yajl_decl_context *context = handle->stack;				\
    _NAME_ *ptr = (_NAME_ *) context->ptr;				
  
  
#define YAJL_OBJECT_END(_NAME_)                                         \
  }

  

#define YAJL_DECL_VALUE_FLOAT(v,l)   ( v != NULL ? atof(v) : 0 )
#define YAJL_DECL_VALUE_BOOLEAN(v,l) ( v != NULL ? *((int*)v) : 0 )
#define YAJL_DECL_VALUE_INTEGER(v,l) ( v != NULL ? atoi(v) : 0 )
#define YAJL_DECL_VALUE_STRING(v,l)  ( v != NULL ? yajl_strndup(v,l) : 0 )
  
#define YAJL_FIELD_TYPE(_TYPE_, _NAME_)			        \
  if ( !strcmp(context->field_name, #_NAME_) )			\
    {								\
      ptr->_NAME_ = YAJL_DECL_VALUE_##_TYPE_ (data, size);	\
      return;							\
    }

#define YAJL_FIELD_FLOAT(_NAME_)     YAJL_FIELD_TYPE(FLOAT,_NAME_)
#define YAJL_FIELD_STRING(_NAME_)    YAJL_FIELD_TYPE(STRING,_NAME_)
#define YAJL_FIELD_INTEGER(_NAME_)   YAJL_FIELD_TYPE(INTEGER,_NAME_)
#define YAJL_FIELD_BOOLEAN(_NAME_)   YAJL_FIELD_TYPE(BOOLEAN,_NAME_)
#define YAJL_FIELD_OBJECT(_TYPE_,_NAME_)				\
  if ( !strcmp(context->field_name, #_NAME_) )				\
    {									\
      yajl_decl_context *new_context =					\
	yajl_decl_context_create ( &_decl_handle_##_TYPE_ );		\
      new_context->stack = context;					\
      new_context->ptr = malloc ( sizeof(_TYPE_) );			\
      memset (new_context->ptr, 0, sizeof(_TYPE_) );			\
      handle->stack = new_context;					\
      ptr->_NAME_ = new_context->ptr;					\
      return;								\
    }									

#define YAJL_PARSE_BEGIN(_NAME_)                                        \
  _decl_handle_##_NAME_.ptr = malloc ( sizeof(_NAME_) );		\
  memset (_decl_handle_##_NAME_.ptr, 0, sizeof(_NAME_) );		\
  _decl_handle_##_NAME_.stack =						\
    yajl_decl_context_create ( &_decl_handle_##_NAME_  );		\
  yajl_decl_begin_parse ( &_decl_handle_##_NAME_ );			

  
#define YAJL_PARSE_END(_NAME_)						\
  yajl_complete_parse(_decl_handle_##_NAME_.yajl_handle);		\
  yajl_free(_decl_handle_##_NAME_.yajl_handle);				\
  yajl_decl_end_parse ( &_decl_handle_##_NAME_ );

#define YAJL_PARSE(_NAME_, _STR_, _LEN_)			        \
  yajl_parse(_decl_handle_##_NAME_.yajl_handle, _STR_, _LEN_ );
  
#ifdef __cplusplus
}
#endif

#endif

  
