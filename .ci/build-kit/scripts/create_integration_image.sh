#!/bin/sh

rsync -a "$EXT_MOUNT/source/tests" ./
retVal=$?

if [ $retVal -ne 0 ]; then
    echo "Failed to copy tests"
    exit $retVal
fi

pip install --break-system-packages \
    $EXT_MOUNT/wheels/everestpy-*.whl \
    $EXT_MOUNT/wheels/everest_testing-*.whl \
    pytest-html
retVal=$?

if [ $retVal -ne 0 ]; then
    echo "Failed to pip-install"
    exit $retVal
fi
