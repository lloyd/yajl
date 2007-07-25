/*
 * Copyright 2007, Lloyd Hilaiel.
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

#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 2048

int test_yajl_null(void *ctx)
{
    printf("null\n");
    return 1;
}

int test_yajl_boolean(void * ctx, int boolVal)
{
    printf("bool: %s\n", boolVal ? "true" : "false");
    return 1;
}

int test_yajl_integer(void *ctx, long long integerVal)
{
    printf("integer: %lld\n", integerVal);
    return 1;
}

int test_yajl_double(void *ctx, double doubleVal)
{
    printf("double: %lf\n", doubleVal);
    return 1;
}

int test_yajl_string(void *ctx, const unsigned char * stringVal,
                     unsigned int stringLen)
{
    printf("string: '");
    fwrite(stringVal, 1, stringLen, stdout);
    printf("'\n");    
    return 1;
}

int test_yajl_map_key(void *ctx, const unsigned char * stringVal,
                     unsigned int stringLen)
{
    char * str = (char *) malloc(stringLen + 1);
    str[stringLen] = 0;
    memcpy(str, stringVal, stringLen);
    printf("key: '%s'\n", str);
    free(str);
    return 1;
}

int test_yajl_start_map(void *ctx)
{
    printf("map open '{'\n");
    return 1;
}


int test_yajl_end_map(void *ctx)
{
    printf("map close '}'\n");
    return 1;
}

int test_yajl_start_array(void *ctx)
{
    printf("array open '['\n");
    return 1;
}

int test_yajl_end_array(void *ctx)
{
    printf("array close ']'\n");
    return 1;
}

static yajl_callbacks callbacks = {
    test_yajl_null,
    test_yajl_boolean,
    test_yajl_integer,
    test_yajl_double,
    test_yajl_string,
    test_yajl_start_map,
    test_yajl_map_key,
    test_yajl_end_map,
    test_yajl_start_array,
    test_yajl_end_array
};

static void usage(const char * progname)
{
    fprintf(stderr,
            "usage:  %s [options] <filename>\n"
            "   -c  allow comments\n",
            progname);
    exit(1);
}

int 
main(int argc, char ** argv)
{
    yajl_handle hand;
    const char * fileName;
    static unsigned char fileData[BUF_SIZE];
    FILE * fileHand;
    yajl_status stat;
    size_t rd;
    yajl_parser_config cfg = { 0 };

    /* check arguments.  We expect exactly one! */
    if (argc == 3) {
        if (!strcmp("-c", argv[1])) {
            cfg.allowComments = 1;
        } else {
            usage(argv[0]);
        }
    } else if (argc != 2) {
        usage(argv[0]);
    }

    fileName = argv[argc-1];

    fileHand = fopen(fileName, "r");

    if (fileHand == NULL) {
        fprintf(stderr, "couldn't open '%s' for reading\n", fileName);
        exit(1);
    }

    /* ok.  open file.  let's read and parse */
    hand = yajl_alloc(&callbacks, &cfg, NULL);

    do {
        rd = fread((void *) fileData, 1, sizeof(fileData), fileHand);
        
        if (rd < 0) {
            fprintf(stderr, "error reading from '%s'\n", fileName);
            break;
        } else if (rd == 0) {
            break;
        } else {
            /* read file data, pass to parser */
            stat = yajl_parse(hand, fileData, rd);
            if (stat != yajl_status_insufficient_data &&
                stat != yajl_status_ok)
            {
                unsigned char * str = yajl_get_error(hand, 1, fileData, rd);
                fprintf(stderr, (char *) str);
                yajl_free_error(str);
                break;
            }
        }
    } while (1);
    
    yajl_free(hand);
    fclose(fileHand);

    return 0;
}
