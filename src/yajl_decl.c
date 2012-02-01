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

static int yajl_set_array_size ( yajl_decl_context* context );

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
  yajl_decl_context *context;

  assert ( param != NULL );

  context = ((yajl_decl_handle*) param)->stack;
  context->callback ( param, NULL, 0 );
  return 1;
}  
   
static int handle_end_map(void *param)
{
  yajl_decl_handle *handle;
  void *old_context = NULL;
  
  assert ( param != NULL );

  handle = (yajl_decl_handle*) param;
 
  if ( handle->stack->stack != NULL )
    {
      old_context = handle->stack;
      handle->stack = handle->stack->stack;
      handle->stack->stack = NULL;
      free(old_context);
    }
  return 1;
}  
  
static int handle_start_array(void *param)
{
  yajl_decl_handle *handle;
  yajl_decl_context *context;
  
  assert ( param != NULL );

  handle = ( yajl_decl_handle* ) param;
  context = handle->stack;
    
  context->array_level++;

  return 1;
}  

static int handle_end_array(void *param)
{
  yajl_decl_handle *handle;
  yajl_decl_context *context;
  yajl_decl_context *old_context;
  
  assert ( param != NULL );

  handle = ( yajl_decl_handle* ) param;
  context = handle->stack;

  context->array_level--;
  if ( context->array_dims > 0 )
    {
      if ( context->array_level == -1 )
	{    
	  yajl_set_array_size ( context );            
	  old_context = handle->stack;
	  handle->stack = handle->stack->stack;
	  handle->stack->stack = NULL;
	  free(old_context);
	}
      else
	{
	  context->array_s[context->array_level]++;
	}
    }
  return 1;
}  


static int yajl_set_array_size ( yajl_decl_context* context )
{
  unsigned int i = 0, sum = 0;
  
  assert ( context != NULL );
  if ( context->array_size_ptr != NULL )
    {
      if ( context->array_dims == 1 )
	{
	  context->array_size_ptr[0] = context->array_size;	  
	}
      else
	{
	  for ( i = 0; i < context->array_dims-1; ++i)
	    {
	      sum += context->array_s[i];
	      context->array_size_ptr[i] = context->array_s[i];
	    }
	  context->array_size_ptr[context->array_dims-1] =
	  context->array_s[context->array_dims-1] = context->array_size / sum;
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
      context->array_level = -1;
    }
  return context;
}


void yajl_decl_context_destroy ( yajl_decl_context* context )
{
  yajl_decl_context *ptr = context;
  yajl_decl_context *next = NULL;
  for (;;)
    {
      if ( ptr->stack == NULL )
	break;
      next = ptr->stack;
      free ( ptr );
      ptr = next;
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

void yajl_decl_callback_array ( void *param, const void *data, int size )
{
  yajl_decl_handle *handle = (yajl_decl_handle*) param;  
  yajl_decl_context *context = handle->stack;
  if ( context->array_size == context->array_capacity )
    {
      context->array_capacity *= 2;
      context->array_base = realloc ( context->array_base, context->array_capacity * context->array_element_size );
      context->array_cursor = ((char*) context->array_base) + context->array_size * context->array_element_size;
    }
  context->set_value ( context->array_cursor, context->array_element_size, data, size );
  context->array_cursor = ((char*) context->array_cursor) +  context->array_element_size;
  context->array_size++;  
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
  return yajl_parse_integer(str, size);
}
