#!/bin/sh

cmake \
    -B "$EXT_MOUNT/build" \
    -S "$EXT_MOUNT/source" \
    -G Ninja \
    -DEVC_ENABLE_CCACHE=1 \
    -DISO15118_2_GENERATE_AND_INSTALL_CERTIFICATES=OFF \
    -DCMAKE_INSTALL_PREFIX="$EXT_MOUNT/dist" \
    -DWHEEL_INSTALL_PREFIX="$EXT_MOUNT/wheels" \
    -DBUILD_TESTING=ON \
    -DEVEREST_ENABLE_COMPILE_WARNINGS=ON
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Configuring failed with return code $retVal"
    exit $retVal
fi

ninja -C "$EXT_MOUNT/build"
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Compiling failed with return code $retVal"
    exit $retVal
fi
