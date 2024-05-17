#!/bin/sh

set -e

source build/venv/bin/activate
cd tests
pytest --everest-prefix ../dist core_tests/*.py framework_tests/*.py
