#include "yajl_encode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct {
	const char* in;
	const char* out;
} table[] = {
	{ "Hello", "Hello" },
	{ "World", "World" },
	{ "Hello World!", "Hello World!" },
	{ "\xe4\xbd\xa0\xe5\xa5\xbd", "\\u4F60\\u597D" },
	{ "\xe4\xb8\x96\xe7\x95\x8c", "\\u4E16\\u754C" },
	{ "\xe4\xbd\xa0\xe5\xa5\xbd\xef\xbc\x8c\xe4\xb8\x96\xe7\x95\x8c\xef\xbc\x81", "\\u4F60\\u597D\\uFF0C\\u4E16\\u754C\\uFF01" },
	{ "\xe4\xbd\xa0\xe5\xa5\xbd, Hello\xef\xbc\x8cWorld\xe4\xb8\x96\xe7\x95\x8c\xef\xbc\x81", "\\u4F60\\u597D, Hello\\uFF0CWorld\\u4E16\\u754C\\uFF01" },
	{ 0, 0 },
};

static void collect_encoded_string (void* ctx, const char* str, size_t len);

int main (int argc, char ** argv)
{
	char buf[512];
	int i;
	for (i = 0; table[i].in; ++i) {
		*buf = 0;
		yajl_string_encode (collect_encoded_string, (void*) buf,
			(const unsigned char*) table[i].in, strlen (table[i].in), 0);

		if (!strcmp (table[i].out, buf)) {
			printf ("OK\n");
		} else {
			printf ("Failed! Expected: %s, Actual: %s\n", table[i].out, buf);
		}
	}
	return 0;
}

void collect_encoded_string (void* ctx, const char* str, size_t len)
{
	char* buf = (char*) ctx;
	strncpy (buf + strlen (buf), str, len)[len] = 0;
}
