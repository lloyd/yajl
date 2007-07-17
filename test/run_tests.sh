#!/usr/bin/env bash

testBin="../build/test/yajl_test"

if [[ ! -x $testBin ]] ; then
  echo "cannot execute test binary: '$testBin'"  
  exit 1;
fi

let testsSucceeded=0
let testsTotal=0 

for file in cases/*.json ; do
  echo -n " test case: '$file': "
  $testBin $file > ${file}.test  2>&1
  diff -u ${file}.gold ${file}.test
  if [[ $? == 0 ]] ; then
    let testsSucceeded+=1
    echo  " success"
  else 
    echo  " -- FAILURE!"
  fi
  let testsTotal+=1
  rm ${file}.test
done

echo $testsSucceeded/$testsTotal tests successful

if [[ $testsSucceeded != $testsTotal ]] ; then
  exit 1
fi

exit 0
