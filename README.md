# everest-core

This is the main part of EVerest containing the actual charge controller logic included in a large set of modules

All documentation and the issue tracking can be found in our main repository here: https://github.com/EVerest/everest

### Prerequisites:

#### Ubuntu 20.04
```bash
sudo apt update
sudo apt install -y git rsync wget cmake doxygen graphviz build-essential clang-tidy cppcheck maven openjdk-11-jdk npm docker docker-compose libboost-all-dev jstyleson jsonschema nodejs libssl-dev libsqlite3-dev clang-format clang-format-12
```
In order to force the use of clang-format version 12 execute:
```bash
sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-12 100
```
Verify clang-format version:
```bash
clang-format --version
Ubuntu clang-format version 12.0.0-3ubuntu1~20.04.4
```


#### OpenSuse
```bash
zypper update && zypper install -y sudo shadow
zypper install -y --type pattern devel_basis
zypper install -y git rsync wget cmake doxygen graphviz clang-tools cppcheck boost-devel libboost_filesystem-devel libboost_log-devel libboost_program_options-devel libboost_system-devel libboost_thread-devel maven java-11-openjdk java-11-openjdk-devel nodejs nodejs-devel npm python3-pip gcc-c++ libopenssl-devel sqlite3-devel
```

### Build & Install:

It is required that you have uploaded your public [ssh key](https://www.atlassian.com/git/tutorials/git-ssh) to [github](https://github.com/settings/keys).

To install the [Everest Dependency Manager](https://github.com/EVerest/everest-dev-environment/blob/main/dependency_manager/README.md), follow these steps:

Install required python packages:
```bash
python3 -m pip install --upgrade pip setuptools wheel
```
Get EDM source files and change into the directory:
```bash
git clone git@github.com:EVerest/everest-dev-environment.git
cd everest-dev-environment/dependency_manager
```
Install EDM:
```bash
python3 -m pip install .
```
Setup Everest workspace: 
```bash
edm --register-cmake-module --config ../everest-complete.yaml --workspace ~/checkout/everest-workspace
```
Set environmental variable for [CPM](https://github.com/cpm-cmake/CPM.cmake/blob/master/README.md#CPM_SOURCE_CACHE) cache:
```bash
export CPM_SOURCE_CACHE=$HOME/.cache/CPM
```
Install [ev-cli](https://github.com/EVerest/everest-utils/tree/main/ev-dev-tools):

Change into the directory and install ev-cli:
```bash
cd ~/checkout/everest-workspace/everest-utils/ev-dev-tools
python3 -m pip install .
```

Now we can build everest!

Create build directory and change directory:
```bash
mkdir -p ~/checkout/everest-workspace/everest-core/build
cd ~/checkout/everest-workspace/everest-core/build
```
Build everest:
```bash
cmake ..
make install
```
Done!

<!--- WIP: [everest-cpp - Init Script](https://github.com/EVerest/everest-utils/tree/main/everest-cpp) -->

### Simulation

In order to test your build of Everest you can simulate the code on your local machine!

Docker container for local MQTT broker, node-red etc.
```bash
cd ~/checkout/everest-workspace/everest-utils/docker
sudo docker-compose up -d
```

#### SIL

```bash
cd ~/checkout/everest-workspace/everest-core/build
.././run_sil.sh
```

#### HIL (WIP!)
```bash
cd ~/checkout/everest-workspace/everest-core/build
.././run_hil.sh
```
