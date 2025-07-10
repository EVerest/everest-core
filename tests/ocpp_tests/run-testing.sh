#!/bin/bash

source ./run-testing-header.sh

echo "Running $PARALLEL_TESTS tests in parallel"

"$PYTHON_INTERPRETER" -m pytest -n "$PARALLEL_TESTS" --dist=loadgroup \
  -rA \
  --junitxml=result.xml \
  --html=report.html \
  --self-contained-html \
  --max-worker-restart=0 \
  --timeout=300 \
  test_sets/*/*.py \
  --everest-prefix "$EVEREST_CORE_DIR/build/dist"
