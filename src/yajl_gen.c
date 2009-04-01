/*
 * Copyright 2007-2009, Lloyd Hilaiel.
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

#include "api/yajl_gen.h"
#include "yajl_buf.h"
#include "yajl_encode.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef enum {
    yajl_gen_start,
    yajl_gen_map_start,
    yajl_gen_map_key,
    yajl_gen_map_val,
    yajl_gen_array_start,
    yajl_gen_in_array,
    yajl_gen_complete,
    yajl_gen_error
} yajl_gen_state;

struct yajl_gen_t 
{
    unsigned int depth;
    unsigned int pretty;
    const char * indentString;
    yajl_gen_state state[YAJL_MAX_DEPTH];
    yajl_buf buf;
    /* memory allocation routines */
    yajl_alloc_funcs alloc;
};

yajl_gen
yajl_gen_alloc(const yajl_gen_config * config,
               const yajl_alloc_funcs * afs)
{
    yajl_gen g = NULL;
    yajl_alloc_funcs afsBuffer;

    /* first order of business is to set up memory allocation routines */
    if (afs != NULL) {
        if (afs->malloc == NULL || afs->realloc == NULL || afs->free == NULL)
        {
            return NULL;
        }
    } else {
        yajl_set_default_alloc_funcs(&afsBuffer);
        afs = &afsBuffer;
    }

    g = (yajl_gen) YA_MALLOC(afs, sizeof(struct yajl_gen_t));
    memset((void *) g, 0, sizeof(struct yajl_gen_t));
    /* copy in pointers to allocation routines */
    memcpy((void *) &(g->alloc), (void *) afs, sizeof(yajl_alloc_funcs));

    if (config) {
        g->pretty = config->beautify;
        g->indentString = config->indentString ? config->indentString : "  ";
    }
    g->buf = yajl_buf_alloc(&(g->alloc));

    return g;
}

void
yajl_gen_free(yajl_gen g)
{
    yajl_buf_free(g->buf);
    YA_FREE(&(g->alloc), g);
}

#define INSERT_SEP \
    if (g->state[g->depth] == yajl_gen_map_key ||               \
        g->state[g->depth] == yajl_gen_in_array) {              \
        yajl_buf_append(g->buf, ",", 1);                        \
        if (g->pretty) yajl_buf_append(g->buf, "\n", 1);        \
    } else if (g->state[g->depth] == yajl_gen_map_val) {        \
        yajl_buf_append(g->buf, ":", 1);                        \
        if (g->pretty) yajl_buf_append(g->buf, " ", 1);         \
   } 

#define INSERT_WHITESPACE                                               \
    if (g->pretty) {                                                    \
        if (g->state[g->depth] != yajl_gen_map_val) {                   \
            unsigned int _i;                                            \
            for (_i=0;_i<g->depth;_i++)                                 \
                yajl_buf_append(g->buf, g->indentString,                \
                                strlen(g->indentString));               \
        }                                                               \
    }

#define ENSURE_NOT_KEY \
    if (g->state[g->depth] == yajl_gen_map_key) {   \
        return yajl_gen_keys_must_be_strings;       \
    }                                               \

/* check that we're not complete, or in error state.  in a valid state
 * to be generating */
#define ENSURE_VALID_STATE \
    if (g->state[g->depth] == yajl_gen_error) {   \
        return yajl_gen_in_error_state;\
    } else if (g->state[g->depth] == yajl_gen_complete) {   \
        return yajl_gen_generation_complete;                \
    }

#define INCREMENT_DEPTH \
    if (++(g->depth) >= YAJL_MAX_DEPTH) return yajl_max_depth_exceeded;

#define APPENDED_ATOM \
    switch (g->state[g->depth]) {                   \
        case yajl_gen_start:                        \
            g->state[g->depth] = yajl_gen_complete; \
            break;                                  \
        case yajl_gen_map_start:                    \
        case yajl_gen_map_key:                      \
            g->state[g->depth] = yajl_gen_map_val;  \
            break;                                  \
        case yajl_gen_array_start:                  \
            g->state[g->depth] = yajl_gen_in_array; \
            break;                                  \
        case yajl_gen_map_val:                      \
            g->state[g->depth] = yajl_gen_map_key;  \
            break;                                  \
        default:                                    \
            break;                                  \
    }                                               \

#define FINAL_NEWLINE                                        \
    if (g->pretty && g->state[g->depth] == yajl_gen_complete) \
        yajl_buf_append(g->buf, "\n", 1);        
    
yajl_gen_status
yajl_gen_integer(yajl_gen g, long int number)
{
    char i[32];
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    sprintf(i, "%ld", number);
    yajl_buf_append(g->buf, i, strlen(i));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_double(yajl_gen g, double number)
{
    char i[32];
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    sprintf(i, "%g", number);
    yajl_buf_append(g->buf, i, strlen(i));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_number(yajl_gen g, const char * s, unsigned int l)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    yajl_buf_append(g->buf, s, l);
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_string(yajl_gen g, const unsigned char * str,
                unsigned int len)
{
    ENSURE_VALID_STATE; INSERT_SEP; INSERT_WHITESPACE;
    yajl_buf_append(g->buf, "\"", 1);
    yajl_string_encode(g->buf, str, len);
    yajl_buf_append(g->buf, "\"", 1);
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_null(yajl_gen g)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    yajl_buf_append(g->buf, "null", strlen("null"));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_bool(yajl_gen g, int boolean)
{
    const char * val = boolean ? "true" : "false";

	ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    yajl_buf_append(g->buf, val, strlen(val));
    APPENDED_ATOM;
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_map_open(yajl_gen g)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    INCREMENT_DEPTH; 
    
    g->state[g->depth] = yajl_gen_map_start;
    yajl_buf_append(g->buf, "{", 1);
    if (g->pretty) yajl_buf_append(g->buf, "\n", 1);
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_map_close(yajl_gen g)
{
    ENSURE_VALID_STATE; 
    (g->depth)--;
    if (g->pretty) yajl_buf_append(g->buf, "\n", 1);
    APPENDED_ATOM;
    INSERT_WHITESPACE;
    yajl_buf_append(g->buf, "}", 1);
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_array_open(yajl_gen g)
{
    ENSURE_VALID_STATE; ENSURE_NOT_KEY; INSERT_SEP; INSERT_WHITESPACE;
    INCREMENT_DEPTH; 
    g->state[g->depth] = yajl_gen_array_start;
    yajl_buf_append(g->buf, "[", 1);
    if (g->pretty) yajl_buf_append(g->buf, "\n", 1);
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_array_close(yajl_gen g)
{
    ENSURE_VALID_STATE;
    if (g->pretty) yajl_buf_append(g->buf, "\n", 1);
    (g->depth)--;
    APPENDED_ATOM;
    INSERT_WHITESPACE;
    yajl_buf_append(g->buf, "]", 1);
    FINAL_NEWLINE;
    return yajl_gen_status_ok;
}

yajl_gen_status
yajl_gen_get_buf(yajl_gen g, const unsigned char ** buf,
                 unsigned int * len)
{
    *buf = yajl_buf_data(g->buf);
    *len = yajl_buf_len(g->buf);
    return yajl_gen_status_ok;
}

void
yajl_gen_clear(yajl_gen g)
{
    yajl_buf_clear(g->buf);
}
