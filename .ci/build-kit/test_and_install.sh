#!/bin/sh

# ninja -j$(nproc) -C build tests
ninja -j$(nproc) -C build install

rsync -a "$EXT_MOUNT/source/tests" ./

rm -rf build
