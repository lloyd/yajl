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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "api/yajl_parse.h"
#include "api/yajl_gen.h"
#include "api/yajl_tree.h"
#include "api/yajl_tree_path.h"

// see ytp_get
static void *ytp_vget(yajl_val n, va_list *pvl)
{	void *v = NULL;

	if(n != NULL)
	{
		switch(n->type)
		{
			// return item primatives, to consumer without
			// consumer needing to know about YAJL_GET_xxx
			case yajl_t_string:	v = n->u.string;	break;
			case yajl_t_number:	v = n->u.number.r;	break;
			case yajl_t_true:	v = (void *)1;	break;
			case yajl_t_false:	v = (void *)1;	break;
			case yajl_t_null:	v = (void *)1;	break;

			// find, then return a named item
			case yajl_t_object:
				{	char const *name = va_arg(*pvl, const char *);

					if(name != NULL)
					{
						// iterate to find the named item
						for(size_t i=0,q=n->u.object.len; i<q && v == NULL; i++)
						{
							if(strcasecmp(name, n->u.object.keys[i]) == 0)
								v = ytp_vget(n->u.object.values[i], pvl);
						}
					}
					else // return the recursed item at the end of the path
						v = n;
				}
				break;

			// return the index item in the array
			case yajl_t_array:
				{	int i = va_arg(*pvl, int);

					if(i && i <= n->u.array.len)
						v = ytp_vget(n->u.array.values[i-1], pvl);
					else // return the recursed item at the end of the path
						v = n;
				}
				break;

			case yajl_t_any: break;
		}
	}

	return v;
}

// The first idea for a recursive yajl_get_tree. I'm
// not all that happy with this as it lacks dyamicism.
//
// yajl_tree_get, cept it will walk the tree using a json path where each segment is a separate argument
// The json path "$.a.y.c[3].d" would be;  ytp_get(root, "a", "b", "c", 3, "d", NULL);
// You must have a tail NULL argument, so the descent parser knows when to stop
//
// Returns the value type at the end of the path.
// If the item at the end of the path is an;
//		yajl_t_object or yajl_t_array, then the item is a yajl_val
//		yajl_t_string or yajl_t_number, then the item is a char *
//		yajl_t_true, yajl_t_false, or yajl_t_null, then the item is a (void *)1
void *ytp_get(yajl_val n, ...)
{	void *v = NULL;

	if(n != NULL)
	{	va_list vl;

		va_start(vl, n);
		v = ytp_vget(n, &vl);
		va_end(vl);
	}

	return v;
}

// strtoi but bound by (l)eft and (r)ight instead of a null termination
// Returns 1 if source string contained only valid digit characters
// The value is stored in the out variable *pI
static int strlrtoi(char const *l, char const *r, size_t *pI)
{	int valid = 0;

	*pI = 0;
	while(l < r && *l && (valid = isdigit(*l)))
	{
		*pI *= 10;
		*pI += (*l - '0');
		l++;
	}

	if(!valid)
		*pI = 0;

	return valid;
}

// The second idea for a  yajl_get_tree that is more suited for
// dynamic runtime conditions, but still not good enough.
// Uses ytp_get() for finding named items.
//
// A very simple json path parser ie. "$.a.b.c[3].d"
// $..item - is parsed, but not implemented
// $.item.* - is parsed, but not implemented
// $.item[range | filter] - is parsed, but not implemented
// $.item[0] - is parsed and handled
//
// This is based on the ideas @ http://goessner.net/articles/JsonPath/
// specifically, this "x.store.book[0].title" path specification style
//
void *ytp_GetPath(yajl_val n, char const *pStr)
{	void *v = NULL;

	if(n != NULL)
	{	char const *l = strstr(pStr, "$."); // make sure we have anchor
		char const *r = NULL;
		int invalid = 0;

		if(l != NULL) // skip the anchor
			l+= 2;
		r = l;

		// path !exauhsted and !invalid
		while(r != NULL && *r && !invalid)
		{
			// find the right bounding edge of this path segment
			while(r != NULL && *r != '.' && *r)
				r++;

			if(*l == '.' && *r == '.') // recursive decent ie "$..author"
			{
				// TODO - build an array of specified items
				invalid = 1;
			}
			else if(*l == '*') // wild card ie "$.store.*" 
			{
				// TODO - build an array of all items
				invalid = 1;
			}
			else if(*l == '[' && *(r-1) == ']') // array ?
			{
				// make sure that we are working with an array item
				invalid = (n->type != yajl_t_array);
				if(!invalid)
				{	size_t i = 0;
					int isInteger = strlrtoi(l+1, r-1, &i);
			
					if(isInteger) // single specified element ie "$.store.book[2].author"
					{
						invalid = !(i < n->u.array.len);
						if(!invalid)
							n = n->u.array.values[i];
					}
					else // TODO - multiple, filtered, bounded, or otherwise specified item set
					// ie "book[(@.length-1)]", "book[-1:]", "book[0,1]", "book[:2]", "book[?(@.isbn)]", etc...
					{
					}
				}
			}
			else // object or other non-array item types
			{	char s[r-l+1];

				memcpy(s, l, r-l);
				s[r-l] = 0;

				n = ytp_get(n, s, NULL);
			}

			// prepare for next path segment ?
			if(r != NULL && *r)
			{
				r++;
				l = r;
			}
		}

		if(!invalid)
			v = n;
	}

	return v;
}

// Dynamic path construction for ytp_GetPath()
void *ytp_GetPathPrintf(yajl_val n, char const *pFmt, ...)
{	void *v = NULL;

	if(n != NULL)
	{	char *pStr = NULL;
		va_list vl;

		va_start(vl, pFmt);
		vasprintf(&pStr, pFmt, vl);

		if(pStr != NULL)
		{
			v = ytp_GetPath(n, pStr);
			free(pStr);
		}
		va_end(vl);
	}

	return v;
}

/*
// strcat but dst is realloc'd to add src
static char *strcatr(char *dst, char *src)
{
	if(src != NULL && *src)
		dst = strcat(realloc(dst, (dst != NULL ? strlen(dst) : 0) + strlen(src) + 1), src);

	return  dst;
}

// ytp_GetPath and strcatr for each path argument
// Probably not as useful as something like ...
// "{$.host}{$.url}{$.%1s.url}{$.%1s.%2s.url}" - convienience function syntax idea - void *ytp_GetPathCatBetter(yajl_val, char const *, ...)
char *ytp_GetPathCat(yajl_val n, char *pStrOut, ...)
{	char const *pStrIn = NULL;
	va_list vl;

	va_start(vl, pStrOut);
	do
	{
		pStrIn = va_arg(vl, char const *);
		if(pStrIn != NULL)
			pStrOut = strcatr(pStrOut, ytp_GetPath(n, pStrIn));

	} while(pStrIn);

	va_end(vl);

	return pStrOut;
}
*/
