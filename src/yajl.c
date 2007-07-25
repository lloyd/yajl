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

#include "api/yajl_parse.h"
#include "yajl_lex.h"
#include "yajl_parser.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

const char *
yajl_status_to_string(yajl_status code)
{
    /* XXX */
    return NULL;
}

yajl_handle
yajl_alloc(const yajl_callbacks * callbacks,
           const yajl_parser_config * config,
           void * ctx)
{
    unsigned int allowComments = 0;
    
    if (config != NULL) {
        allowComments = config->allowComments;
    }

    yajl_handle hand = (yajl_handle) malloc(sizeof(struct yajl_handle_t));

    hand->callbacks = callbacks;
    hand->ctx = ctx;
    hand->lexer = yajl_lex_alloc(allowComments);
    hand->errorOffset = 0;
    hand->decodeBuf = yajl_buf_alloc();
    hand->stateBuf = yajl_buf_alloc();

    yajl_state_push(hand, yajl_state_start);    

    return hand;
}

void
yajl_free(yajl_handle handle)
{
    yajl_buf_free(handle->stateBuf);
    yajl_buf_free(handle->decodeBuf);
    yajl_lex_free(handle->lexer);
    free(handle);
}

yajl_status
yajl_parse(yajl_handle hand, const unsigned char * jsonText,
           unsigned int jsonTextLen)
{
    unsigned int offset = 0;
    yajl_status status;
    status = yajl_do_parse(hand, &offset, jsonText, jsonTextLen);
    return status;
}

unsigned char *
yajl_get_error(yajl_handle hand, int verbose,
               const unsigned char * jsonText, unsigned int jsonTextLen)
{
    return yajl_render_error_string(hand, jsonText, jsonTextLen, verbose);
}

void
yajl_free_error(unsigned char * str)
{
    /* XXX: use memory allocation functions if set */
    free(str);
}

/* XXX: add utility routines to parse from file */
