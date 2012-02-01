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


#ifndef YAJL_MAX_ARRAY_DIM
#define YAJL_MAX_ARRAY_DIM     8   /* maximum array dimension */
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
  unsigned int  *array_size_ptr;  
  int array_level;
  unsigned int  array_dims;
  unsigned int  array_element_size;
  unsigned int  array_capacity;
  unsigned int  array_size;
  unsigned int  array_s[YAJL_MAX_ARRAY_DIM];  /* size of each dimension*/
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
long long yajl_decl_atoi ( const char *, size_t );

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
#define YAJL_DECL_VALUE_DOUBLE(v,l)   ( v != NULL ? atof(v) : 0 )
#define YAJL_DECL_VALUE_BOOLEAN(v,l) ( v != NULL ? *((int*)v) : 0 )
#define YAJL_DECL_VALUE_INTEGER(v,l) ( v != NULL ? yajl_decl_atoi(v,l) : 0 )
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



/* TODO */
void yajl_decl_callback_array ( void *, const void *, int);
void yajl_set_value_INTEGER ( void *, int, const void *, int );
void yajl_set_value_STRING  ( void *, int, const void *, int );
void yajl_set_value_FLOAT   ( void *, int, const void *, int );
void yajl_set_value_DOUBLE  ( void *, int, const void *, int );
void yajl_set_value_BOOLEAN ( void *, int, const void *, int );
  
