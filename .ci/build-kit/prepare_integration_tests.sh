#!/bin/sh

# ninja -j$(nproc) -C build tests
ninja -j$(nproc) -C build install
ninja -C build everestpy_install_wheel
ninja -C build everest-testing_install_wheel

# install everest testing by cmake target to make sure using the version defined in dependencies.yaml
ninja -C build install_everest_testing

rsync -a "$EXT_MOUNT/source/tests" ./

# rm -rf build
