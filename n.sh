#!/bin/sh
rm -rf build CMakeFiles CMakeCache.txt cmake_install.cmake Makefile
./configure -p $HOME && make && make install
