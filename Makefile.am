EXTRA_DIST = src/YAJL.dxy

dist_doc_DATA = TODO README

lib_LTLIBRARIES = libyajl.la

pkginclude_HEADERS = src/yajl/yajl_parse.h \
		     src/yajl/yajl_gen.h \
		     src/yajl/yajl_common.h \
		     src/yajl/yajl_version.h

libyajl_la_SOURCES = src/yajl.c \
		     src/yajl_lex.c \
		     src/yajl_parser.c \
		     src/yajl_buf.c \
		     src/yajl_encode.c \
		     src/yajl_gen.c \
		     src/yajl_alloc.c \
		     src/yajl_version.c \
		     src/yajl_parser.h \
		     src/yajl_lex.h \
		     src/yajl_buf.h \
		     src/yajl_encode.h \
		     src/yajl_alloc.h

bin_PROGRAMS = json_reformat \
	       json_verify

json_reformat_SOURCES = reformatter/json_reformat.c

json_reformat_LDADD = libyajl.la

json_verify_SOURCES = verify/json_verify.c

json_verify_LDADD = libyajl.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pkgconfig/yajl.pc

BUILT_SOURCES = $(top_srcdir)/.version
$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

libyajl_la_CPPFLAGS = -DYAJL_BUILD

libyajl_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

ACLOCAL_AMFLAGS = -I m4

INCLUDES = -I. -I${top_srcdir}/src -I${top_srcdir}/src/yajl

