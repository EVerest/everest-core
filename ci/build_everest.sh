#!/bin/bash

export PATH=$PATH:/home/docker/.local/bin

echo "##############   build everest dependency manager   #############"

sudo mkdir -p /checkout/everest-workspace/
sudo chown -R docker /checkout
cd /checkout/everest-workspace/
git clone https://github.com/EVerest/everest-dev-environment.git
cd /checkout/everest-workspace/everest-dev-environment/dependency_manager
python3 -m pip install .

echo "##############   build ev-dev-tools   #############"

cd /checkout/everest-workspace/
git clone https://github.com/EVerest/everest-utils.git 
cd /checkout/everest-workspace/everest-utils/ev-dev-tools
python3 -m pip install .

echo "##############   checkout everest-testing   #############"

cd  /checkout/everest-workspace/everest-utils/everest-testing
python3 -m pip install .

echo "##############   install additional dependencies   #############"

cd /checkout/everest-workspace/
git clone https://github.com/EVerest/ext-switchev-iso15118.git
cd /checkout/everest-workspace/ext-switchev-iso15118
python3 -m pip install -r requirements.txt

echo "##############   build everest-core   #############"

sudo chown -R docker /cpm_cache

cd /checkout/everest-workspace/
git clone https://github.com/EVerest/everest-core.git
cd /checkout/everest-workspace/everest-core
git checkout $GIT_BRANCH
mkdir -p /checkout/everest-workspace/everest-core/build
cd /checkout/everest-workspace/everest-core/build

cmake -j$(nproc) ..
make -j$(nproc) install
