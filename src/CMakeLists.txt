# Copyright 2007, Lloyd Hilaiel.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
# 
#  3. Neither the name of Lloyd Hilaiel nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

SET (SRCS yajl.c yajl_lex.c yajl_parser.c yajl_buf.c yajl_encode.c yajl_gen.c)
SET (HDRS yajl_parser.h yajl_lex.h yajl_buf.h yajl_encode.h)
SET (PUB_HDRS api/yajl_parse.h api/yajl_gen.h api/yajl_common.h)

# useful when fixing lexer bugs.
#ADD_DEFINITIONS(-DYAJL_LEXER_DEBUG)

ADD_LIBRARY(yajl_s STATIC ${SRCS} ${HDRS} ${PUB_HDRS})

ADD_LIBRARY(yajl SHARED ${SRCS} ${HDRS} ${PUB_HDRS})

#### setup shared library version number
SET_TARGET_PROPERTIES(yajl PROPERTIES SOVERSION 1 VERSION 1.0)

#### build up an sdk as a post build step

# set up some paths
SET (libDir ${CMAKE_CURRENT_BINARY_DIR}/../dist/lib)
SET (incDir ${CMAKE_CURRENT_BINARY_DIR}/../dist/include/yajl)

# create some directories
FILE(MAKE_DIRECTORY ${libDir})
FILE(MAKE_DIRECTORY ${incDir})

# copy in the library
GET_TARGET_PROPERTY(libPath yajl_s LOCATION)

ADD_CUSTOM_COMMAND(TARGET yajl_s POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${libPath} ${libDir})

GET_TARGET_PROPERTY(libPath yajl LOCATION)

ADD_CUSTOM_COMMAND(TARGET yajl_s POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${libPath} ${libDir})

FOREACH (header ${PUB_HDRS})
  SET (header ${CMAKE_CURRENT_SOURCE_DIR}/${header})

  EXEC_PROGRAM(${CMAKE_COMMAND} ARGS -E copy_if_different ${header} ${incDir})

  ADD_CUSTOM_COMMAND(TARGET yajl_s POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${header} ${incDir})
ENDFOREACH ()

INCLUDE_DIRECTORIES(${incDir}/..)
