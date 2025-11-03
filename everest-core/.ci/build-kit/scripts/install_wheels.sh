#!/bin/sh

ninja -C "$EXT_MOUNT/build" \
    everestpy_install_wheel \
    everest-testing_install_wheel \
    iso15118_install_wheel
retVal=$?

if [ $retVal -ne 0 ]; then
    echo "Wheel Installation failed with return code $retVal"
    exit $retVal
fi
