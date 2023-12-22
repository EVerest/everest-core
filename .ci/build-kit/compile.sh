#!/usr/bin/env sh
set -ex

cmake \
    -B build \
    -G Ninja \
    -DCMAKE_INSTALL_PREFIX="$WORKSPACE_PATH/dist" \
    -DBUILD_TESTING=ON \
    "${CMAKE_FLAGS_EXTRA}"

ninja -j$(nproc) -C build
