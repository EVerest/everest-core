.. _exp_dev_tools_edm:

###
edm
###

edm stands for EVerest dependency manager. It helps you orchestrating the
dependencies between the different EVerest repositories.

Dependency Manager for EVerest
##############################

Install and Quick Start
***********************

To install the **edm** dependency manager for EVerest you have to perform the
following steps.

Please make sure you are running a sufficiently recent version of **Python3 (>=3.6)** and that you are able to install Python packages from source.
See the *python3* command below for upgrading the required packages. Refer to
the
`Python Installing Packages <https://packaging.python.org/tutorials/installing-packages/#requirements-for-installing-packages>`_
documentation for indepth guidance if any problems arise. You may want to create and activate a virtual environment
using `venv <https://docs.python.org/3/library/venv.html>`_

.. code-block:: bash

  python3 -m venv venv
  source venv/bin/activate

before executing the commands below.

.. code-block:: bash

  python3 -m pip install --upgrade pip setuptools wheel jstyleson jsonschema

Python packages needed to run edm
*********************************

The following Python3 packages are needed to run **edm**. If you install edm
using this guide they will be installed automatically.

+ Python >= 3.6
+ Jinja2 >= 3.0
+ PyYAML >= 5.4

Installing edm
**************

Now you can clone this repository and install **edm**:

.. code-block:: bash

  git clone https://github.com/EVerest/everest-core
  cd everest-core/applications/dev-environment/dependency_manager
  python3 -m pip install . --break-system-packages

  edm init --workspace ~/checkout/everest-workspace

The last command creates a workspace in the ``~/checkout/everest-workspace``
directory from the most recent release of EVerest. If you want the most recent
main you can use:

.. code-block:: bash

  edm init main --workspace ~/checkout/everest-workspace

The workspace will have the following structure containing all current
dependencies for EVerest:

.. code-block:: bash

	everest-workspace/
	├── everest-cmake
	├── everest-core
	├── everest-dev-environment
	├── everest-framework
	├── everest-sqlite
	├── everest-utils
	├── Josev
	├── libcbv2g
	├── libevse-security
	├── libfsm
	├── libiso15118
	├── liblog
	├── libnfc-nci
	├── libocpp
	├── libslac
	├── libtimer
	└── workspace-config.yaml

The ``workspace-config.yaml`` contains a copy of the config that was used to create
this workspace.

Enabling CPM_SOURCE_CACHE and setting PATH
******************************************

The EVerest dependency manager uses
`CPM <https://github.com/cpm-cmake/CPM.cmake>`_
for its CMake integration. This means you *can* and **should** set the
``CPM_SOURCE_CACHE`` environment variable. This makes sure that dependencies
that you do not manage in the workspace are not re-downloaded multiple times.
For detailed information and other useful environment variables please
refer to the `CPM Documentation <https://github.com/cpm-cmake/CPM.cmake/blob/master/README.md#CPM_SOURCE_CACHE>`_.

Also set the PATH variable:

.. code-block:: bash

	export CPM_SOURCE_CACHE=$HOME/.cache/CPM
	export PATH=$PATH:/home/$(whoami)/.local/bin

Building EVerest
****************

Make sure you have installed :doc:`ev-cli <ev-cli>` first.
You can now use the following commands to build the repository everest-core:

.. code-block:: bash

  cd ~/checkout/everest-workspace/everest-core
  mkdir build
  cd build
  cmake ..
  make -j$(nproc) install

.. _cmake_integration_setup:

Setting up and updating a workspace
###################################

For letting **edm** do the work of setting up an initial EVerest workspace,
do this:

.. code-block:: bash

  edm init --workspace ~/checkout/everest-workspace

If you are currently in the everest-workspace directory the following command
has the same effect:

.. code-block:: bash

  edm init

For using a dedicated release version, you can do this:

.. code-block:: bash

  edm init 2023.7.0

In this example, version 2023.7.0 is pulled from the server. This will only work
if your previous code is not in a "dirty" state.

