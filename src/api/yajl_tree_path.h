/*
 * Copyright (c) 2015, Neal Horman, http://www.wanlink.com
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

#ifndef _YAJLTREEPATH_H_
#define _YAJLTREEPATH_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

#include "yajl_parse.h"
#include "yajl_gen.h"
#include "yajl_tree.h"

#ifdef __cplusplus
extern "C" {
#endif

/** yajl_tree_get, cept it will walk the tree using a json path where each segment is a separate argument
 *  The json path "$.a.y.c[3].d" would be;  ytp_get(root, "a", "b", "c", 3, "d", NULL);
 *  You must have a tail NULL argument, so the descent parser knows when to stop
 *  \param n - A yajl_val root node
 *
 *  \returns the value type at the end of the path.
 *  \attention {
 *   All return values must be treated as const imutable.
 *   Failure to do so, will result in altering
 *   the tree, at best, and SEGV at worst!
 *  }
 *  If the item at the end of the path is an;
 *		yajl_t_object or yajl_t_array, then the item is a const yajl_val
 *		yajl_t_string or yajl_t_number, then the item is a char const *
 *		yajl_t_true, yajl_t_false, or yajl_t_null, then the item is a (void const *)1
 */
void *ytp_get(yajl_val n, ...);

/** A very simple json path parser ie. "$.a.b.c[3].d"
 *  $..item - is parsed, but not implemented
 *  $.item.* - is parsed, but not implemented
 *  $.item[range | filter] - is parsed, but not implemented
 *  $.item[0] - is parsed and handled
 *  \param n - A yajl_val root node
 *  \param pPath - A json path specification
 *
 *  \returns the value type at the end of the path. see ytp_get()
 */
void *ytp_GetPath(yajl_val n, char const *pPath);

/** Dynamic path construction for ytp_GetPath()
 *  This builds a json path string using printf(), to be passed to to ytp_GetPath()
 *  \param n - A yajl_val root node
 *  \param pFmt - A printf() format specification string, followed by the required arguments indicated in the format string
 *  \returns the value type at the end of the path. see ytp_get()
 */
void *ytp_GetPathPrintf(yajl_val n, char const *pFmt, ...);

#ifdef __cplusplus
}
#endif

#endif
