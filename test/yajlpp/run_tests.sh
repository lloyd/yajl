#!/bin/sh

echo Running yajlpp tests:

tests=0
passed=0

for file in `ls`; do
    [ ! -x $file -o -d $file ] && continue
    tests=`expr 1 + $tests`
    printf " test(%s): " $file
    ./$file
    if [ $? -eq 1 ]; then
        passed=`expr 1 + $passed`
        echo 'SUCCESS'
    else
        echo 'FAILURE'
    fi
done

echo "$passed/$tests tests successful"

exit 0
