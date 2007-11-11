#!/usr/bin/env bash

testBin="../build/test/yajl_test"

if [[ ! -x $testBin ]] ; then
  echo "cannot execute test binary: '$testBin'"  
  exit 1;
fi

let testsSucceeded=0
let testsTotal=0 

for file in cases/*.json ; do
  allowComments="-c"

  # if the filename starts with dc_, we disallow comments for this test
  if [[ $(basename $file) == dc_* ]] ; then
    allowComments=""
  fi
  echo -n " test case: '$file': "
  let iter=1
  success="success"

  # parse with a read buffer size ranging from 1-31 to stress stream parsing
  while (( $iter < 32 )) && [ $success == "success" ] ; do
    $testBin $allowComments -b $iter < $file > ${file}.test  2>&1
    diff -u ${file}.gold ${file}.test
    if [[ $? == 0 ]] ; then
      if (( $iter == 31 )) ; then let testsSucceeded+=1 ; fi
    else 
      success="FAILURE"
      let iter=32
    fi
    let iter+=1
    rm ${file}.test
  done

  echo $success
  let testsTotal+=1
done

echo $testsSucceeded/$testsTotal tests successful

if [[ $testsSucceeded != $testsTotal ]] ; then
  exit 1
fi

exit 0
