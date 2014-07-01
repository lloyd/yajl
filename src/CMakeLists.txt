# Copyright (c) 2007-2014, Lloyd Hilaiel <me@lloyd.io>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

SET (SRCS
  yajl.c
  yajl_lex.c
  yajl_parser.c
  yajl_buf.c
  yajl_encode.c
  yajl_gen.c
  yajl_alloc.c
  yajl_version.c
)

SET (HDRS
  yajl_parser.h
  yajl_lex.h
  yajl_buf.h
  yajl_encode.h
  yajl_alloc.h
)

SET (PUB_HDRS
  ${CMAKE_SOURCE_DIR}/include/yajl/yajl_parse.h
  ${CMAKE_SOURCE_DIR}/include/yajl/yajl_gen.h
  ${CMAKE_SOURCE_DIR}/include/yajl/yajl_common.h
  ${CMAKE_BINARY_DIR}/include/yajl/yajl_version.h
)

IF(NOT DISABLE_TREE)
  SET (SRCS ${SRCS} yajl_tree.c)
  SET (PUB_HDRS ${PUB_HDRS} ${CMAKE_SOURCE_DIR}/include/yajl/yajl_tree.h)
ENDIF(NOT DISABLE_TREE)

# useful when fixing lexer bugs.
#ADD_DEFINITIONS(-DYAJL_LEXER_DEBUG)

# Ensure defined when building YAJL (as opposed to using it from
# another project).  Used to ensure correct function export when
# building win32 DLL.
ADD_DEFINITIONS(-DYAJL_BUILD)

IF(HAVE_FINITE)
  ADD_DEFINITIONS(-DHAVE_FINITE=1)
ENDIF(HAVE_FINITE)

IF(HAVE__FINITE)
  ADD_DEFINITIONS(-DHAVE__FINITE=1)
ENDIF(HAVE__FINITE)

IF(HAVE_ISNAN)
  ADD_DEFINITIONS(-DHAVE_ISNAN=1)
ENDIF(HAVE_ISNAN)

IF(HAVE__ISNAN)
  ADD_DEFINITIONS(-DHAVE__ISNAN=1)
ENDIF(HAVE__ISNAN)

IF(HAVE_ISINF)
  ADD_DEFINITIONS(-DHAVE_ISINF=1)
ENDIF(HAVE_ISINF)

IF(HAVE_IEEEFP_H)
  ADD_DEFINITIONS(-DHAVE_IEEEFP_H=1)
ENDIF(HAVE_IEEEFP_H)

IF(HAVE_FLOAT_H)
  ADD_DEFINITIONS(-DHAVE_FLOAT_H=1)
ENDIF(HAVE_FLOAT_H)

IF(HAVE_INTTYPES_H)
  ADD_DEFINITIONS(-DHAVE_INTTYPES_H=1)
ENDIF(HAVE_INTTYPES_H)

IF(UNIX)
  IF ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    ADD_DEFINITIONS(-D_BSD_SOURCE)
  ELSEIF("${CMAKE_SYSTEM_NAME}" STREQUAL "HP-UX")
    ADD_DEFINITIONS(-D_POSIX_SOURCE -D_XOPEN_SOURCE)
  ELSEIF ("${CMAKE_SYSTEM_NAME}" STREQUAL "SunOS")
    ADD_DEFINITIONS(-D__EXTENSIONS__)
  ENDIF("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
ENDIF(UNIX)

# generate build-time source
SET(dollar $)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/include/yajl/yajl_version.h.cmake
               ${CMAKE_BINARY_DIR}/include/yajl/yajl_version.h)
CONFIGURE_FILE(yajl.pc.cmake yajl.pc)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include)

IF (NOT DISABLE_STATIC)
  ADD_LIBRARY(yajl_s STATIC ${SRCS} ${HDRS} ${PUB_HDRS})

  IF(UNIX)
    IF(CMAKE_COMPILER_IS_GNUCC)
      SET_TARGET_PROPERTIES(yajl_s PROPERTIES COMPILE_FLAGS "-fPIC")
    ENDIF(CMAKE_COMPILER_IS_GNUCC)

    IF("${CMAKE_SYSTEM_NAME}" STREQUAL "AIX")
      SET(CMAKE_C_ARCHIVE_CREATE
          "<CMAKE_AR> -X32_64 cr <TARGET> <LINK_FLAGS> <OBJECTS>")
    ENDIF("${CMAKE_SYSTEM_NAME}" STREQUAL "AIX")
  ENDIF(UNIX)

  # at build time you may specify the cmake variable LIB_SUFFIX to handle
  # 64-bit systems which use 'lib64'
  INSTALL(TARGETS yajl_s ARCHIVE DESTINATION lib${LIB_SUFFIX})
ENDIF (NOT DISABLE_STATIC)

IF (NOT DISABLE_SHARED)
  ADD_LIBRARY(yajl SHARED ${SRCS} ${HDRS} ${PUB_HDRS})

  IF(UNIX)
    IF("${CMAKE_SYSTEM_NAME}" STREQUAL "HP-UX")
      TARGET_LINK_LIBRARIES (yajl m)
    ENDIF("${CMAKE_SYSTEM_NAME}" STREQUAL "HP-UX")
  ELSEIF(APPLE)
    #### ensure a .dylib has correct absolute installation paths upon installation
    MESSAGE("INSTALL_NAME_DIR: ${CMAKE_INSTALL_PREFIX}/lib")
    SET_TARGET_PROPERTIES(yajl PROPERTIES
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
  ENDIF(UNIX)

  #### setup shared library version number
  SET_TARGET_PROPERTIES(yajl PROPERTIES DEFINE_SYMBOL YAJL_SHARED)

  IF(NOT DISABLE_SO_VERSION)
    SET_TARGET_PROPERTIES(yajl PROPERTIES
                          SOVERSION ${YAJL_MAJOR}
                          VERSION ${YAJL_MAJOR}.${YAJL_MINOR}.${YAJL_MICRO})
  ENDIF(NOT DISABLE_SO_VERSION)

  # at build time you may specify the cmake variable LIB_SUFFIX to handle
  # 64-bit systems which use 'lib64'
  #
  # rpmbuild in Ubuntu 12.04 excludes *.so.* from provides and requires list
  # without executable permission.
  INSTALL(TARGETS yajl
          RUNTIME DESTINATION bin
          LIBRARY DESTINATION lib${LIB_SUFFIX}
                  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                              GROUP_READ GROUP_EXECUTE
                              WORLD_READ WORLD_EXECUTE
          ARCHIVE DESTINATION lib${LIB_SUFFIX})
ENDIF (NOT DISABLE_SHARED)

INSTALL(FILES ${PUB_HDRS} DESTINATION include/yajl)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/yajl.pc DESTINATION share/pkgconfig)

# vim: ts=2 sw=2 et:
