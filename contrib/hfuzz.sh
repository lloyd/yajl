#!/bin/sh

./configure CC=hfuzz-clang  CPPFLAGS='-D FUZZER' CFLAGS="-ggdb3"
make
VALIDATE_FUZZ=1 honggfuzz --linux_perf_instr --threads 4  -i ./test/parsing/cases/ -- verify/json_verify
