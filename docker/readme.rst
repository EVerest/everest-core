************
Docker setup
************

You need to install docker_ and docker-compose_.  Furthermore, `visual
studio code`_ might be handy as a common integrated development
environment.

In order for custom or local containers being able to talk to the
services, provided by the *docker-compose* containers, we need to create
a common docker network.  It is called ``infranet_network`` and needs to
be created by the following command (IPv6 is enabled for containers
which might it)::

  docker network create --driver bridge --ipv6  --subnet fd00::/80 infranet_network --attachable

Start *vscode*, open the workspace *docker-mqtt.code-workspace* and
install suggested extensions.  Open a shell in sub-directory ``docker``
and run (might take while for the first run)::

  docker-compose up -d

Now, the following services should be running:

- **Mosquitto MQTT broker** (service name: mqtt-server) with ports

  - ``1883``: mqtt tcp connection
  - ``9001``: mqtt websocket connection

- **mariadb** (service name: ocpp-db), sql database needed by **SteVe**

  - ``3306``: sql tcp connection

- **SteVe** (service name: steve) on port 8180 with endpoints

  - ``:8180/steve/manager/home``: web interface (login = admin:1234)
  - ``:8180/steve/services/CentralSystemService``: SOAP endpoint for
    OCPP
  - ``:8180/steve/websocket/CentralSystemService/(chargeBoxId)``:
    WebSocket/JSON endpoint for OCPP

These three services are defined in ``docker/docker-compose.yml`` and
they live inside the docker network ``docker_default`` with their
respective ports.  By default these ports are not directly accessible by
using ``localhost:8080`` for example.  The current configuration exposes
all these ports to the local host with some port mapping, so the often
used ports will not clash with other services running already on your
host system.  The mapping is as follows:

- ``1883`` -> ``1883``, ``9001`` -> ``9001`` for
  **mosquitto**
- ``13306`` -> ``3306`` for **mariadb**
- ``8180`` -> ``8180`` for **SteVe**

So, if you want to access the **mosquitto** default mqtt port via your
local host, you need to access ``localhost:1883``.  But if you want to
access it from a service or container inside the *docker_default*
network, you'll need to access ``mqtt-server:1883``. Using the docker
extension in *vscode*, you can show the logs of these services or attach
a shell to them by navigating to the docker tab and then right-clicking
on the specific container.


everest playground
==================

If you would like to get a pre-configured development setup using
*vscode* for the *everest-cpp* framework, you need to start up the mqtt
server with::

    docker-compose up -d mqtt-server

Then, using *vscode*, open up a new window with *Ctrl+Shift+N* (or use
the current), press *F1*, enter ``remopen``, select `Remote-Containers:
Open Folder in Container...`, head to the directory
``docker/everest-playground`` and open. This will build a docker image
with a standard development environment and start `vscode`
inside it.  This image will also link to the ``infranet_network`` network,
so it can access the mqtt service and possible other services.

In order to build the *everest-cpp* framework, create a directory called
``build`` and run::

  cmake PATH_TO_EVEREST_CPP
  make -j8

inside it.


.. _docker: https://docs.docker.com/engine/install/#server
.. _docker-compose: https://docs.docker.com/compose/install/#install-compose)
.. _visual studio code: https://code.visualstudio.com/docs/setup/linux
