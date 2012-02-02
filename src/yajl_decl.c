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


#include "api/yajl_decl.h"
#include "yajl_parser.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

static void yajl_set_array_size ( yajl_decl_context* context );

static int handle_null(void *param)
{
  yajl_decl_context *context;
  
  assert ( param != NULL );

  context = ((yajl_decl_handle*) param)->stack;
  context->callback ( param, NULL, 0 );  
  return 1;
}  
  
static int handle_boolean(void *param, int boolean)
{
  yajl_decl_context *context;
  
  assert ( param != NULL );

  context = ((yajl_decl_handle*) param)->stack;
  context->callback ( param, &boolean, 0 );
  return 1;
}  
  
static int handle_number(void *param, const char *str, size_t len)
{
  yajl_decl_context *context;
  
  assert ( param != NULL );

  context = ((yajl_decl_handle*) param)->stack;
  context->callback ( param, str, len );
  return 1;
}  
  
static int handle_string(void *param, const unsigned char * str, size_t len)
{
  yajl_decl_context *context;
  
  assert ( param != NULL );

  context = ((yajl_decl_handle*) param)->stack;
  context->callback ( param, str, len );
  
  return 1;
}  
  
static int handle_map_key(void *param, const unsigned char *str,  size_t len)  
{  
  yajl_decl_context *context;
  assert ( param != NULL );
    
  context = ((yajl_decl_handle*) param)->stack;
  strncpy ( context->field_name,  (const char*) str,  len );
  context->field_name[len] = '\0';  /*!*/
  return 1;
}  
  
static int handle_start_map(void *param)
{
  yajl_decl_handle *handle;
  yajl_decl_context *context;
  yajl_decl_array_desc *array_desc;
  
  assert ( param != NULL );

  handle = (yajl_decl_handle*) param;
  context = handle->stack;
  
  array_desc = context->u.array_desc;
  if ( array_desc != NULL )
    {
      yajl_decl_context *new_context =				
	yajl_decl_context_create ( array_desc->array_element_handle );	
      new_context->stack = context;					
      new_context->ptr = malloc ( array_desc->array_object_size );			
      memset (new_context->ptr, 0, array_desc->array_object_size );		
      handle->stack = new_context;
    }    
  else
    {
      context->callback ( param, NULL, 0 );     
    }
 
  return 1;
}  
   
static int handle_end_map(void *param)
{
  yajl_decl_handle *handle;
  yajl_decl_context *context = NULL;
  
  assert ( param != NULL );

  handle = (yajl_decl_handle*) param;
  context = handle->stack;
  
  if ( context->stack != NULL )
    {
      handle->stack = context->stack;
      if ( handle->stack->u.array_desc != NULL )
	{
	  handle->stack->callback ( param, &context->ptr, sizeof(context->ptr) );
	}
      
      //handle->stack->stack = NULL;
      if ( context->u.array_desc != NULL ) /*FIXME*/
	free ( context->u.array_desc );
      free(context);
    }
  
  return 1;
}  
  
static int handle_start_array(void *param)
{
  yajl_decl_handle *handle;
  yajl_decl_context *context;
  yajl_decl_array_desc *array_desc;
	
  assert ( param != NULL );

  handle = ( yajl_decl_handle* ) param;
  context = handle->stack;

  assert ( context != NULL );

  array_desc = context->u.array_desc;
  if ( array_desc ) /* we are already parsing an array */
    {
      array_desc->array_level++;
    }
  else
    {
      context->callback(param, NULL, 0);    
    }
  
  return 1;
}  

static int handle_end_array(void *param)
{
  yajl_decl_handle *handle;
  yajl_decl_context *context;
  yajl_decl_array_desc *array_desc;
  
  assert ( param != NULL );

  handle = ( yajl_decl_handle* ) param;
  context = handle->stack;

  array_desc = context->u.array_desc;
  if ( array_desc != NULL )
    {
      array_desc->array_level--;
      if ( array_desc->array_level == -1 )
	{
	  array_desc->array_base = realloc ( array_desc->array_base,
					     array_desc->array_size * array_desc->array_element_size );
	  *((char**)array_desc->array_dest_ptr) = ((char*) array_desc->array_base);
	  if ( array_desc->array_size_ptr != NULL )
	    yajl_set_array_size ( context );            	  
	  handle->stack = context->stack;
	  free(array_desc);
	  free(context);
	}
      else
	{
	  array_desc->array_s[array_desc->array_level]++;
	}
    }
  
  return 1;
}  


