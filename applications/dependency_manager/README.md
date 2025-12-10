# Dependency Manager for EVerest

- [Dependency Manager for EVerest](#dependency-manager-for-everest)
  - [Install and Quick Start](#install-and-quick-start)
    - [Installing edm](#installing-edm)
    - [Enabling CPM_SOURCE_CACHE](#enabling-cpm_source_cache)
    - [Python packages needed to run edm](#python-packages-needed-to-run-edm)
  - [Setting up CMake integration](#setting-up-cmake-integration)
  - [Setting up a workspace](#setting-up-a-workspace)
  - [Updating a workspace](#updating-a-workspace)
  - [Using the EDM CMake module and dependencies.yaml](#using-the-edm-cmake-module-and-dependenciesyaml)
  - [Modifying dependencies](#modifying-dependencies)
  - [Create a workspace config from an existing directory tree](#create-a-workspace-config-from-an-existing-directory-tree)
  - [Git information at a glance](#git-information-at-a-glance)

## Install and Quick Start
To install the **edm** dependency manager for EVerest you have to perform the following steps.

Please make sure you are running a sufficiently recent version of **Python3 (>=3.6)** and that you are able to install Python packages from source. Usually you just have to ensure that you have **pip**, **setuptools** and **wheel** available. Refer to [the Python *Installing Packages* documentation](https://packaging.python.org/tutorials/installing-packages/#requirements-for-installing-packages) for indepth guidance if any problems arise.

```bash
python3 -m pip install --upgrade pip setuptools wheel
```

### Installing edm
Now you can clone *this repository* and install **edm**:

(make sure you have set up your [ssh key](https://www.atlassian.com/git/tutorials/git-ssh) in github first!)

```bash
git clone git@github.com:EVerest/everest-dev-environment.git
cd everest-dev-environment/dependency_manager
python3 -m pip install .
edm --config ../everest-complete.yaml --workspace ~/checkout/everest-workspace
```

The last command registers the [**EDM** CMake module](#setting-up-cmake-integration) and creates a workspace in the *~/checkout/everest-workspace* directory from [a config that is shipped with this repository](../everest-complete.yaml).
The workspace will have the following structure containing all current dependencies for EVerest:
```bash
everest-workspace/
├── everest-core
├── everest-deploy-devkit
├── everest-dev-environment
├── everest-framework
├── everest-utils
├── liblog
├── libmodbus
├── libocpp
├── libsunspec
├── libtimer
├── open-plc-utils
├── RISE-V2G
└── workspace-config.yaml
```
The *workspace-config.yaml* contains a copy of the config that was used to create this workspace.

### Enabling CPM_SOURCE_CACHE
The **edm** dependency manager uses [CPM](https://github.com/cpm-cmake/CPM.cmake) for its CMake integration.
This means you *can* and **should** set the *CPM_SOURCE_CACHE* environment variable. This makes sure that dependencies that you do not manage in the workspace are not re-downloaded multiple times. For detailed information and other useful environment variables please refer to the [CPM Documentation](https://github.com/cpm-cmake/CPM.cmake/blob/master/README.md#CPM_SOURCE_CACHE).
```bash
export CPM_SOURCE_CACHE=$HOME/.cache/CPM
```

### Python packages needed to run edm
The following Python3 packages are needed to run the **edm** dependency manager.
If you installed **edm** using the guide above they were already installed automatically.

- Python >= 3.6
- Jinja2 >= 3.0
- PyYAML >= 5.4

## Setting up and updating a workspace
For letting **edm** do the work of setting up an initial EVerest workspace,
do this:

```bash
edm init --workspace ~/checkout/everest-workspace
```
If you are currently in the *everest-workspace* directory the following command has the same effect:

```bash
edm init
```

For using a dedicated release version, you can do this:

```bash
edm init 2023.7.0
```

In this example, version 2023.7.0 is pulled from the server. This will only work if
you previous code is not in a "dirty" state. 

## Using the EDM CMake module and dependencies.yaml
To use **edm** from CMake you have to add the following line to the top-level *CMakeLists.txt* file in the respective source repository:
```cmake
find_package(EDM REQUIRED)
```
The **EDM** CMake module will be discovered automatically if you [registered the CMake module in the way it described in the *Setting up CMake integration* section of this readme](#setting-up-cmake-integration).

To define dependencies you can now add a **dependencies.yaml** file to your source repository. It should look like this:
```yaml
---
liblog:
  git: git@github.com:EVerest/liblog.git
  git_tag: main
  options: ["BUILD_EXAMPLES OFF"]
libtimer:
  git: git@github.com:EVerest/libtimer.git
  git_tag: main
  options: ["BUILD_EXAMPLES OFF"]

```

If you want to conditionally include some dependencies, eg. for testing, you can do this in the following way:
```yaml
catch2:
  git: https://github.com/catchorg/Catch2.git
  git_tag: v3.4.0
  cmake_condition: "BUILD_TESTING"

```
Here *cmake_condition* can be any string that CMake can use in an if() block. Please be aware that any variables you use here must be defined before a call to *evc_setup_edm()* is made in your CMakeLists.txt

## Modifying dependencies

To change dependency git URLs you can set the *EVEREST_MODIFY_DEPENDENCIES_URLS* environment variable to a string containing prefixes and replacements delimited by whitespace characters.
For example:
```bash
EVEREST_MODIFY_DEPENDENCIES_URLS="prefix=https://github.com/EVerest/ replace=git@github.com:EVerest/"
```
This would change all dependency git URLs that start with *https://github.com/EVerest/* to *git@github.com:EVerest/*.

Multiple prefix and replace pairs can be chained together delimited by whitespace characters.
For example:
```bash
EVEREST_MODIFY_DEPENDENCIES_URLS="prefix=https://github.com/EVerest/ replace=git@github.com:EVerest/ prefix=https://github.com/EVerest/everest-framework.git replace=https://github.com/EVerest/everest-framework.git"
```
This would change all dependency git URLs that start with *https://github.com/EVerest/* to *git@github.com:EVerest/* as well as keeping the dependency https URL of *https://github.com/EVerest/everest-framework.git* as *https://github.com/EVerest/everest-framework.git*.

Additionally you can set the *EVEREST_MODIFY_DEPENDENCIES* environment variable to a file containing modifications to the projects dependencies.yaml files when running cmake:

```bash
EVEREST_MODIFY_DEPENDENCIES=../dependencies_modified.yaml cmake -S . -B build
```

The *dependencies_modified.yaml* file can contain something along these lines:

```yaml
nlohmann_json:
  git: null # this makes edm look for nlohmann_json via find_package
libfmt:
  rename: fmt # if find_package needs a different dependency name you can rename it
  git: null
catch2:
  git_tag: v1.2.3 # if you want to select a different git tag for a build this is also possible
```

## Create a workspace config from an existing directory tree
Suppose you already have a directory tree that you want to save into a config file.
You can do this with the following command:
```bash
edm --create-config custom-config.yaml
```

This is a short form of
```bash
edm --create-config custom-config.yaml --include-remotes git@github.com:EVerest/*
```
and only includes repositories from the *EVerest* namespace. You can add as many remotes to this list as you want.

For example if you only want to include certain repositories you can use the following command.
```bash
edm --create-config custom-config.yaml --include-remotes git@github.com:EVerest/everest* git@github.com:EVerest/liblog.git
```

If you want to include all repositories, including external dependencies, in the config you can use the following command.
```bash
edm --create-config custom-config.yaml --external-in-config
```

## Git information at a glance
You can get a list of all git repositories in the current directory and their state using the following command.
```bash
edm --git-info --git-fetch
```
If you want to know the state of all repositories in a workspace you can use the following command.
```bash
edm --workspace ~/checkout/everest-workspace --git-info --git-fetch
```

This creates output that is similar to the following example.
```bash
[edm]: Git info for "~/checkout/everest-workspace":
[edm]: Using git-fetch to update remote information. This might take a few seconds.
[edm]: "everest-dev-environment" @ branch: main [remote: origin/main] [behind 6] [clean]
[edm]: "everest-framework" @ branch: main [remote: origin/main] [dirty]
[edm]: "everest-deploy-devkit" @ branch: main [remote: origin/main] [clean]
[edm]: "libtimer" @ branch: main [remote: origin/main] [dirty]
[edm]: 2/4 repositories are dirty.
```
