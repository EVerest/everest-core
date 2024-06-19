#!/bin/sh

set -e

cmake \
    -B build \
    -S "$EXT_MOUNT/source" \
    -G Ninja \
    -DCB_V2G_BUILD_TESTS=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

ninja -C build

trap "cp build/Testing/Temporary/LastTest.log /ext/ctest-report" EXIT

ninja -C build test
