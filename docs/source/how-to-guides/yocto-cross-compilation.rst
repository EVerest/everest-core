.. _howto_yocto_cross_compilation:

############################
Cross-compilation of EVerest
############################

This is a how-to-guide on cross-compiling EVerest for a Yocto-based
Linux system.

For in-depth explanations about using EVerest with Linux and Yocto, please
refer to the :doc:`Linux / Yocto explanations </explanation/linux-yocto/index>`.

.. _exp_cross_compilation_building_an_sdk:

Building a Yocto SDK
=====================

This route will be available if you have already built your Yocto images
yourself, and we will assume for now that you already have a working
Yocto environment to build your Yocto images.

To build an SDK that can be used for cross-compilation, after sourcing
your Yocto environment you will just have to run

.. code-block:: bash

   bitbake IMAGE_NAME -c populate_sdk

After that you will be able to find the installer of your
newly-generated SDK under ``deploy*/sdk/``.

Cross-compiling EVerest using a Yocto SDK
=========================================

Before you can cross-compile EVerest for your target system, you will
have to acquire a Yocto SDK that is based on the Yocto image that is
currently running on the target you want to cross-compile for.

You can either `build one yourself <exp_cross_compilation_building_an_sdk>` or be supplied
with one by the organization that is building and maintaining the Yocto
image you are using.

TODO potentially use other SDK!

.. tip::

   You can download an SDK for the phyVERSO board here:
   [SDK BaseCamp/phyVERSO](http://pionix-update.de/phyverso/1.0.0/phytec-ampliphy-rauc-glibc-x86_64-phyverso-basecamp-image-aarch64-toolchain-BSP-Yocto-Ampliphy-AM62x-PD23.2.1-phyVERSO-EVCS.sh).
   To install the SDK, follow the steps below.

Whether you have created an SDK by yourself or are supplied with one,
the first step you will have to do will be to install the SDK on your
development machine. For that, you will first have to make the SDK
installer file executable and execute it.

.. code-block:: bash

   chmod +x name_of_sdk_installer.sh
   ./name_of_sdk_installer.sh

After that, the installer will ask you for the path in which you want to
install the SDK in.

You are free to install it in your preferred directory.

Whenever you want to do any compilations using this SDK/cross-toolchain,
you will have to source the respective environment-setup file of the
given SDK, which resides in the SDK's install location.

To source the cross-toolchain environment use the following syntax:

.. code-block:: bash

   source /path/to/sdk/environment-setup-XYZ.sh

``XYZ`` depends on the architecture and distribution name of your Yocto
image/SDK.

Use ``ls`` or tab-completion to find the full name of the
environment-setup-\*.sh you want to use.

Now that the SDK is installed on your development machine and you know
how to source the cross-toolchain, we can start fetch the EVerest
sources and cross-compile them using the SDK-supplied toolchain.

Clone the ``everest-core`` repository and checkout the branch/tag/commit you
want to cross-compile.

.. code-block:: bash

   git clone git@github.com:EVerest/everest-core.git
   cd everest-core
   git checkout TAG

.. warning::

   Make sure to source the SDK's toolchain first before continuing!

You can now create a ``build-cross`` directory, where you will do your
cross-compilation and run cmake with your desired additional flags.

.. code-block:: bash

   mkdir build-cross
   cd build-cross
   cmake ..

After this completes, you can start the compilation process with
``make -jN install``, whereas ``N`` equals the amount of concurrent jobs
you want to run. For this example we will go with a count of 16
concurrent processes:

.. code-block:: bash

   make -j16 install

This will cross-compile and “install” EVerest into
``everest-core/build-cross/dist/``. You can now ``rsync`` the cross-compiled
EVerest to your target (the next command assumes that your target is
accessible at 192.168.3.11):

.. code-block:: bash

   rsync -av dist/ root@192.168.3.11:/var/everest

You can now SSH into your target and stop the normally running
``everest`` process:

.. code-block::

   systemctl stop everest

After that, you can start your cross-compiled EVerest via:

.. code-block::

   LD_LIBRARY_PATH=/var/everest/lib:$LD_LIBRARY_PATH /var/everest/bin/manager --prefix /var/everest/ --config /path/to/your/config

------------------------------------------

Authors: Cornelius Claussen
