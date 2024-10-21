#!/bin/sh

cd tests
pytest \
  -rA \
  --junitxml="$EXT_MOUNT/result.xml" \
  --html="$EXT_MOUNT/report.html" \
  --self-contained-html \
  core_tests/*.py \
  framework_tests/*.py \
  --everest-prefix "$EXT_MOUNT/dist"
retVal=$?

if [ $retVal -ne 0 ]; then
    echo "Integration tests failed with return code $retVal"
    exit $retVal
fi
