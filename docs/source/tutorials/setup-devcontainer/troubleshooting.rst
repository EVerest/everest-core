###############
Troubleshooting
###############

Port conflicts
--------------

Each instance uses the same ports (1883, 1880, 4000, etc.). Please note that only one instance can run at a time.
If another process is using the port you need here is how to detect it and make it available:

.. code-block:: bash

    sudo lsof -ti:1880 \| xargs sudo kill -9
    ./applications/devrd/devrd start

If a system service is using a port (e.g. mosquitto) stopping/disabling this service may be required.

Regenerate environment configuration
------------------------------------

If you suspect that the environment variables in the ``.env`` file
are not correct, you can regenerate it with:

.. code-block:: bash

    ./applications/devrd/devrd env

Customize environment variables
--------------------------------

To customize specific environment variables, you can edit the
``.devcontainer/.env`` file directly or pass them as arguments to the ``./applications/devrd/devrd env`` command.

.. note::

    If you manually edit the ``.env`` file and change ``EVEREST_TOOL_BRANCH``
    or other build arguments, you must rebuild the container for changes to take effect:

Rebuild devrd setup
-------------------

The setup can be rebuild with:

Option 1: Force rebuild (recommended)

.. code-block:: bash

    ./applications/devrd/devrd env                    # Regenerate .env if you edited it manually
    ./applications/devrd/devrd build                  # Rebuild with new environment variables

Option 2: Clean rebuild (if rebuild doesn't work)

.. code-block:: bash

    ./applications/devrd/devrd stop                   # Stop all containers, images, and volumes
    ./applications/devrd/devrd purge                  # Remove all containers, images, and volumes
    ./applications/devrd/devrd build                  # Rebuild from scratch


Purge and rebuild devrd setup
-----------------------------

.. code-block:: bash

    ./applications/devrd/devrd purge                  # Remove all resources for current folder
    ./applications/devrd/devrd build                  # Will generate .env if missing


Volume conflicts
----------------

Docker volumes are shared. Use

.. code-block:: bash

    ./applications/devrd/devrd purge

before switching instances.

SSH keys
--------

Ensure your SSH agent has the necessary keys for all repositories.

Container naming
----------------

Docker containers are named based on the workspace directory to avoid conflicts.

Switching between instances
---------------------------

When switching between different everest-core instances (e.g., different branches or forks),
use the following commands:

.. code-block:: bash

    # Stop current instance
    ./applications/devrd/devrd stop

    # Purge if switching to different branch/project
    ./applications/devrd/devrd purge

    # Start new instance
    cd ~/different-everest-directory
    ./applications/devrd/devrd start

----

**Authors:** Florian Mihut, Andreas Heinrich
