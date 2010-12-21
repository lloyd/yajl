EXTRA_DIST = src/YAJL.dxy

dist_doc_DATA = TODO README

lib_LTLIBRARIES = libyail.la

pkginclude_HEADERS = src/api/yajl_parse.h \
		     src/api/yajl_gen.h \
		     src/api/yajl_common.h \
		     src/api/yajl_version.h

libyail_la_SOURCES = src/yajl.c \
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

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = pkgconfig/yail.pc

BUILT_SOURCES = $(top_srcdir)/.version
$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

libyail_la_CPPFLAGS = -DYAJL_BUILD

libyail_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

ACLOCAL_AMFLAGS = -I m4

INCLUDES = -I. -I${top_srcdir}/src -I${top_srcdir}/src/api

