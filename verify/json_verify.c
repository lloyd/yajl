/*
 * Copyright (c) 2007-2014, Lloyd Hilaiel <me@lloyd.io>
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

#include <yajl/yajl_parse.h>
#include <yajl/yajl_tree.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#ifdef FUZZER
int
LLVMFuzzerInitialize(int *argc, char ***argv)
{
  return 0;
}

int
LLVMFuzzerTestOneInput(uint8_t *buf, size_t len)
{
  yajl_val tree;
  yajl_handle hand;
  char err[1024];
  char *str = malloc(len+1);

  memcpy(str, buf, len);
  str[len] = '\0';

  hand = yajl_alloc(NULL, NULL, NULL);
  yajl_parse(hand, buf, len);
  yajl_free(hand);

  /* make sure it is NUL terminated.  */
  tree = yajl_tree_parse(str, err, sizeof(err));
  if (tree)
    yajl_tree_free(tree);
  free(str);
  return 0;
}
#endif

static void
usage(const char * progname)
{
    fprintf(stderr, "%s: validate json from stdin\n"
                    "usage: json_verify [options]\n"
                    "    -c allow comments\n"
                    "    -q quiet mode\n"
                    "    -s verify a stream of multiple json entities\n"
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
    int a = 1;

#ifdef FUZZER
  if (getenv("VALIDATE_FUZZ")) {
    extern void HF_ITER(uint8_t** buf, size_t* len);
    for (;;) {
      size_t len;
      uint8_t *buf;

      HF_ITER(&buf, &len);
  
      LLVMFuzzerTestOneInput(buf, len);
    }
  }
#endif

    /* allocate a parser */
    hand = yajl_alloc(NULL, NULL, NULL);

    /* check arguments.*/
    while ((a < argc) && (argv[a][0] == '-') && (strlen(argv[a]) > 1)) {
        unsigned int i;
        for ( i=1; i < strlen(argv[a]); i++) {
            switch (argv[a][i]) {
                case 'q':
                    quiet = 1;
                    break;
                case 'c':
                    yajl_config(hand, yajl_allow_comments, 1);
                    break;
                case 'u':
                    yajl_config(hand, yajl_dont_validate_strings, 1);
                    break;
                case 's':
                    yajl_config(hand, yajl_allow_multiple_values, 1);
                    break;
                default:
                    fprintf(stderr, "unrecognized option: '%c'\n\n", argv[a][i]);
                    usage(argv[0]);
            }
        }
        ++a;
    }
    if (a < argc) {
        usage(argv[0]);
    }

    for (;;) {
        rd = fread((void *) fileData, 1, sizeof(fileData) - 1, stdin);

        retval = 0;

        if (rd == 0) {
            if (!feof(stdin)) {
                if (!quiet) {
                    fprintf(stderr, "error encountered on file read\n");
                }
                retval = 1;
            }
            break;
        }
        fileData[rd] = 0;

        /* read file data, pass to parser */
        stat = yajl_parse(hand, fileData, rd);

        if (stat != yajl_status_ok) break;
    }

    /* parse any remaining buffered data */
    stat = yajl_complete_parse(hand);

    if (stat != yajl_status_ok)
    {
        if (!quiet) {
            unsigned char * str = yajl_get_error(hand, 1, fileData, rd);
            fprintf(stderr, "%s", (const char *) str);
            yajl_free_error(hand, str);
        }
        retval = 1;
    }

    yajl_free(hand);

    if (!quiet) {
        printf("JSON is %s\n", retval ? "invalid" : "valid");
    }

    return retval;
}
