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

SET (SRCS json_reformat.c)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/include ${CMAKE_SOURCE_DIR}/include)
SET (CMAKE_EXE_LINKER_FLAGS ${YAJL_EXE_LINKER_FLAGS})

ADD_EXECUTABLE(json_reformat ${SRCS})

IF (DISABLE_SHARED)
  TARGET_LINK_LIBRARIES(json_reformat yajl_s)

  # In some environments, we must explicitly link libm (like qnx,
  # thanks @shahbag)
  IF (NOT WIN32)
      TARGET_LINK_LIBRARIES(json_reformat m)
  ENDIF (NOT WIN32)
ELSE (DISABLE_SHARED)
  TARGET_LINK_LIBRARIES(json_reformat yajl)
ENDIF (DISABLE_SHARED)

ADD_DEPENDENCIES (json_reformat src)
INSTALL(TARGETS json_reformat RUNTIME DESTINATION bin)

# vim: ts=2 sw=2 et:
