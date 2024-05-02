#!/bin/sh

set -e

cmake \
    -B build \
    -S "$EXT_MOUNT/source" \
    -G Ninja \
    -DISO15118_BUILD_TESTS=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

ninja -j$(nproc) -C build

trap "cp build/Testing/Temporary/LastTest.log /ext/ctest-report" EXIT

ninja -j$(nproc) -C build test
