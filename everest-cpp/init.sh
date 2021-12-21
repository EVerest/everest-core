#!/bin/bash
##
## SPDX-License-Identifier: Apache-2.0
## Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
##
echo "Installing required tools, initializing workspace, changing into everest-core/build"
git clone git@github.com:EVerest/everest-dev-environment.git &> /dev/null || true
(cd everest-dev-environment/dependency_manager && python3 -m pip install .)
edm --register-cmake-module
edm --config everest-dev-environment/everest-complete.yaml --workspace .
edm --update
(cd everest-utils/ev-dev-tools && python3 -m pip install .)
mkdir -p everest-core/build
cd everest-core/build