static void yajl_set_array_size ( yajl_decl_context* context )
{
  yajl_decl_array_desc *array_desc;
  unsigned int i = 0, sum = 0;
  
  assert ( context != NULL );
  array_desc = context->u.array_desc;

  if ( array_desc != NULL )
    {     
      if ( array_desc->array_dims == 1 )
	{
	  array_desc->array_size_ptr[0] = array_desc->array_size;
	}
      else
	{
	  for ( i = 0; i < array_desc->array_dims-1; ++i)
	    {
	      sum += array_desc->array_s[i];
	      array_desc->array_size_ptr[i] = array_desc->array_s[i];
	    }
	  if ( sum == 0)
	    sum = 1;
	  array_desc->array_size_ptr[array_desc->array_dims-1] =
	  array_desc->array_s[array_desc->array_dims-1] = array_desc->array_size / sum;
	}
    }
}

static const yajl_callbacks callbacks =
  {
    /* null        = */ handle_null,
    /* boolean     = */ handle_boolean,
    /* integer     = */ NULL,
    /* double      = */ NULL,
    /* number      = */ handle_number,
    /* string      = */ handle_string,
    /* start map   = */ handle_start_map,
    /* map key     = */ handle_map_key,
    /* end map     = */ handle_end_map,
    /* start array = */ handle_start_array,
    /* end array   = */ handle_end_array
  };

void yajl_decl_begin_parse ( yajl_decl_handle* handle )
{
  assert(handle != NULL);
  handle->yajl_handle = yajl_alloc(&callbacks, NULL, (void*) handle);
  yajl_config(handle->yajl_handle, yajl_allow_comments, 1);
}

void yajl_decl_end_parse ( yajl_decl_handle* handle )
{
  yajl_decl_context_destroy ( handle->stack );
}


yajl_decl_context* yajl_decl_context_create ( yajl_decl_handle *handle )
{
  yajl_decl_context *context =  malloc (sizeof(yajl_decl_context));  
  if ( context != NULL )
    {
      memset ( context, 0, sizeof(yajl_decl_context ));
      context->callback = handle->callback;
      context->ptr = handle->ptr;
    }
  return context;
}


void yajl_decl_context_destroy ( yajl_decl_context* context )
{
  yajl_decl_context *ptr = context;
  yajl_decl_context *next = NULL;
  for (;;)
    {
      if ( ptr->u.array_desc != NULL )
	free ( ptr->u.array_desc );   
      next = ptr->stack;
      free ( ptr );
      ptr = next;   
      if ( ptr == NULL )
	break;
    }
}

void yajl_set_value_INTEGER ( void *dst, int dst_size, const void *src, int src_size )
{
  long long i = yajl_parse_integer(src, src_size);
  memcpy ( dst, &i, dst_size );  
}

void yajl_set_value_STRING  ( void *dst, int dst_size, const void *src, int src_size )
{
  *((char**)dst) = yajl_strndup ( (const char*) src, src_size );
}

void yajl_set_value_FLOAT   ( void *dst, int dst_size, const void *src, int src_size )
{
  float f= atof(src);
  *((float*)dst) = f;  
}

void yajl_set_value_DOUBLE  ( void *dst, int dst_size, const void *src, int src_size )
{
  double f = atof(src);
  *((double*)dst) = f;
}

void yajl_set_value_BOOLEAN ( void *dst, int dst_size, const void *src, int src_size )
{
  *((int*)dst) = *((int*)src);
}

void yajl_set_value_blob ( void *dst, int dst_size, const void *src, int src_size )
{
  memcpy ( dst, (char*) src, dst_size );
}


void yajl_decl_callback_array ( void *param, const void *data, int size )
{
  yajl_decl_handle *handle = (yajl_decl_handle*) param;  
  yajl_decl_context *context;
  yajl_decl_array_desc *array_desc;

  assert ( handle != NULL );
  
  context = handle->stack;
  assert ( context != NULL );

  array_desc = context->u.array_desc;
  assert ( array_desc != NULL );

  if ( array_desc->array_size == array_desc->array_capacity )
    {
      array_desc->array_capacity *= 2;
      array_desc->array_base = realloc ( array_desc->array_base, array_desc->array_capacity * array_desc->array_element_size );
      array_desc->array_cursor = ((char*) array_desc->array_base) + array_desc->array_size * array_desc->array_element_size;
    }
  array_desc->set_value ( array_desc->array_cursor, array_desc->array_element_size, data, size );
  array_desc->array_cursor = ((char*) array_desc->array_cursor) +  array_desc->array_element_size;
  array_desc->array_size++;  
}

/**
 * Our own strndup implementation
 */
char* yajl_strndup ( const char *src, size_t size )
{
  char *dst = malloc ( (size+1)*sizeof(char) );
  if ( dst != NULL )
    {
      memcpy ( dst, src, sizeof(char) * size );
      dst[size] = 0;
    }
  return dst;
}


long long yajl_decl_atoi ( const char *str, size_t size )
{
  return yajl_parse_integer((const unsigned char*) str, size);
}
