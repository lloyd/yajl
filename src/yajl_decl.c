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

#include <stdlib.h>
#include <string.h>
#include <assert.h>


static int handle_null(void *param)
{  
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
  /*TODO: implement array support */
  return 1;
}  
  
static int handle_end_array(void *param)
{
  /*TODO: implement array support */
  return 1;
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
