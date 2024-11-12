#!/bin/sh

cd tests

pip install --break-system-packages -r ocpp_tests/requirements.txt
cmake --build "$EXT_MOUNT/build" --target iso15118_pip_install_dist

pytest \
  -rA \
  --junitxml="$EXT_MOUNT/result.xml" \
  --html="$EXT_MOUNT/report.html" \
  --self-contained-html \
  ocpp_tests/test_sets/*.py \
  --everest-prefix "$EXT_MOUNT/dist"
retVal=$?

if [ $retVal -ne 0 ]; then
    echo "OCPP tests failed with return code $retVal"
    exit $retVal
fi
