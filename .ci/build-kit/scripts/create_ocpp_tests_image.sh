#!/bin/bash

rsync -a "$EXT_MOUNT/source/tests" ./
retVal=$?

if [ $retVal -ne 0 ]; then
    echo "Failed to copy tests"
    exit $retVal
fi

python3 -m venv "$EXT_MOUNT/venv" --system-site-packages
retVal=$?

if [ $retVal -ne 0 ]; then
    echo "Failed to create virtual environment"
    exit $retVal
fi

source "$EXT_MOUNT/venv/bin/activate"
retVal=$?

if [ $retVal -ne 0 ]; then
    echo "Failed to activate virtual environment"
    exit $retVal
fi

pip install --break-system-packages \
    "$EXT_MOUNT"/wheels/everestpy-*.whl \
    "$EXT_MOUNT"/wheels/everest_testing-*.whl \
    "$EXT_MOUNT"/wheels/iso15118-*.whl \
    pytest-html
retVal=$?

if [ $retVal -ne 0 ]; then
    echo "Failed to pip-install"
    exit $retVal
fi

pip install --break-system-packages -r tests/ocpp_tests/requirements.txt

$(cd ./tests/ocpp_tests/test_sets/everest-aux/ && ./install_certs.sh "$EXT_MOUNT/dist" && ./install_configs.sh "$EXT_MOUNT/dist")
