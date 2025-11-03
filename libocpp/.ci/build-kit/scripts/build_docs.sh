#!/bin/sh

cmake \
    -B "$EXT_MOUNT/build" \
    -S "$EXT_MOUNT/source" \
    -G Ninja \
    -DBUILD_TESTING=ON \
    -DLIBOCPP16_BUILD_EXAMPLES=ON \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX="$EXT_MOUNT/dist" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Configuring failed with return code $retVal"
    exit $retVal
fi

ninja -C "$EXT_MOUNT/build" doxygen-ocpp
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Building docs failed with return code $retVal"
    exit $retVal
fi

cp -r "$EXT_MOUNT/build/dist/docs/html" "$EXT_MOUNT/doxygen-docs"
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Copying docs failed with return code $retVal"
    exit $retVal
fi
