#!/bin/bash
PYTHON_INTERPRETER="${PYTHON_INTERPRETER:-python3}"
echo "Using python: $PYTHON_INTERPRETER"
OCPP_TESTING_DIR=$(cd $(dirname "${BASH_SOURCE:-$0}") && pwd)
EVEREST_DIR=$(dirname $(dirname "$OCPP_TESTING_DIR"))

if [ ! -d "$EVEREST_DIR" ]; then
    echo "EVerest not found at: $EVEREST_DIR"
    exit 0
fi

echo "Using EVerest: $EVEREST_DIR"

PARALLEL_TESTS=$(nproc)
if [ $# -eq 1 ] ; then
    PARALLEL_TESTS="$1"
fi

echo "Running $PARALLEL_TESTS tests in parallel"

cd "$OCPP_TESTING_DIR"
$(cd test_sets/everest-aux/ && ./install_certs.sh "$EVEREST_DIR/build/dist" && ./install_configs.sh "$EVEREST_DIR/build/dist")
