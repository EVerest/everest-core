#!/bin/sh

cmake \
    -B "$EXT_MOUNT/build" \
    -S "$EXT_MOUNT/source" \
    -G Ninja \
    -DEVC_ENABLE_CCACHE=1 \
    -DISO15118_2_GENERATE_AND_INSTALL_CERTIFICATES=OFF \
    -DCMAKE_INSTALL_PREFIX="$EXT_MOUNT/dist" \
    -DBUILD_TESTING=ON

ninja -j$(nproc) -C "$EXT_MOUNT/build"
ninja -j$(nproc) -C "$EXT_MOUNT/build" install
