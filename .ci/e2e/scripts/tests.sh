#!/usr/bin/env sh
set -ex

# install everest testing by cmake target to make sure using the version defined in dependencies.yaml
ninja -C build install_everest_testing

cd tests
pytest --everest-prefix ../build/dist core_tests/*.py framework_tests/*.py
