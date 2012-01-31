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

#include <yajl/yajl_decl.h>
#include <stdio.h>
#include <assert.h>

#define FALSE  0
#define TRUE   1

#define ASSERT(x,msg) if ( !(x) ) {fprintf(stderr, "%s\n", msg); exit(1);}


void test_adt2 ( void ); 
void test_adt1 ( void );


/* types */
typedef struct
{
  int integer;
  int boolean;
  double real;
  char *string;
  
} adt1;

typedef struct
{
  int integer;
  adt1* object; 
} adt2;


typedef struct
{
  float *list;
} adt3;



/* JSON */
static const char *json_adt1 = "{                                \
                     \"integer\":  1,				 \
                     \"boolean\":  true,			 \
                     \"real\":     2.0,				 \
                     \"string\":   \"string1\"			 \
                   }";


static const char *json_adt2 = "{                                \
                     \"integer\":  3,				 \
                     \"object\":   {				 \
                       \"integer\":  10,			 \
                       \"boolean\":  true,			 \
                       \"real\":     20.0,			 \
                       \"string\":   \"string10\"		 \
                     }						 \
                   }";



static const char *json_adt3 = "{                                \
                     \"list\":  [[1.1, 2.2, 3.3, 4.4],[5.5, 6.6, 7.7, 8.8]]		 \
                   }";


/* DECLARATIONS */

YAJL_OBJECT_BEGIN (adt1)
  YAJL_FIELD_INTEGER(integer);
  YAJL_FIELD_BOOLEAN(boolean);
  YAJL_FIELD_FLOAT(real);
  YAJL_FIELD_STRING(string);
YAJL_OBJECT_END(adt1)

YAJL_OBJECT_BEGIN (adt2)
  YAJL_FIELD_INTEGER(integer);
  YAJL_FIELD_OBJECT(adt1, object);
YAJL_OBJECT_END(adt2)

YAJL_OBJECT_BEGIN (adt3)
  YAJL_ARRAY_FLOAT(list);
YAJL_OBJECT_END(adt3)


/* TEST */
void test_adt1 ( void )
{
  adt1* var = NULL;

  var = YAJL_PARSE_BEGIN (adt1);
  YAJL_PARSE(adt1, (unsigned char*) json_adt1, strlen(json_adt1));
  YAJL_PARSE_END(adt1);

  ASSERT(var != NULL,                              "test_adt1: var == NULL");
  ASSERT(var->integer == 1,                        "test_adt1: var->integer != 1" );
  ASSERT(var->boolean == TRUE,                     "test_adt1: var->boolean != TRUE" );
  ASSERT(var->real == 2.0,                         "test_adt1: var->real != 2.0" );
  ASSERT(!strcmp(var->string, "string1"),          "test_adt1, var->string != \"string1\"" );
}


void test_adt2 ( void )
{
  adt2* var = NULL;
  
  var = YAJL_PARSE_BEGIN (adt2);
  YAJL_PARSE(adt2, (unsigned char*) json_adt2, strlen(json_adt2));
  YAJL_PARSE_END(adt2);

  ASSERT(var != NULL,                              "test_adt2: var == NULL");
  ASSERT(var->integer == 3,                        "test_adt2: var->integer != 3" );
  ASSERT(var->object->integer == 10,               "test_adt2: var->object->integer != 10" );
  ASSERT(var->object->boolean == TRUE,             "test_adt2: var->object->boolean != TRUE" );
  ASSERT(var->object->real == 20.0,                "test_adt2: var->object->real != 20.0" );
  ASSERT(!strcmp(var->object->string, "string10"), "test_adt1, var->object->string != \"string10\"" );
  
}

void test_adt3 ( void )
{
  adt3* var = NULL;
  
  var = YAJL_PARSE_BEGIN (adt3);
  YAJL_PARSE(adt3, (unsigned char*) json_adt3, strlen(json_adt3));
  YAJL_PARSE_END(adt3);
  for ( int i = 0; i < 4; ++i)
    printf("%f\n", var->list[i] );
}


int main ( int argc, char **argv )
{  
  test_adt1 ();
  test_adt2 ();
  test_adt3 ();
  
  return 0;
}
