######################################################
Setup Variant: Service Containers managed by devrd cli
######################################################

This variant makes use of the devrd cli to manage the service containers.
So this variant can be used if one is not using VSCode.
It allows a dedicated control of the service containers independent
of the devcontainer lifecycle. The devcontainer itself will not start
or stop any service containers. With this variant the VSCode built-in
terminal can't be used to run commands in the devcontainer directly.
For this the devrd cli provides a command to open an interactive session
in the devcontainer.

.. include:: /snippets/overview-devcontainer.rst

******************************
What to expect from this setup
******************************

If one prefers to run the devcontainer outside VSCode, this setup variant is
the right choice.

The devrd cli will help to manage the service containers independent
of the devcontainer lifecycle. So one can start and stop the service containers
at any time.
The contents of the everest-core repo are mapped inside the container
in the directory `/workspace`

*************
Prerequisites
*************

To install the prerequisites, please check your operating system or distribution online documentation:

- :ref: `Docker installed <install_docker>`
- :ref: `Docker compose installed version V2 (not working with V1). <install_docker>`

**************
Required Steps
**************

1. **Clone the everest-core repository**

    If you have not done this yet, clone the everest-core repository
    from GitHub to your local machine:

    .. code-block:: bash

        git clone https://github.com/everest-core/everest-core.git path/to/everest-core
    
    Where `path/to/everest-core` is the path where you want to
    clone the repository to.

2. **Build and start the devcontainer and service containers**

    Change into the cloned repository directory:

    .. code-block:: bash

        cd path/to/everest-core

    Then build and start the devcontainer and the service containers
    with the devrd cli:

    .. code-block:: bash

        devrd start

    This will build and start the devcontainer and all service containers.
    If not yet existing, devrd will generate the `.devcontainer/.env` file.

3. **Open an interactive shell in the devcontainer**

    To run commands inside the devcontainer, open an interactive shell
    with the devrd cli:

    .. code-block:: bash

        devrd prompted

    This will open an interactive shell inside the devcontainer.
    The contents of the everest-core repository are mapped
    to the `/workspace` directory inside the container.

    You can now run all development commands inside this shell.

    To exit the shell, simply type `exit` or press `Ctrl+D`.

*************************************
Optional: Install bash/zsh completion
*************************************

If you want to enable completion on your host system follow the steps below.

Install bash completion
-----------------------

Add the following lines to your ~/.bashrc file:

.. code-block:: bash

    # bash completion for devrd cli
    source applications/devrd/devrd-completion.bash

Then reload your ~/.bashrc file:

.. code-block:: bash

    source ~/.bashrc

Install zsh completion
----------------------

Add the following lines to your ~/.zshrc file:

.. code-block:: bash

    # zsh completion for devrd cli
    autoload -U compinit && compinit
    source applications/devrd/devrd-completion.zsh

Then reload your ~/.zshrc file:

.. code-block:: bash

    source ~/.zshrc

.. include:: /snippets/troubleshooting-devcontainer.rst

**Authors:** Florian Mihut, Andreas Heinrich

.. _install_vscode_dev_containers_extension: https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers
.. _install_docker: https://docs.docker.com/engine/install/
