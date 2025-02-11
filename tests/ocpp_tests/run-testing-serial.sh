#!/bin/bash

source ./run-testing-header.sh

echo "Running tests serially"

# run all tests serially
"$PYTHON_INTERPRETER" -m pytest --junitxml=result.xml --html=report.html test_sets/ocpp16/*.py test_sets/ocpp201/*.py --everest-prefix "$EVEREST_CORE_DIR/build/dist"
