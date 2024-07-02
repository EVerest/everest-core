#!/usr/bin/env bash
set -eu -o pipefail

EDM_LINK=git+https://github.com/Everest/everest-dev-environment.git#subdirectory=dependency_manager
mkdir -p venv > /dev/null
TARGET_PATH=$(realpath venv)
pip3 install --upgrade -t $TARGET_PATH $EDM_LINK > /dev/null
export PYTHONPATH=$TARGET_PATH
export PATH=$TARGET_PATH/bin:$PATH

edm "$@"