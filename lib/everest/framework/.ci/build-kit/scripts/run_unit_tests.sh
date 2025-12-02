#!/bin/sh

CTEST_OUTPUT_ON_FAILURE=1 ninja -C "$EXT_MOUNT/build" test
retVal=$?

# Copy the LastTest.log file to the mounted directory in any case
cp "$EXT_MOUNT/build/Testing/Temporary/LastTest.log" "$EXT_MOUNT/ctest-report"

if [ $retVal -ne 0 ]; then
    echo "Unit tests failed with return code $retVal"
    exit $retVal
fi
