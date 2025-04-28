#!/bin/sh

cd tests

PARALLEL_TESTS=$(nproc)

echo "Running $PARALLEL_TESTS ocpp tests in parallel"

python3 -m pytest \
  -rA \
  -d --tx "$PARALLEL_TESTS"*popen//python=python3 \
  --max-worker-restart=0 \
  --timeout=300 \
  --junitxml="$EXT_MOUNT/ocpp-tests-result.xml" \
  --html="$EXT_MOUNT/ocpp-tests-report.html" \
  --self-contained-html \
  ocpp_tests/test_sets/ocpp16/*.py \
  ocpp_tests/test_sets/ocpp201/*.py \
  --everest-prefix "$EXT_MOUNT/dist"
retVal=$?

if [ $retVal -ne 0 ]; then
    echo "OCPP tests failed with return code $retVal"
    exit $retVal
fi
