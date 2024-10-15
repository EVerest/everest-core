#!/bin/sh

set -e


EVEREST_CMAKE_PATH=/usr/lib/cmake/everest-cmake
EVEREST_CMAKE_VERSION=4d81814
rm -rf ${EVEREST_CMAKE_PATH}
git clone https://github.com/EVerest/everest-cmake.git ${EVEREST_CMAKE_PATH} \
    && cd ${EVEREST_CMAKE_PATH} \
    && git checkout ${EVEREST_CMAKE_VERSION}

cmake \
    -B "$EXT_MOUNT/build" \
    -S "$EXT_MOUNT/source" \
    -G Ninja \
    -DEVC_ENABLE_CCACHE=1 \
    -DISO15118_2_GENERATE_AND_INSTALL_CERTIFICATES=OFF \
    -DCMAKE_INSTALL_PREFIX="$EXT_MOUNT/dist" \
    -DWHEEL_INSTALL_PREFIX="$EXT_MOUNT/dist-wheels" \
    -DBUILD_TESTING=ON \
    -DEVEREST_ENABLE_COMPILE_WARNINGS=ON

ninja -j$(nproc) -C "$EXT_MOUNT/build"
