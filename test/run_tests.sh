#!/bin/sh

DIFF_FLAGS="-u"
case "$(uname)" in
  *W32*)
    DIFF_FLAGS="-wu"
    ;;
esac

if [ -z "$testBin" ]; then
    testBin="$1"
fi

# find test binary on both platforms.  allow the caller to force a
# particular test binary (useful for non-cmake build systems).
if [ -z "$testBin" ]; then
	testBin="../build/test/Debug/yajl_test.exe"
	if [ ! -x $testBin ] ; then
	  testBin="../build/test/yajl_test"
	  if [  ! -x $testBin ] ; then
	    echo "cannot execute test binary: '$testBin'"  
	    exit 1;
	  fi
	fi
fi

echo "using test binary: $testBin"

testsSucceeded=0
testsTotal=0 

for file in cases/*.json ; do
  allowComments="-c"

  # if the filename starts with dc_, we disallow comments for this test
  case $(basename $file) in
    dc_*)
      allowComments=""
    ;;
  esac
  echo -n " test case: '$file': "
  iter=1
  success="success"

  echo "$testBin $allowComments -b $iter < $file > ${file}.test "
  # parse with a read buffer size ranging from 1-31 to stress stream parsing
  while [ $iter -lt 32  ] && [ $success = "success" ] ; do
    $testBin $allowComments -b $iter < $file > ${file}.test  2>&1
    diff ${DIFF_FLAGS} ${file}.gold ${file}.test
    if [ $? -eq 0 ] ; then
      if [ $iter -eq 31 ] ; then : $(( testsSucceeded += 1)) ; fi
    else 
      success="FAILURE"
      iter=32
    fi
    : $(( iter += 1 ))
    rm ${file}.test
  done

  echo $success
  : $(( testsTotal += 1 ))
done

echo $testsSucceeded/$testsTotal tests successful

if [ $testsSucceeded != $testsTotal ] ; then
  exit 1
fi

exit 0
