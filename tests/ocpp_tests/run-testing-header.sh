#!/bin/bash
PYTHON_INTERPRETER="${PYTHON_INTERPRETER:-python3}"
echo "Using python: $PYTHON_INTERPRETER"
OCPP_TESTING_DIR=$(cd $(dirname "${BASH_SOURCE:-$0}") && pwd)
EVEREST_CORE_DIR=$(dirname $(dirname "$OCPP_TESTING_DIR"))

if [ ! -d "$EVEREST_CORE_DIR" ]; then
    echo "everest-core not found at: $EVEREST_CORE_DIR"
    exit 0
fi

echo "Using everest-core: $EVEREST_CORE_DIR"

PARALLEL_TESTS=$(nproc)
if [ $# -eq 1 ] ; then
    PARALLEL_TESTS="$1"
fi

cd "$OCPP_TESTING_DIR"
$(cd test_sets/everest-aux/ && ./install_certs.sh "$EVEREST_CORE_DIR/build/dist" && ./install_configs.sh "$EVEREST_CORE_DIR/build/dist")
