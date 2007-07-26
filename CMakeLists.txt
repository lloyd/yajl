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

PROJECT(YetAnotherJSONParser)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS 1)

IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Debug")
ENDIF ()

SET(CMAKE_C_FLAGS "-Wall")
IF (WIN32) 
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996 /wd4255 /wd4130 /wd4100 /wd4711")
  SET(CMAKE_C_FLAGS_DEBUG "/D DEBUG /Od")
  SET(CMAKE_C_FLAGS_RELEASE "/D NDEBUG /O2")
ELSE ()
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -ansi")
  SET(CMAKE_C_FLAGS_DEBUG "-DDEBUG -g")
  SET(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O2")
ENDIF () 


ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(reformatter)
ADD_SUBDIRECTORY(verify)
