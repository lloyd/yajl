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

#ifndef __YAJL_REV_BUF_H__
#define __YAJL_REV_BUF_H__

#include "api/yajl_common.h"
#include "yajl_alloc.h"

/*
 * Implementation/performance notes.  If this were moved to a header
 * only implementation using #define's where possible we might be 
 * able to sqeeze a little performance out of the guy by killing function
 * call overhead.  YMMV.
 */

/**
 * yajl_rev_buf is a buffer with exponential growth.  the buffer ensures that
 * you are always null padded.
 */
typedef struct yajl_rev_buf_t * yajl_rev_buf;

/* allocate a new buffer */
yajl_rev_buf yajl_rev_buf_alloc(yajl_alloc_funcs * alloc);

/* free the buffer */
void yajl_rev_buf_free(yajl_rev_buf rev_buf);

/* append a number of bytes to the buffer */
void yajl_rev_buf_append(yajl_rev_buf rev_buf, const void * data, size_t len);

/* empty the buffer */
void yajl_rev_buf_clear(yajl_rev_buf rev_buf);

/* get a pointer to the beginning of the buffer */
const unsigned char * yajl_rev_buf_data(yajl_rev_buf rev_buf);

/* get the length of the buffer */
size_t yajl_rev_buf_len(yajl_rev_buf rev_buf);

/* truncate the buffer */
void yajl_rev_buf_truncate(yajl_rev_buf rev_buf, size_t len);

#endif
