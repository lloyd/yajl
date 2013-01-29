AUTOMAKE_OPTIONS = foreign

libyajldir = $(includedir)/yajl

# These are the headers people who install this should have,
# this will be modified later.
libyajl_HEADERS = yajl/yajl_common.h     \
                  yajl/yajl_gen.h        \
                  yajl/yajl_parse.h      \
                  yajl/yajl_tree.h

# These are the internal headers. Library consumers shouldn't need these.
noinst_HEADERS = yajl_alloc.h     \
                 yajl_buf.h       \
                 yajl_bytestack.h \
                 yajl_encode.h    \
                 yajl_lex.h       \
                 yajl_parser.h

lib_LTLIBRARIES = libyajl.la
libyajl_la_CFLAGS = $(AM_CLFAGS) -I api/
libyajl_la_SOURCES = yajl.c yajl_alloc.c yajl_buf.c yajl_encode.c    \
		     yajl_gen.c yajl_lex.c yajl_parser.c yajl_tree.c
