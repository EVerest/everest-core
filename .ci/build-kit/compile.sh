#!/bin/sh

cmake \
    -B build \
    -S "$EXT_MOUNT/source" \
    -G Ninja \
    -DEVC_ENABLE_CCACHE=1 \
    -DCMAKE_INSTALL_PREFIX="$WORKSPACE_PATH/dist"

ninja -j$(nproc) -C build
