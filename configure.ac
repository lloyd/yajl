AC_PREREQ([2.52])
AC_INIT([yajl], [2.0.5], [yajl@librelist.com])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
LT_INIT

AC_PROG_CC

# a lot of cflags
AM_CFLAGS="-std=c99 -pedantic -Wpointer-arith -Wno-format-y2k -Wstrict-prototypes -Wmissing-declarations -Wnested-externs -Wextra  -Wundef -Wwrite-strings -Wold-style-definition -Wredundant-decls -Wno-unused-parameter -Wno-sign-compare -Wmissing-prototypes"

# Checks for header files.
AC_CHECK_HEADERS([stddef.h string.h stdlib.h stdarg.h assert.h stdio.h math.h float.h ctype.h limits.h errno.h])

# Checks for library functions. (There are probably more I should check for?)
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strdup])

AC_SUBST(AM_CFLAGS)

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
