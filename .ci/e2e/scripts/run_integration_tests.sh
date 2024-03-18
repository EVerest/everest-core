#!/bin/sh

cd tests
pytest --everest-prefix ../dist core_tests/*.py framework_tests/*.py
