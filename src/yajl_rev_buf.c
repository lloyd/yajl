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

#include "yajl_rev_buf.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define YAJL_BUF_INIT_SIZE 2048

struct yajl_rev_buf_t {
    size_t len;
    size_t used;
    unsigned char * data;
    yajl_alloc_funcs * alloc;
};

static
void yajl_rev_buf_ensure_available(yajl_rev_buf rev_buf, size_t want)
{
    size_t need, have;
    
    assert(rev_buf != NULL);

    /* first call */
    if (rev_buf->len == 0) {
        rev_buf->len = YAJL_BUF_INIT_SIZE;
        rev_buf->used = YAJL_BUF_INIT_SIZE;
        rev_buf->data = (unsigned char *) YA_MALLOC(rev_buf->alloc, rev_buf->len);
        rev_buf->data[YAJL_BUF_INIT_SIZE - 1] = 0;
    }

    need = rev_buf->len;
    have = rev_buf->len - rev_buf->used;

    while (want >= (need - have)) need <<= 1;

    if (need != rev_buf->len) {
        rev_buf->data = (unsigned char *) YA_REALLOC(rev_buf->alloc, rev_buf->data, need);
        memcpy(rev_buf->data + need - have - 1, rev_buf->data + rev_buf->used - 1, have + 1);
        rev_buf->len = need;
        rev_buf->used = need - have;
    }
}

yajl_rev_buf yajl_rev_buf_alloc(yajl_alloc_funcs * alloc)
{
    yajl_rev_buf b = YA_MALLOC(alloc, sizeof(struct yajl_rev_buf_t));
    memset((void *) b, 0, sizeof(struct yajl_rev_buf_t));
    b->data = (unsigned char *) "" + 1;
    b->alloc = alloc;
    return b;
}

void yajl_rev_buf_free(yajl_rev_buf rev_buf)
{
    assert(rev_buf != NULL);
    if (rev_buf->len) YA_FREE(rev_buf->alloc, rev_buf->data);
    YA_FREE(rev_buf->alloc, rev_buf);
}

void yajl_rev_buf_append(yajl_rev_buf rev_buf, const void * data, size_t len)
{
    yajl_rev_buf_ensure_available(rev_buf, len);
    if (len > 0) {
        assert(rev_buf->len);
        rev_buf->used -= len;
        memcpy(rev_buf->data + rev_buf->used - 1, data, len);
    }
}

void yajl_rev_buf_clear(yajl_rev_buf rev_buf)
{
    rev_buf->used = rev_buf->len;
}

const unsigned char * yajl_rev_buf_data(yajl_rev_buf rev_buf)
{
    return rev_buf->data + rev_buf->used - 1;
}

size_t yajl_rev_buf_len(yajl_rev_buf rev_buf)
{
    return rev_buf->len - rev_buf->used;
}

void
yajl_rev_buf_truncate(yajl_rev_buf rev_buf, size_t len)
{
    assert(len <= rev_buf->used);
    rev_buf->used = rev_buf->used - len;
}
