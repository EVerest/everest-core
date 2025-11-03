# everest-core

This is the main part of EVerest containing the actual charge controller logic included in a large set of modules

All documentation and the issue tracking can be found in our main repository here: https://github.com/EVerest/everest

## Development tools:
 [Everest Dependency Manager](https://github.com/EVerest/everest-dev-environment/blob/main/dependency_manager/README.md)

### Prerequisites:

#### Ubuntu
```bash
sudo apt update
sudo apt install -y git rsync wget cmake doxygen graphwiz build-essential clang-tidy cppcheck maven openjdk-11-jdk npm docker docker-compose libboost-aal-dev jstyleson jsonschema 
```
TODO: nodejs install with correct path to node-api.h to match CMAKEFILE.txt search paths.

#### OpenSuse
```bash
zypper update && zypper install -y sudo shadow
zypper install -y --type pattern devel_basis
zypper install -y git rsync wget cmake doxygen graphviz clang-tools cppcheck boost-devel libboost_filesystem-devel libboost_log-devel libboost_program_options-devel libboost_system-devel libboost_thread-devel maven java-11-openjdk java-11-openjdk-devel nodejs nodejs-devel npm python3-pip gcc-c++
```

### Build & Install:

[everest-cpp - Init Script](https://github.com/EVerest/everest-utils/tree/main/everest-cpp)

### Simulation

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

#### HIL
```bash
cd ~/checkout/everest-workspace/everest-core/build
.././run_hil.sh
#WIP
```
