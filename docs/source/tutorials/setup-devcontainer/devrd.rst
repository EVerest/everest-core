:title: Managed by devrd CLI

######################################################
Setup Variant: Service Containers Managed by devrd cli
######################################################

This variant utilizes the devrd CLI to manage service containers,
making it an ideal choice for users who do not use VSCode.
It provides dedicated control of the service containers independent
of the devcontainer lifecycle.

Note that when using this variant and still using VSCode, the VSCode
built-in terminal cannot execute commands directly within the devcontainer.
Instead, use the devrd CLI to open an interactive session.

******************************
What to expect from this setup
******************************

If one prefers to run the devcontainer outside VSCode, this setup variant is
the right choice.

The devrd cli will help to manage the service containers independent
of the devcontainer lifecycle. So one can start and stop the service containers
at any time.
The contents of the everest-core repo are mapped inside the container
in the directory ``/workspace``

*************
Prerequisites
*************

To install the prerequisites, please check your operating system or distribution online documentation:

- Docker installed [#docker]_
- Docker compose installed version V2 (not working with V1) [#docker_compose]_

**************
Required Steps
**************

1. **Clone the everest-core repository**

   If you have not done this yet, clone the everest-core repository
   from GitHub to your local machine:

   .. code-block:: bash

       git clone https://github.com/everest-core/everest-core.git path/to/everest-core
    
   Where ``path/to/everest-core`` is the path where you want to
   clone the repository to.

2. **Build and start the devcontainer and service containers**

   Change into the cloned repository directory:

   .. code-block:: bash

       cd path/to/everest-core

   Then build and start the devcontainer and the service containers
   with the devrd cli:

   .. code-block:: bash

       ./application/devrd/devrd start

   This will build and start the devcontainer and all service containers.
   If not yet existing, devrd will generate the ``.devcontainer/.env`` file.

3. **Open an interactive shell in the devcontainer**

   To run commands inside the devcontainer, open an interactive shell
   with the devrd cli:

   .. code-block:: bash

       ./application/devrd/devrd prompt

   This will open an interactive shell inside the devcontainer.
   The contents of the everest-core repository are mapped
   to the ``/workspace`` directory inside the container.

   You can now run all development commands inside this shell.

   To exit the shell, simply type ``exit`` or press ``Ctrl+D``.

*************************************
Optional: Install bash/zsh completion
*************************************

If you want to enable completion on your host system follow the steps below.

Install bash completion
-----------------------

Add the following lines to your ``~/.bashrc`` file:

.. code-block:: bash

    # bash completion for devrd cli
    source applications/devrd/devrd-completion.bash

Then reload your ``~/.bashrc`` file:

.. code-block:: bash

    source ~/.bashrc

Install zsh completion
----------------------

Add the following lines to your ``~/.zshrc`` file:

.. code-block:: bash

    # zsh completion for devrd cli
    autoload -U compinit && compinit
    source applications/devrd/devrd-completion.zsh

Then reload your ``~/.zshrc`` file:

.. code-block:: bash

    source ~/.zshrc

.. include:: /snippets/troubleshooting-devcontainer.rst

**Authors:** Florian Mihut, Andreas Heinrich

.. [#docker] `<https://docs.docker.com/engine/install/>`_
.. [#docker_compose] `<https://docs.docker.com/compose/install>`_
