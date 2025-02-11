#!/bin/bash

source ./run-testing-header.sh

echo "Running $PARALLEL_TESTS tests in parallel"

# run all tests in parallel
"$PYTHON_INTERPRETER" -m pytest -d --tx "$PARALLEL_TESTS"*popen//python="$PYTHON_INTERPRETER" -rA --junitxml=result.xml --html=report.html --self-contained-html --max-worker-restart=0 test_sets/ocpp201/*.py --everest-prefix "$EVEREST_CORE_DIR/build/dist"
