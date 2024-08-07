#!/bin/sh

set -e

rsync -a "$EXT_MOUNT/source/tests" ./

pip install --break-system-packages \
    $EXT_MOUNT/wheels/everestpy-*.whl
pip install --break-system-packages \
    $EXT_MOUNT/wheels/everest_testing-*.whl
pip install --break-system-packages \
    pytest-html
