#!/bin/bash
##
## SPDX-License-Identifier: Apache-2.0
## Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
##
echo "Installing required tools, initializing workspace, changing into everest-core/build"
GIT_PROTOCOL="git"
if [ -d "everest-dev-environment" ] ; then
    echo "Directory 'everest-dev-environment' already exists, trying to update its contents..."
    (cd everest-dev-environment && git pull)
else
    ssh -T git@github.com
    SSH_TEST_RETVAL=$?
    if [ $SSH_TEST_RETVAL -ne 1 ] ; then
        echo "Could not clone everest-dev-environment with git protocol."
        echo "Did you add your SSH key on GitHub and made it available to ssh-agent?"
        echo "Trying to continue with https protocol."
        GIT_PROTOCOL="https"
    fi
    if [ "${GIT_PROTOCOL}" = "https" ] ; then
        git clone https://github.com/EVerest/everest-dev-environment.git || { echo "Could not clone everest-dev-environment."; return; }
    else
        git clone git@github.com:EVerest/everest-dev-environment.git
    fi
fi
(cd everest-dev-environment/dependency_manager && python3 -m pip install .)
edm init main
(cd everest-utils/ev-dev-tools && python3 -m pip install .)
mkdir -p everest-core/build
cd everest-core/build
echo ""
echo "You can now build everest-core with the following commands:"
echo ""
echo "cmake .. && make install -j$(nproc)"
