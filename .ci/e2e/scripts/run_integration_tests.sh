#!/bin/sh

set -e

cd tests
pytest \
  -rA \
  --junitxml="$EXT_MOUNT/result.xml" \
  --html="$EXT_MOUNT/report.html" \
  --self-contained-html \
  core_tests/*.py \
  framework_tests/*.py \
  --everest-prefix "$EXT_MOUNT/dist"
