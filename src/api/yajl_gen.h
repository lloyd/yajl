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

#include <yajl/yajl_common.h>

#ifndef __YAJL_GEN_H__
#define __YAJL_GEN_H__

#ifdef __cplusplus
extern "C" {
#endif    
    typedef enum {
        yajl_gen_status_ok = 0,
        yajl_gen_keys_must_be_strings,
        yajl_max_depth_exceeded,
        yajl_gen_in_error_state,
        yajl_gen_generation_complete                
    } yajl_gen_status;

    /** an opaque handle to a generator */
    typedef struct yajl_gen_t * yajl_gen;

    /** configuration structure for the generator */
    typedef struct {
        unsigned int beautify;
        const char * indentString;
    } yajl_gen_config;

    /** allocate a generator handle */
    yajl_gen YAJL_API yajl_gen_alloc(const yajl_gen_config * config);

    /** free a generator handle */    
    void YAJL_API yajl_gen_free(yajl_gen handle);

    yajl_gen_status YAJL_API yajl_gen_integer(yajl_gen hand,
                                              long long int number);
    yajl_gen_status YAJL_API yajl_gen_double(yajl_gen hand, double number);
    yajl_gen_status YAJL_API yajl_gen_string(yajl_gen hand,
                                             const unsigned char * str,
                                             unsigned int len);
    yajl_gen_status YAJL_API yajl_gen_null(yajl_gen hand);
    yajl_gen_status YAJL_API yajl_gen_bool(yajl_gen hand, int boolean);    
    yajl_gen_status YAJL_API yajl_gen_map_open(yajl_gen hand);
    yajl_gen_status YAJL_API yajl_gen_map_close(yajl_gen hand);
    yajl_gen_status YAJL_API yajl_gen_array_open(yajl_gen hand);
    yajl_gen_status YAJL_API yajl_gen_array_close(yajl_gen hand);

    yajl_gen_status YAJL_API yajl_gen_get_buf(yajl_gen hand,
                                              const unsigned char ** buf,
                                              unsigned int * len);

    void YAJL_API yajl_gen_clear(yajl_gen hand);

#ifdef __cplusplus
};
#endif    

#endif
