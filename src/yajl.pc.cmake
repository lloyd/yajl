prefix=${pcfiledir}/../..
libdir=${prefix}/lib@LIB_SUFFIX@
includedir=${prefix}/include

Name: Yet Another JSON Library
Description: A Portable JSON parsing and serialization library in ANSI C
Version: @YAJL_MAJOR@.@YAJL_MINOR@.@YAJL_MICRO@
Cflags: -I${includedir}
Libs: -L${libdir} -lyajl
