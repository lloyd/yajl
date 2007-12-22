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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
usage(const char * progname)
{
    fprintf(stderr, "%s: validate json from stdin\n"
                    "usage: json_verify [options]\n"
                    "    -q quiet mode\n"
                    "    -c allow comments\n"
                    "    -u allow invalid utf8 inside strings\n",
            progname);
    exit(1);
}

int 
main(int argc, char ** argv)
{
    yajl_status stat;
    size_t rd;
    yajl_handle hand;
    static unsigned char fileData[65536];
    int quiet = 0;
	int retval = 0;
    yajl_parser_config cfg = { 0, 1 };

    /* check arguments.*/
    if (argc > 1 && argc < 4) {
        int i;

        for (i=1; i < argc;i++) {
            if (!strcmp("-q", argv[i])) {
                quiet = 1;
            } else if (!strcmp("-c", argv[i])) {
                cfg.allowComments = 1;
            } else if (!strcmp("-u", argv[i])) {
                cfg.checkUTF8 = 0;
            } else {
                fprintf(stderr, "unrecognized option: '%s'\n\n", argv[i]);
                usage(argv[0]);
            }
        }
    } else if (argc != 1) {
        usage(argv[0]);
    }
    
    /* allocate a parser */
    hand = yajl_alloc(NULL, &cfg, NULL);
        
    for (;;) {
        rd = fread((void *) fileData, 1, sizeof(fileData) - 1, stdin);

        retval = 0;
        
        if (rd == 0) {
            if (feof(stdin)) {
                break;
            } else {
                if (!quiet) {
                    fprintf(stderr, "error encountered on file read\n");
                }
                retval = 1;
                break;
            }
        } else {
            fileData[rd] = 0;
            
            /* read file data, pass to parser */
            stat = yajl_parse(hand, fileData, rd);
            if (stat != yajl_status_ok &&
                stat != yajl_status_insufficient_data)
            {
                if (!quiet) {
                    unsigned char * str = yajl_get_error(hand, 1, fileData, rd);
                    fprintf(stderr, (const char *) str);
                    yajl_free_error(str);
                }
                retval = 1;
                break;
            }
        }
    }
    
    yajl_free(hand);

    if (!quiet) {
        printf("JSON is %s\n", retval ? "invalid" : "valid");
    }
    
    return retval;
}
