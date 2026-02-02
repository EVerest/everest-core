#####################################################
Use the EVerest Development Container (devrd managed)
#####################################################

The EVerest development container (devcontainer) provides
a consistent development environment for EVerest development
and sil testing.
With the devcontainer itself also service containers
are provided to run required services like databases,
message brokers, etc.

.. hint::
    
    This guide describes the usage of the devcontainer
    managed by the `devrd` cli tool.
    For the variant using the VSCode Dev Containers extension
    this is not required since everything is managed by VSCode
    automatically.

.. include:: /snippets/overview-devcontainer.rst

***************************
Provided Service Containers
***************************

Services are organized into logical profiles for easier management:

.. list-table::
    :header-rows: 1
    :widths: 20 20 20 20 20

    * - Profiles
      - Services
      - Container Name
      - URL
      - Purpose
    * - mqtt/ocpp/sil/all
      - **MQTT Server**
      - `<prefix>_devcontainer-mqtt-server-1`
      - localhost:1883
      - Basic MQTT broker
    * - ``ocpp/all``
      - **OCPP DB**
      - `<prefix>_devcontainer-ocpp-db-1`
      - Internal
      - OCPP database
    * - `ocpp/all`
      - **Steve (HTTP)**
      - `<prefix>_devcontainer-steve-1`
      - <http://localhost:8180>
      - OCPP backend management
    * - `sil/all`
      - **Node-RED UI**
      - `<prefix>_devcontainer-nodered-1`
      - <http://localhost:1880/ui>
      - SIL simulation interface
    * - `sil/all`
      - **MQTT Explorer**
      - `<prefix>_devcontainer-mqtt-explorer-1`
      - <http://localhost:4000>
      - MQTT topic browser
    * - `ocpp/sil/all`
      - **Dev Container**
      - `<prefix>_devcontainer-devcontainer-1`
      - Command line
      - The development container
    * - `ocpp/sil/all`
      - **Docker Proxy**
      - `<prefix>_devcontainer-docker-proxy-1`
      - Internal
      - Secure Docker API access

.. note::

    the `all` profile is a synthetic profile that includes all
    services. Use `./applications/devrd/devrd start all` or `./devrd start` (default)
    to start all services.

Where `<prefix>` is the docker compose project name prefix.

***********************************
Starting/Stopping Services/Profiles
***********************************

To start/stop service containers use the `devrd` cli tool,
with `./applications/devrd/devrd start` and `./devrd stop` commands.

Usage examples:

.. code-block:: bash

    # Start profiles
    ./applications/devrd/devrd start                  # Start all services (generates .env if missing)
    ./applications/devrd/devrd start all              # Start all services (same as above)
    ./applications/devrd/devrd start sil              # Start SIL simulation tools
    ./applications/devrd/devrd start ocpp             # Start OCPP backend
    ./applications/devrd/devrd start mqtt             # Start only MQTT server

    # Stop services
    ./applications/devrd/devrd stop                   # Stop all services
    ./applications/devrd/devrd stop all               # Stop all services (same as above)
    ./applications/devrd/devrd stop sil               # Stop SIL profile only
    ./applications/devrd/devrd stop ev-ws             # Stop all containers matching pattern 'ev-ws'

*******************
Devcontainer Access
*******************

There are two ways to access the devcontainer:

1. **Open an interactive shell in the devcontainer**

   To run commands inside the devcontainer, open an interactive shell
   with the devrd cli:

   .. code-block:: bash

       ./applications/devrd/devrd prompt

   This will open an interactive shell inside the devcontainer.
   The contents of the everest-core repository are mapped
   to the `/workspace` directory inside the container.

   You can now run all development commands inside this shell.

2. **Run a single command in the devcontainer**

   To run a single command inside the devcontainer, use the
   `devrd exec` command:

   .. code-block:: bash

       ./applications/devrd/devrd exec <command> [args...]

   This will run the specified command with optional arguments
   inside the devcontainer and return the output to the host terminal.

   Example:

   .. code-block:: bash

       ./applications/devrd/devrd exec ls /workspace

   This will list the contents of the `/workspace` directory
   inside the devcontainer.

********************************************
Clean Up Devcontainer and Service Containers
********************************************

To clean up the devcontainer and all service containers,
use the `./applications/devrd/devrd stop` and `./devrd purge` command:

.. code-block:: bash

    ./applications/devrd/devrd stop           # Stop all service containers
    ./applications/devrd/devrd purge          # Remove all service containers, images and volumes

***********************************
Manage Flows for Node-RED Container
***********************************

There are two commands one should know when working with Node-RED flows:

1. **List available flows**

   To list all available flows in the Node-RED container use the command:

   .. code-block:: bash

       ./applications/devrd/devrd flows

2. **Switch to specific flow file**

   To switch to a specific flow file in the Node-RED container use the command:

   .. code-block:: bash

       ./applications/devrd/devrd flow path/to/flow/file.json

   Where `path/to/flow/file.json` is the path to the flow file.

**********************************
Modify Workspace Directory Mapping
**********************************

While the default mapping of the everest-core repository
is to the `/workspace` directory inside the devcontainer,
this can be modified by using the `./applications/devrd/devrd env` command:

.. code-block:: bash

    ./applications/devrd/devrd env -w /path/to/workspace

This will mount the everest-core repository to the specified
`/path/to/workspace` directory inside the devcontainer.

Where `/path/to/workspace` is the desired path inside the container.


.. include:: /snippets/troubleshooting-devcontainer.rst

**Authors:** Florian Mihut, Andreas Heinrich