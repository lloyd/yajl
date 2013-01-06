# Copyright (c) 2007-2011, Lloyd Hilaiel <lloyd@hilaiel.com>
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

CONFIGURE_FILE(run_tests.sh.in run_tests.sh)

INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}/include
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
)

ADD_EXECUTABLE(yajl_test yajl_test.c)

IF (DISABLE_SHARED)
  TARGET_LINK_LIBRARIES(yajl_test yajl_s)
ELSE (DISABLE_SHARED)
  TARGET_LINK_LIBRARIES(yajl_test yajl)
ENDIF (DISABLE_SHARED)

ADD_DEPENDENCIES (yajl_test src)

# FIXME string_encode_test can not be ran correctly in Windows.
IF (NOT WIN32)
  IF (NOT DISABLE_STATIC)
	  ADD_EXECUTABLE(string_encode_test string_encode_test.c)
	  TARGET_LINK_LIBRARIES(string_encode_test yajl_s)
	  ADD_DEPENDENCIES (string_encode_test src)
  ENDIF (NOT DISABLE_STATIC)
ENDIF (NOT WIN32)

# vim: ts=2 sw=2 et:
