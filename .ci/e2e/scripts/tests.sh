#!/usr/bin/env sh
set -x

ninja -j$(nproc) -C build install

# install everestpy via cmake target from everest-framework
ninja -C build everestpy_pip_install_dist

rsync -a "tests" ./

rm -rf build

cd tests
pytest --everest-prefix ../dist core_tests/*.py framework_tests/*.py
