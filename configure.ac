AC_PREREQ(2.61)
AC_INIT([yajl], [m4_esyscmd([./git-version-gen .tarball-version])],
        [http://lloyd.github.com/yajl])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([dist-bzip2 no-dist-gzip foreign subdir-objects])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_MAINTAINER_MODE

AC_PROG_CC_C99

LT_INIT

AC_CONFIG_HEADER([config.h])

AC_CONFIG_MACRO_DIR([m4])

LT_CURRENT=1
LT_REVISION=0
LT_AGE=0

YAJL_MAJOR=1
YAJL_MINOR=0
YAJL_MICRO=11

dnl VERSION="${LT_CURRENT}.${LT_REVISION}.${LT_AGE}"

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_SUBST(YAJL_MAJOR)
AC_SUBST(YAJL_MINOR)
AC_SUBST(YAJL_MICRO)

AC_CONFIG_FILES([Makefile
                 src/yajl/yajl_version.h
                 pkgconfig/yajl.pc
                 pkgconfig/yajl-uninstalled.pc])

AC_OUTPUT

