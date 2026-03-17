#!/bin/bash

TESTING_DIR=$(cd $(dirname "${BASH_SOURCE:-$0}") && pwd)
EVEREST_DIR="$(dirname "$TESTING_DIR")/"

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
# run all tests in parallel
python3 -m pytest -d --tx "$PARALLEL_TESTS"*popen//python=python3 -rA --junitxml=result.xml --html=report.html --self-contained-html --max-worker-restart=0 --everest-prefix "$EVEREST_DIR/build/dist" core_tests/*.py framework_tests/*.py async_api_tests/*.py
