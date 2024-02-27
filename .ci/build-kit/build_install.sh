#!/usr/bin/env sh
set -ex

cmake \
    -B build/ \
    -G Ninja \
    -DCMAKE_INSTALL_PREFIX="build/dist/" \
    -DBUILD_TESTING=ON \
    "${CMAKE_FLAGS_EXTRA}"

ninja -j$(nproc) -C build install
