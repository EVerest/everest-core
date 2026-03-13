#!/bin/sh

rsync -a "$EXT_MOUNT/source/tests" ./
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Failed to copy tests"
    exit $retVal
fi

python3 -m pip install --break-system-packages \
    "$EXT_MOUNT"/wheels/everestpy-*.whl \
    "$EXT_MOUNT"/wheels/everest_testing-*.whl \
    "$EXT_MOUNT"/wheels/iso15118-*.whl \
    pytest-html
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Failed to pip-install"
    exit $retVal
fi

python3 -m pip install --break-system-packages -r tests/ocpp_tests/requirements.txt
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Failed to pip-install test requirements"
    exit $retVal
fi

backup_cwd=$(pwd)
cd tests/ocpp_tests/test_sets/everest-aux/

./install_certs.sh "$EXT_MOUNT/dist"
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Failed to install certs"
    exit $retVal
fi

./install_configs.sh "$EXT_MOUNT/dist"
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Failed to install configs"
    exit $retVal
fi

cd "$backup_cwd"
