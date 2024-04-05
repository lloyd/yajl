noinst_LTLIBRARIES = libyajl.la

EXTRA_DIST = src/headers src/api autogen.sh

libyajl_la_CPPFLAGS = -DNDEBUG
libyajl_la_CFLAGS = -I $(abs_srcdir)/src/headers
libyajl_la_SOURCES = \
	src/yajl_alloc.h \
	src/yajl_bytestack.h \
	src/yajl_lex.h \
	src/yajl_buf.h \
	src/yajl_encode.h \
	src/yajl_parser.h \
	src/yajl_alloc.c \
	src/yajl.c \
	src/yajl_gen.c \
	src/yajl_parser.c \
	src/yajl_buf.c \
	src/yajl_encode.c \
	src/yajl_lex.c \
	src/yajl_tree.c

noinst_PROGRAMS = verify/json_verify example/parse_config

verify_json_verify_SOURCES = verify/json_verify.c
verify_json_verify_CFLAGS = -I $(srcdir)/src/headers
verify_json_verify_LDADD = libyajl.la

example_parse_config_SOURCES = example/parse_config.c
example_parse_config_CFLAGS = -I $(srcdir)/src/headers
example_parse_config_LDADD = libyajl.la
