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
	{ "你好", "\\u4F60\\u597D" },
	{ "世界", "\\u4E16\\u754C" },
	{ "你好，世界！", "\\u4F60\\u597D\\uFF0C\\u4E16\\u754C\\uFF01" },
	{ "你好, Hello，World世界！", "\\u4F60\\u597D, Hello\\uFF0CWorld\\u4E16\\u754C\\uFF01" },
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