Using the edm CMake module and dependencies.yaml
################################################

To use edm from CMake you have to add the following line to the top-level
CMakeLists.txt file in the respective source repository:

.. code-block:: bash

   find_package(EDM REQUIRED)

To define dependencies you can now add a dependencies.yaml file to your source
repository. It should look like this:

.. code-block:: bash

    ---
    sigslot:
      git: https://github.com/palacaze/sigslot
      git_tag: v1.2.3
      cmake_condition: "EVEREST_DEPENDENCY_ENABLED_SIGSLOT"
      options:
        - "SIGSLOT_COMPILE_EXAMPLES OFF"
        - "SIGSLOT_COMPILE_TESTS OFF"
    pugixml:
      git: https://github.com/zeux/pugixml
      git_tag: v1.15
      cmake_condition: "EVEREST_DEPENDENCY_ENABLED_PUGIXML"

If you want to conditionally include some dependencies, e.g. for testing, you can
do this in the following way:

.. code-block:: bash

	catch2:
	  git: https://github.com/catchorg/Catch2.git
	  git_tag: v3.4.0
	  cmake_condition: "BUILD_TESTING"

Here *cmake_condition* can be any string that CMake can use in an if() block.
Please be aware that any variables you use here must be defined before a call to
*evc_setup_edm()* is made in your ``CMakeLists.txt``

Additionally you can set the ``EVEREST_MODIFY_DEPENDENCIES`` environment variable
to a file containing modifications to the projects ``dependencies.yaml`` files when
running cmake:

.. code-block:: bash

  EVEREST_MODIFY_DEPENDENCIES=../dependencies_modified.yaml cmake -S . -B build

The ``dependencies_modified.yaml`` file can contain something along these lines:

.. code-block:: bash

	nlohmann_json:
	  git: null       # this makes edm look for nlohmann_json via find_package
	libfmt:
	  rename: fmt     # if find_package needs a different dependency name you can rename it
	  git: null
	catch2:
	  git_tag: v1.2.3 # select a different git tag for a build

Create a workspace config from an existing directory tree
#########################################################

Suppose you already have a directory tree that you want to save into a config
file. You can do this with the following command:

.. code-block:: bash

  edm --create-config custom-config.yaml

This is a short form of:

.. code-block:: bash

  edm --create-config custom-config.yaml --include-remotes https://github.com/EVerest/*

and only includes repositories from the EVerest namespace. You can add as many
remotes to this list as you want.

For example, if you only want to include certain repositories you can use the
following command.

.. code-block:: bash

  edm --create-config custom-config.yaml --include-remotes https://github.com/EVerest/everest* https://github.com/EVerest/ext-switchev-iso15118.git

If you want to include all repositories, including external dependencies, in
the config you can use the following command:

.. code-block:: bash

  edm --create-config custom-config.yaml --external-in-config

.. _git_information_at_a_glance:

Git information at a glance
###########################

You can get a list of all git repositories in the current directory and their
state using the following command:

.. code-block:: bash

  edm --git-info --git-fetch

If you want to know the state of all repositories in a workspace you can use
the following command:

.. code-block:: bash

  edm --workspace ~/checkout/everest-workspace --git-info --git-fetch

This creates output that is similar to the following example:

.. code-block:: bash

  [edm]: Git info for "~/checkout/everest-workspace":
  [edm]: Using git-fetch to update remote information. This might take a few seconds.
  [edm]: "everest-dev-environment" @ branch: main [remote: origin/main] [behind 6] [clean]
  [edm]: "everest-framework" @ branch: main [remote: origin/main] [dirty]
  [edm]: "everest-deploy-devkit" @ branch: main [remote: origin/main] [clean]
  [edm]: "libtimer" @ branch: main [remote: origin/main] [dirty]
  [edm]: 2/4 repositories are dirty.

Further information can be seen as shell output by calling edm with parameter
**-h** or **--help**. 

----

**Authors**: Kai-Uwe Hermann, Stefan Wahren, Andreas Heinrich, Manuel Ziegler