#define YAJL_ARRAY_GENERIC_S(_TYPE_, _NAME_, _DIMS_, _SIZE_PTR_)	\
  if ( !strcmp(context->field_name, #_NAME_) )				\
  {									\
    yajl_decl_context *new_context = malloc(sizeof(yajl_decl_context));	\
    new_context->stack = context;					\
    handle->stack = new_context;					\
    new_context->array_size_ptr = NULL;					\
    new_context->array_dims = _DIMS_;					\
    new_context->array_size = 0;					\
    new_context->array_capacity = YAJL_MIN_CAPACITY;			\
    new_context->array_element_size = sizeof( *ptr->_NAME_ );		\
    new_context->array_level = context->array_level;			\
    new_context->array_size_ptr = (unsigned int*) &ptr->_SIZE_PTR_;	\
    ptr->_NAME_ = malloc ( new_context->array_element_size *		\
			   new_context->array_capacity );		\
    new_context->array_cursor = new_context->array_base = ptr->_NAME_;	\
    new_context->callback = yajl_decl_callback_array;			\
    new_context->set_value = yajl_set_value_##_TYPE_;			\
    memset ( new_context->array_s, 0,					\
	     sizeof(unsigned int)*YAJL_MAX_ARRAY_DIM );			\
    new_context->callback ( param, data, size );			\
    return;								\
  }

#define YAJL_ARRAY_GENERIC(_TYPE_, _NAME_, _DIMS_)			\
  if ( !strcmp(context->field_name, #_NAME_) )				\
  {									\
    yajl_decl_context *new_context = malloc(sizeof(yajl_decl_context));	\
    new_context->stack = context;					\
    handle->stack = new_context;					\
    new_context->array_size_ptr = NULL;					\
    new_context->array_dims = _DIMS_;					\
    new_context->array_size = 0;					\
    new_context->array_capacity = YAJL_MIN_CAPACITY;			\
    new_context->array_element_size = sizeof( *ptr->_NAME_ );		\
    new_context->array_level = context->array_level;			\
    ptr->_NAME_ = malloc ( new_context->array_element_size *		\
			   new_context->array_capacity );		\
    new_context->array_cursor = new_context->array_base = ptr->_NAME_;	\
    new_context->callback = yajl_decl_callback_array;			\
    new_context->set_value = yajl_set_value_##_TYPE_;			\
    memset ( new_context->array_s, 0,					\
	     sizeof(unsigned int)*YAJL_MAX_ARRAY_DIM );			\
    new_context->callback ( param, data, size );			\
    return;								\
  }

#define YAJL_ARRAY_TYPE(_TYPE_, _NAME_)		\
  YAJL_ARRAY_GENERIC(_TYPE_,_NAME_, 1)

#define YAJL_ARRAY_TYPE_S(_TYPE_, _NAME_, _SIZE_PTR_)	\
  YAJL_ARRAY_GENERIC_S(_TYPE_,_NAME_,1,_SIZE_PTR_)

#define YAJL_ARRAY_FLOAT(_NAME_)		\
  YAJL_ARRAY_TYPE(FLOAT,_NAME_)
#define YAJL_ARRAY_DOUBLE(_NAME_)		\
  YAJL_ARRAY_TYPE(DOUBLE,_NAME_)
#define YAJL_ARRAY_STRING(_NAME_)		\
  YAJL_ARRAY_TYPE(STRING,_NAME_)
#define YAJL_ARRAY_INTEGER(_NAME_)		\
  YAJL_ARRAY_TYPE(INTEGER,_NAME_)
#define YAJL_ARRAY_BOOLEAN(_NAME_)		\
  YAJL_ARRAY_TYPE(BOOLEAN,_NAME_)
#define YAJL_ARRAY_FLOAT_S(_NAME_,_SIZE_PTR_)	\
  YAJL_ARRAY_TYPE_S(FLOAT,_NAME_,_SIZE_PTR_)
#define YAJL_ARRAY_DOUBLE_S(_NAME_,_SIZE_PTR_)	\
  YAJL_ARRAY_TYPE_S(DOUBLE,_NAME_,_SIZE_PTR_)
#define YAJL_ARRAY_STRING_S(_NAME_,_SIZE_PTR_)	\
  YAJL_ARRAY_TYPE_S(STRING,_NAME_,_SIZE_PTR_)
#define YAJL_ARRAY_INTEGER_S(_NAME_,_SIZE_PTR_)	\
  YAJL_ARRAY_TYPE_S(INTEGER,_NAME_,_SIZE_PTR_)
#define YAJL_ARRAY_BOOLEAN_S(_NAME_,_SIZE_PTR_)	\
  YAJL_ARRAY_TYPE_S(BOOLEAN,_NAME_,_SIZE_PTR_)

#define YAJL_MULTIARRAY_TYPE(_TYPE_, _NAME_, _DIMS_)	\
  YAJL_ARRAY_GENERIC(_TYPE_,_NAME_,_DIMS_)
#define YAJL_MULTIARRAY_TYPE_S(_TYPE_, _NAME_, _DIMS_, _SIZE_PTR_)	\
  YAJL_ARRAY_GENERIC_S(_TYPE_,_NAME_,_DIMS_, _SIZE_PTR_)


#define YAJL_MULTIARRAY_FLOAT(_NAME_,_DIMS_)	\
  YAJL_MULTIARRAY_TYPE(FLOAT, _NAME_, _DIMS_ )
#define YAJL_MULTIARRAY_DOUBLE(_NAME_,_DIMS_)	\
  YAJL_MULTIARRAY_TYPE(DOUBLE, _NAME_, _DIMS_ )
#define YAJL_MULTIARRAY_STRING(_NAME_,_DIMS_)	\
  YAJL_MULTIARRAY_TYPE(STRING, _NAME_, _DIMS_ )
#define YAJL_MULTIARRAY_INTEGER(_NAME_,_DIMS_)	\
  YAJL_MULTIARRAY_TYPE(INTEGER, _NAME_, _DIMS_ )
#define YAJL_MULTIARRAY_BOOLEAN(_NAME_,_DIMS_)	\
  YAJL_MULTIARRAY_TYPE(BOOLEAN, _NAME_, _DIMS_ )

#define YAJL_MULTIARRAY_FLOAT_S(_NAME_,_DIMS_, _SIZE_PTR_)	\
  YAJL_MULTIARRAY_TYPE_S(FLOAT, _NAME_, _DIMS_, _SIZE_PTR_ )
#define YAJL_MULTIARRAY_DOUBLE_S(_NAME_,_DIMS_, _SIZE_PTR_)	\
  YAJL_MULTIARRAY_TYPE_S(DOUBLE, _NAME_, _DIMS_, _SIZE_PTR_ )
#define YAJL_MULTIARRAY_STRING_S(_NAME_,_DIMS_, _SIZE_PTR_)	\
  YAJL_MULTIARRAY_TYPE_S(STRING, _NAME_, _DIMS_, _SIZE_PTR_ )
#define YAJL_MULTIARRAY_INTEGER_S(_NAME_,_DIMS_, _SIZE_PTR_ )	\
  YAJL_MULTIARRAY_TYPE_S(INTEGER, _NAME_, _DIMS_, _SIZE_PTR_  )
#define YAJL_MULTIARRAY_BOOLEAN_S(_NAME_,_DIMS_, _SIZE_PTR_ )	\
  YAJL_MULTIARRAY_TYPE_S(BOOLEAN, _NAME_, _DIMS_, _SIZE_PTR_ )

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

  
