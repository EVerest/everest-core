#!/bin/sh

set -e

rsync -a "$EXT_MOUNT/source/tests" ./

pip install $EXT_MOUNT/wheels/everestpy-*.whl
pip install $EXT_MOUNT/wheels/everest_testing-*.whl
pip install pytest-html
