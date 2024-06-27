#!/bin/sh

set -e

ninja -C "$EXT_MOUNT/build" install
ninja -C "$EXT_MOUNT/build" everestpy_install_wheel
ninja -C "$EXT_MOUNT/build" everest-testing_install_wheel
ninja -C "$EXT_MOUNT/build" iso15118_install_wheel
