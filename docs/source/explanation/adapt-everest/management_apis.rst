.. management_apis:

###############
Management APIs
###############

While technically part of the :ref:`EVerest APIs <everest_apis>`, the management APIs
are a special case, since they are not intended for hardware integration or custom
extensions, but rather for remote management of EVerest itself. They are used for
example to manage the configuration of EVerest, e.g. to update configuration parameters
or to manage configuration sets, e.g. create/activate configurations.

There are two distinct management APIs available: The configuration_API and the
lifecycle_API. As use-cases demand both of them to be available when modules are not
running, they are implemented as part of the central manager process. They are
disabled by default and need to be enabled explicitly by adding the respective
command line argument to the EVerest startup command.

The APIs
========

The management APIs can be used independently of each other, i.e. it is possible to activate
only one of them. However, they can also be used together, e.g. to load an entirely new
configuration from a yaml string while EVerest is running and to restart the modules to
apply the new configuration.

configuration_API
-----------------

The configuration_API provides functionality to manage the configuration of EVerest.
This includes

- the management of configuration slots, i.e. creating, deleting or activating slots
- loading raw yaml data via the API, e.g. to load a new configuration slot with the
  content of a yaml file
- the management of configuration parameters, e.g. to update the value of a module
  configuration parameter for during runtime.
- querying the current configuration slots, e.g. to fetch the list of available
  configuration slots or the details of a specific slot.

The full specification can be found in the `AsyncAPI HTML documentation <../../reference/api/configuration_API/index.html>`_ .

To activate the configuration_API, use the ``--configuration-api`` flag when starting
EVerest, e.g. with the following command in your *build* folder:

.. code-block:: bash

   ./run-scripts/run-sil.sh --configuration-api

lifecycle_API
-------------

The lifecycle_API provides functionality to manage the lifecycle of EVerest modules.
This includes

- starting and stopping modules
- fetching the status of modules, e.g. whether they are running or not.

The full specification can be found in the `AsyncAPI HTML documentation <../../reference/api/lifecycle_API/index.html>`_ .

To activate the lifecycle_API, use the ``--lifecycle-api`` flag when starting
EVerest, e.g. with the following command in your *build* folder:

.. code-block:: bash

   ./run-scripts/run-sil.sh --lifecycle-api

AsyncAPI specification
======================

*Messages* are named to match the client's perspective. Variables which a client
subscribes to are named ``receive_{message_name}``. Commands which a client
can request to be executed are named ``send_request_{message_name}``. The
response to a command is named ``receive_reply_{message_name}``.

An example for the *active_slot* variable:

- message for the variable: *receive_active_slot*

Here is an example for the *list_all_slots* command of the configuration_API:

- message for the command: *send_request_list_all_slots*
- message for the response: *receive_reply_list_all_slots*


MQTT Topics
===========

The MQTT topics of the management APIs follow not quite the same but
similar pattern as the other EVerest APIs. All topics are prefixed with
*everest_api/1/{api_type}/* - with *1* being the version and *{api_type}*
the type of the API. Opposed to the other EVerest APIs, the management APIs
do not have a module_id in the topic, since they are not implemented as
separate modules.

The prefix is followed by the direction of the message. There are two
options:

-  *m2e*: "module to EVerest" for messages from the client to EVerest
-  *e2m*: "EVerest to module" for messages from EVerest to the client

This is finally followed by the name of the message. Here is a complete
example:

.. code-block:: text

   everest_api/1/configuration/e2m/active_slot
   everest_api/1/configuration/m2e/list_all_slots

The reply topic for a command is not fixed but is specified in the ``replyTo``
field of the command message. This allows clients to specify their own topic
for receiving the response, e.g. to distinguish responses from different
commands as well as from different invocations of the same command.
