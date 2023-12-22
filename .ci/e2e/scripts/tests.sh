#!/usr/bin/env sh
set -ex

ninja -j$(nproc) -C build install

# install everest testing by cmake target to make sure using the version defined in dependencies.yaml
ninja -C build install_everest_testing

cd tests
pytest --everest-prefix ../dist core_tests/*.py framework_tests/*.py
