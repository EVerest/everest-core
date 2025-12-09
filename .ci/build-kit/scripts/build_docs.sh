#!/bin/sh

EVEREST_DOCS_VERSION_NAME=${EVEREST_DOCS_VERSION_NAME:-"nightly"}

cmake \
    -B "$EXT_MOUNT/build" \
    -S "$EXT_MOUNT/source" \
    -G Ninja \
    -D EVC_ENABLE_CCACHE=ON \
    -D EVEREST_ENABLE_COMPILE_WARNINGS=ON \
    -D EVEREST_BUILD_DOCS=ON \
    -D TRAILBOOK_everest_DOWNLOAD_ALL_VERSIONS=ON \
    -D TRAILBOOK_everest_IS_RELEASE=OFF \
    -D EVEREST_DOCS_VERSION_NAME="$EVEREST_DOCS_VERSION_NAME"
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Configuring failed with return code $retVal"
    exit $retVal
fi

ninja -C "$EXT_MOUNT/build" trailbook_everest
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Compiling failed with return code $retVal"
    exit $retVal
fi
