#!/bin/sh

cd tests

pytest \
  -rA \
  --junitxml="$EXT_MOUNT/result.xml" \
  --html="$EXT_MOUNT/report.html" \
  --self-contained-html \
  ocpp_tests/test_sets/ocpp16/*.py \
  ocpp_tests/test_sets/ocpp201/*.py \
  --everest-prefix "$EXT_MOUNT/dist"
retVal=$?

if [ $retVal -ne 0 ]; then
    echo "OCPP tests failed with return code $retVal"
    exit $retVal
fi
