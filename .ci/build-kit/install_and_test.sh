#!/bin/sh

set -e

cmake \
    -B build \
    -G Ninja \
    -S "$EXT_MOUNT/source" \
    -DBUILD_TESTING=ON \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX="$WORKSPACE_PATH/dist"

ninja -j$(nproc) -C build install

ninja -j$(nproc) -C build test

cp build/Testing/Temporary/LastTest.log /ext/ctest-report
