:title: Managed by VSCode

###################################################
Setup Variant: Service Containers Managed by VSCode
###################################################

This variant makes use of the VSCode Dev Containers extension [#vscode_devcontainter]_
.
This will allow to run commands in the VSCode built-in terminal.
It will manage the service containers by starting them together with
the `devcontainer` itself. Dedicated control of individual services
is not possible with this variant.

******************************
What to expect from this setup
******************************

By following the steps in this tutorial you will setup a development
environment for EVerest using a development container (devcontainer).
VSCode will automatically build the container. After this all development
happens inside the container.

The contents of the everest-core repo are mapped inside the container
in the directory ``/workspace``
You can exit VSCode at any time, re-running it will cause VSCode to ask you
again to reopen in container.

Opening the everest-core repository in the VSCode devcontainer will
automatically start all of the provided service containers.

*************
Prerequisites
*************

To install the prerequisites, please check your operating system or distribution online documentation:

- Docker installed [#docker]_
- Docker compose installed version V2 (not working with V1) [#docker_compose]_
- VS Code with Dev Containers extension installed [#vscode_devcontainter]_

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

2. **Open in VSCode**

   Then open this repository in VSCode:

   .. code-block:: bash

       code path/to/everest-core
    
   Choose **Reopen in container** when prompted by VSCode.

Now VSCode will build the devcontainer.
This can take some time depending on your machine and internet connection.
After this you can use the built-in terminal in VSCode to run commands
inside the devcontainer.

****************************************
Tips for VSCode Dev Containers Extension
****************************************

When the repository is opened in VSCode you can enter the devcontainer at
any time by running the command **Dev Containers: Reopen in Container** from the
command palette (F1).
You can also stop the devcontainer by running the command **Dev Containers: Reopen Folder Locally**.

.. include:: /snippets/troubleshooting-devcontainer.rst

**Authors:** Florian Mihut, Andreas Heinrich

.. [#docker] `<https://docs.docker.com/engine/install/>`_
.. [#docker_compose] `<https://docs.docker.com/compose/install>`_
.. [#vscode_devcontainter] `<https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers>`_