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
#include <yajl/yajl_gen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "documents.h"

/* a platform specific defn' of a function to get a high res time in a
 * portable format */
#ifndef WIN32
#include <sys/time.h>
static double mygettime(void) {
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec + (now.tv_usec / 1000000.0);
}
#else
#define _WIN32 1
#include <windows.h>
static double mygettime(void) {
    long long tval;
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	tval = ft.dwHighDateTime;
	tval <<=32;
	tval |= ft.dwLowDateTime;
	return tval / 10000000.00;
}
#endif

#define TEST_TIME_SECS 3

/* if sample documents have been parsed `times` times, what
 * throughput does this represent?  print to stdout */
static void
print_throughput(long long times, double starttime)
{
    double throughput;
    double now;
    const char * all_units[] = { "B/s", "KB/s", "MB/s", (char *) 0 };
    const char ** units = all_units;
    int i, avg_doc_size = 0;

    now = mygettime();

    for (i = 0; i < num_docs(); i++) avg_doc_size += doc_size(i);
    avg_doc_size /= num_docs();

    throughput = (times * avg_doc_size) / (now - starttime);

    while (*(units + 1) && throughput > 1024) {
        throughput /= 1024;
        units++;
    }

    printf("%g %s", throughput, *units);
}


static int
parse(int validate_utf8)
{
    long long times = 0;
    double starttime;

    starttime = mygettime();

    printf("Parsing speed (with%s UTF8 validation): ",
           validate_utf8 ? "" : "out");
    fflush(stdout);

    for (;;) {
		int i;
        {
            /* we'll run this test for no less than 3 seconds. */
            double now = mygettime();
            if (now - starttime >= TEST_TIME_SECS) break;
        }

        /* parse through 100 documents at a time before kicking out and
         * checking if time has elapsed */
        for (i = 0; i < 100; i++) {
            yajl_handle hand = yajl_alloc(NULL, NULL, NULL);
            yajl_status stat;
            const char ** d;

            yajl_config(hand, yajl_dont_validate_strings, validate_utf8 ? 0 : 1);

            for (d = get_doc(times % num_docs()); *d; d++) {
                stat = yajl_parse(hand, (unsigned char *) *d, strlen(*d));
                if (stat != yajl_status_ok) break;
            }

            stat = yajl_complete_parse(hand);

            if (stat != yajl_status_ok) {
                unsigned char * str =
                    yajl_get_error(hand, 1,
                                   (unsigned char *) *d,
                                   (*d ? strlen(*d) : 0));
                fprintf(stderr, "%s", (const char *) str);
                yajl_free_error(hand, str);
                return 1;
            }
            yajl_free(hand);
            times++;
        }
    }

    print_throughput(times, starttime);
    printf("\n");

    return 0;
}

static int
genRecurse(yajl_gen yg, yajl_val v)
{
    switch (v->type) {
        case yajl_t_string:
            yajl_gen_string(yg, (unsigned char *) v->u.string, strlen(v->u.string));
            break;
        case yajl_t_number:
            yajl_gen_number(yg, v->u.number.r, strlen(v->u.number.r));
            break;
        case yajl_t_object:
            yajl_gen_map_open(yg);
            {
                int i;
                for (i=0; i < v->u.object.len; i++) {
                    const unsigned char * key = (unsigned char *) v->u.object.keys[i];
                    yajl_gen_string(yg, key, strlen((char *) key));
                    genRecurse(yg, v->u.object.values[i]);
                }
            }
            yajl_gen_map_close(yg);
            break;
        case yajl_t_array:
            yajl_gen_array_open(yg);
            {
                int i;
                for (i=0; i < v->u.array.len; i++) {
                    genRecurse(yg, v->u.array.values[i]);
                }
            }
            yajl_gen_array_close(yg);
            break;
        case yajl_t_true:
            yajl_gen_bool(yg, 1);
            break;
        case yajl_t_false:
            yajl_gen_bool(yg, 0);
            break;
        case yajl_t_null:
            yajl_gen_null(yg);
            break;
        default:
            break;
    }

    return 0;
}

static void
noopYAJLPrintFunc(void * v, const char * c, size_t s)
{
    return;
}

static int
doGen(yajl_val n)
{
    yajl_gen yg;

    yg = yajl_gen_alloc(NULL);
    yajl_gen_config(yg, yajl_gen_print_callback, noopYAJLPrintFunc, NULL);
    genRecurse(yg, n);
    yajl_gen_free(yg);

    return 0;
}


static int
gen(void)
{
    long long times = 0;
    double starttime;
    yajl_val * forest;
    int i;
    char ebuf[256];

    starttime = mygettime();

    printf("Stringify speed: ");

    /* first we'll parse all three documents into a trees */
    forest = (yajl_val *) calloc(sizeof(yajl_val *), num_docs());

    for (i=0; i<num_docs(); i++) {
        char * buf;
        const char ** doc;
        int j;
        int used = 0;

        buf = (char *) calloc(1, doc_size(i));
        doc = get_doc(i);
        for (j=0; doc[j] != NULL; j++) {
            memcpy(buf + used, doc[j], strlen(doc[j]));
            used += strlen(doc[j]);
        }
        buf[used] = 0;
        forest[i] = yajl_tree_parse(buf, ebuf, sizeof(ebuf));
        free(buf);
    }

    /* now we'll start stringifying these memory representations of
     * documents */
    for (;;) {
        int i;
        {
            /* we'll run this test for no less than 3 seconds. */
            double now = mygettime();
            if (now - starttime >= TEST_TIME_SECS) break;
        }

        /* parse through 100 documents at a time before kicking out and
         * checking if time has elapsed */
        for (i = 0; i < 100; i++) {
            doGen(forest[times % num_docs()]);
            times++;
        }
    }


    /* free up the parsed documents when we're done */
    for (i=0; i<num_docs(); i++) {
        yajl_tree_free(forest[i]);
    }
    free(forest);

    print_throughput(times, starttime);
    printf("\n");

    return 0;
}


int
main(void)
{
    int rv = 0;

    printf("-- speed tests determine parsing throughput given %d different sample documents --\n",
           num_docs());

    rv = parse(1);
    if (rv != 0) return rv;
    rv = parse(0);
    if (rv != 0) return rv;
    rv = gen();

    return rv;
}

