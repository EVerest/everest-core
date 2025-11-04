.. _howto_prepare_development_environment:

####################################
Prepare Your Development Environment
####################################

This guide will help you to set up a development environment for EVerest on your local machine.

For a native build, EVerest requires a Linux based system.

To build on a Windows or Mac system, you can use WSL2 (Windows) or
Docker / Podman (Mac).

For Mac, see the :doc:`How-to for MAC setup </how-to-guides/how-to-mac-setup>`.

System Requirements and Dependencies
====================================

This section lists all dependencies and supported environments required to set up 
your development environment and build EVerest.

General Requirements
--------------------

* Python (greater than 3.6)
* Jinja2
* PyYAML
* Compiler:

  * GCC 9 (lower versions could work with some tweaking but are not
    recommended)
  * Clang (starting with version 12) has been used with EVerest but is not
    officially supported.
  * See also :ref:`TODO the FAQ entry about GNU compilers <faq_gnu_compilers>`!

Make sure you have the Python packages pip, setuptools and wheel available,
which are needed for the EVerest dependency manager:

.. code-block:: bash

  python3 -m pip install --upgrade pip setuptools wheel

.. note::

  Depending on the programming language that you will use for implementing
  custom modules, you might need additional tools for compilation and similar
  use cases. If you stumble over some trouble that is caused by EVerest -
  especially when using other languages than C++ -, drop us a line or consider
  creating a GitHub issue.

A complete list of libraries to be installed is given by the following best
practices which setup a development environment on a number of operating
systems.

Tested Linux Distributions
--------------------------

**Ubuntu**:

.. warning::

  Ubuntu 20.04 is not supported anymore. Please use Ubuntu 22.04 or newer.

Tested with Ubuntu 22.04 or newer.

Use `apt` to get your needed libraries installed:

.. code-block:: bash

  sudo apt update
  sudo apt install -y python3-pip git rsync wget cmake doxygen graphviz build-essential clang-tidy cppcheck openjdk-17-jdk npm docker.io docker-compose libboost-all-dev nodejs libssl-dev libsqlite3-dev clang-format curl rfkill libpcap-dev libevent-dev pkg-config libcap-dev

**OpenSUSE**:

Use `zypper` to get your needed libraries installed:

.. code-block:: bash

  zypper update && zypper install -y sudo shadow
  zypper install -y --type pattern devel_basis
  zypper install -y git rsync wget cmake doxygen graphviz clang-tools cppcheck boost-devel libboost_filesystem-devel libboost_log-devel libboost_program_options-devel libboost_system-devel libboost_thread-devel java-17-openjdk java-17-openjdk-devel nodejs nodejs-devel npm python3-devel python3-pip gcc-c++ libopenssl-devel sqlite3-devel libpcap-devel libevent-devel libcap-devel

**Fedora**:

Tested with Fedora 40, 41 and 42. Here is how to get your needed libraries with
`dnf`.

.. code-block:: bash

  sudo dnf update
  sudo dnf install make automake gcc gcc-c++ kernel-devel python3-pip python3-devel git rsync wget cmake doxygen graphviz clang-tools-extra cppcheck java-21-openjdk java-21-openjdk-devel boost-devel nodejs nodejs-devel npm openssl openssl-devel libsqlite3x-devel curl rfkill libpcap-devel libevent-devel libcap-devel

Now, it's time to continue with the
:doc:`Quick Start Guide to install EVerest <explanation/getting-started/get-started-sw>`.

Troubleshooting
===============

Some common problems during setting up your environment are collected here.

Maven dependency
----------------
For EVerest releases older than 2023.9.0 (released October 2nd 2023),
Maven is required for EVerest to run. Should you need to run one of those
versions, make sure you install the `maven` package with the package manager
of your choice.

Java dependency
---------------
Java is not required for running the core of EVerest. However, it is
required if you want to run the PyEvJosev EV simulation, as the EXI
(Efficient XML Interchange) part of Josev requires Java.

Python versions with pyenv
--------------------------
If you use `pyenv` for running multiple Python version in parallel on your
system, you probably will see `cmake` not using the Python version activated
by `pyenv`.

One solution to this is to run `cmake` with the `PYTHON_EXECUTABLE` flag. See
`cmake` documentation for more information on this.

------------------------------
Authors: Manuel Ziegler, Andreas Heinrich, Martin Litre, Moritz Barsnick