AC_PREREQ([2.69])
AC_INIT([yajl], [2.1.0], [me@lloyd.io])
AC_CONFIG_SRCDIR([src/yajl.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_INIT_AUTOMAKE([1.11 -Wno-portability foreign tar-ustar no-dist-gzip dist-xz subdir-objects])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])
LT_INIT([disable-shared])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([float.h limits.h stddef.h stdlib.h string.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gettimeofday memset strspn])

AC_CONFIG_FILES([
Makefile
])

AC_OUTPUT
