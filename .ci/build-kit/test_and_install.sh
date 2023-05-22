#!/bin/sh

# ninja -j$(nproc) -C build tests
ninja -j$(nproc) -C build install

# install everestpy via cmake target from everest-framework
ninja -C build everestpy_pip_install_dist

rsync -a "$EXT_MOUNT/source/tests" ./

rm -rf build
