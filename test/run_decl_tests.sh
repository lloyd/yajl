#!/bin/sh 

ECHO=`which echo`

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
    testBin="../build/test/Release/yajl_decl_test.exe"
    if [ ! -x $testBin ] ; then
        testBin="../build/test/Debug/yajl_decl_test.exe"
        if [ ! -x $testBin ] ; then
            testBin="../build/test/yajl_decl_test"
            if [  ! -x $testBin ] ; then
                ${ECHO} "cannot execute decl_test binary: '$testBin'"  
                exit 1;
            fi
        fi
    fi
fi

${ECHO} "using decl_test binary: $testBin"

testBinShort=`basename $testBin`

testsSucceeded=0
testsTotal=0

$testBin 2>&1
if [ $? -eq 0 ] ; then
  exit 0
fi
exit 1
