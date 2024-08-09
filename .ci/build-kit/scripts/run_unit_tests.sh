#!/bin/sh

set -e

trap "cp $EXT_MOUNT/build/Testing/Temporary/LastTest.log $EXT_MOUNT/ctest-report" EXIT

ninja -C "$EXT_MOUNT/build" test
