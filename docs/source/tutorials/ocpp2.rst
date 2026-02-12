.. _tutorial-ocpp2:

*****************************
OCPP 2.0.1 and 2.1 in EVerest
*****************************

.. note::

  EVerest has an implementation of OCPP 1.6J and 2.0.1 and 2.1. This tutorial is about
  the 2.0.1 and 2.1 implementation. To get documentation about all implemented versions,
  see `lib/ocpp in the everest-core repository <https://github.com/EVerest/everest-core/tree/main/lib/everest/ocpp>`_.

OCPP2.0.1 and OCPP2.1 in EVerest
=================================

EVerest provides an implementation of OCPP 2.0.1 and 2.1 based on
`libocpp` which is hosted as part of the `everest-core repository <https://github.com/EVerest/everest-core/tree/main/lib/everest/ocpp>`_. Since OCPP 2.0.1 and 2.1
is mostly backwards compatible, the implementation of OCPP 2.1 is based on the
2.0.1 implementation. Every functionality that is provided as part of OCPP 2.0.1
is also available in OCPP 2.1.

In EVerest, the :ref:`OCPP201 module <everest_modules_OCPP>`
provides the OCPP 2.0.1 and 2.1 functionality.

Where applicable the following documentation uses "2.x" to refer to both versions.

.. note::
  
  Which OCPP2.x version are supported can be controlled using the ``SupportedOcppVersions`` variable
  of the ``InternalCtrlr`` component of the device model. Please see the
  :ref:`Device Model Configuration <tutorial-ocpp2-configure-ocpp>` section for more information.
  By default, both versions are supported. It still depends on the CSMS which version is actually used.
  For more information about the version negotiation, please refer to the
  **OCPP 2.1 Part 4 - JSON over WebSockets implementation guide**

-----------------------------------------------

This tutorial includes:

- How to run EVerest SIL with a `simple CSMS <https://github.com/EVerest/ocpp-csms>`_
- How to configure the OCPP 2.x device model
- How to connect to different CSMS
- How to load the OCPP 2.x module as part of the EVerest configuration

.. _tutorial-ocpp2-prerequisites:

Prerequisites
=============

If you're new to EVerest, start with our
:ref:`Quick Start Guide <htg_getting_started_sw>`
to get a simulation in EVerest running for the first time.
If you have done that successfully, you can move on with this tutorial.

.. _tutorial-ocpp2-run-with-csms:

Run EVerest SIL with OCPP 2.x and a simple CSMS
=================================================

EVerest provides a `simple CSMS <https://github.com/EVerest/ocpp-csms>`_ that
can be used for testing. It works with both OCPP 2.0.1 and 2.1.
It responds "friendly" to all OCPP messages initiated by the charging station.
Follow the instruction of its README to start up the CSMS locally.

EVerest's `everest-core` repository provides a configuration that you can use
to run EVerest with OCPP 2.x.
By default, this configuration is connecting to `localhost:9000` which is also
the default address and port of our simple CSMS.

Simply run

.. code-block:: bash

    ${EVEREST_WORKSPACE:?}/everest-core/build/run-scripts/run-sil-ocpp201-pnc.sh

to start EVerest with OCPP 2.x support and Plug&Charge enabled. You can start playing around with the EVerest
simulation to start charging sessions.

You can find the OCPP message log in different formats in the
`/tmp/everest_ocpp_logs` directory.

.. _tutorial-ocpp2-configure-ocpp:

Device Model Configuration
==========================

OCPP 2.x defines a device model structure and a lot of standardized variables
that are used within the functional requirements of the protocol.
Please see "Part 1 - Architecture & Topology" of the OCPP 2.0.1 specification
for further information about the device model and how it is composed.
You should be familiar with the terms OCPP 2.x terms **Component**,
**Variable**, **VariabeCharacteristics** and **VariableAttributes**,
**VariableMonitoring** to be able to follow the further explanations.

OCPP 2.x does not differentiate between configuration and telemetry
variables. This provides a lot of flexibility, but it also adds some overhead
to the definition of configuration variables (e.g. for configuration variables
only the ``actual`` attribute is actually relevant, but ``target``, ``minSet``,
and ``maxSet`` attribute types are never used and not needed for simple
configuration variables).

Device Model definition and configuration structure
---------------------------------------------------

In libocpp, the device model is defined and configured using JSON files.
These files serve two main purposes:

* **Definition**: the device model (including its components and variables)
* **Configuration**: the value of variable attributes

There is one JSON file for each Component.
Each Component contains the definition of its Variables.
Each Variable contains the definition of its VariableCharacteristics,
VariableAttributes and VariableMonitoring.
The actual value of a Variable can be configured as part of the
VariableAttribute(s).

This is how a definition and configuration for the ``LocalAuthListCtrlr``
component could look like:

.. code-block:: json

  {
    "description": "Schema for LocalAuthListCtrlr",
    "name": "LocalAuthListCtrlr",
    "type": "object",
    "properties": {
      "LocalAuthListCtrlrAvailable": {
        "variable_name": "Available",
        "characteristics": {
          "supportsMonitoring": true,
          "dataType": "boolean"
        },
        "attributes": [
          {
            "type": "Actual",
            "mutability": "ReadOnly",
            "value": true
          }
        ],
        "description": "Local Authorization List is available.",
        "default": true,
        "type": "boolean"
      },
      "BytesPerMessageSendLocalList": {
        "variable_name": "BytesPerMessage",
        "characteristics": {
          "supportsMonitoring": true,
          "dataType": "integer"
        },
        "attributes": [
          {
            "type": "Actual",
            "mutability": "ReadOnly",
            "value": 4096
          }
        ],
        "description": "Maximum number of bytes in a SendLocalList message.",
        "type": "integer"
      },
      "LocalAuthListCtrlrEnabled": {
        "variable_name": "Enabled",
        "characteristics": {
          "supportsMonitoring": true,
          "dataType": "boolean"
        },
        "attributes": [
          {
            "type": "Actual",
            "mutability": "ReadWrite",
            "value": true
          }
        ],
        "description": "If this variable exists and reports a value of true, Local Authorization List is enabled.",
        "default": true,
        "type": "boolean"
      },
      "LocalAuthListCtrlrEntries": {
        "variable_name": "Entries",
        "characteristics": {
          "supportsMonitoring": true,
          "dataType": "integer"
        },
        "attributes": [
          {
            "type": "Actual",
            "mutability": "ReadOnly"
          }
        ],
        "description": "Amount of IdTokens currently in the Local Authorization List",
        "type": "integer"
      },
      "ItemsPerMessageSendLocalList": {
        "variable_name": "ItemsPerMessage",
        "characteristics": {
          "supportsMonitoring": true,
          "dataType": "integer"
        },
        "attributes": [
          {
            "type": "Actual",
            "mutability": "ReadOnly",
            "value": 250
          }
        ],
        "description": "Maximum number of records in SendLocalList",
        "type": "integer"
      },
      "LocalAuthListCtrlrStorage": {
        "variable_name": "Storage",
        "characteristics": {
          "unit": "B",
          "supportsMonitoring": true,
          "dataType": "integer"
        },
        "attributes": [
          {
            "type": "Actual",
            "mutability": "ReadOnly"
          }
        ],
        "description": "Indicates the number of bytes currently used by the Local Authorization List. MaxLimit indicates the maximum number of bytes that can be used by the Local Authorization List.",
        "type": "integer"
      },
      "LocalAuthListCtrlrDisablePostAuthorize": {
        "variable_name": "DisablePostAuthorize",
        "characteristics": {
          "supportsMonitoring": true,
          "dataType": "boolean"
        },
        "attributes": [
          {
            "type": "Actual",
            "mutability": "ReadWrite"
          }
        ],
        "description": "When set to true this variable disables the behavior to request authorization for an idToken that is stored in the local authorization list with a status other than Accepted, as stated in C14.FR.03.",
        "type": "boolean"
      }
    },
    "required": [
      "BytesPerMessageSendLocalList",
      "ItemsPerMessageSendLocalList",
      "LocalAuthListCtrlrEntries"
    ]
  }

You can change the components according to your needs, but note that the
definitions for the ``variable_name`` and ``characteristics`` are usually defined
by the OCPP 2.x specifications.
To configure a variable attribute value, specify the ``value`` for the attribute
type that you would like to configure.
In the example above, the actual value of the VariableAttribute of the Variable
``Enabled`` is set to ``true``. Note that not all variables have specified variable
attributes with a ``value``, e.g. ``LocalAuthListCtrlrEntries`` does not specify a
value. ``LocalAuthListCtrlrEntries`` is rather a telemetry than configuration,
so libocpp will set the value for this at runtime and therefore it is not
required to configure a value for it.
It's an example for a variable that is only defined, but not configured.

.. _tutorial-ocpp2-device-model-init:

Device Model initialization
---------------------------

The config files are parsed at startup and used to initialize an SQLite
database. Please see
`the documentation about the device model initialization <https://github.com/EVerest/everest-core/blob/main/lib/everest/ocpp/doc/v2/ocpp_201_device_model_initialization.md>`_
for detailed information about this process.

You should specify the path to the directory of your device model definitions
using the configuration parameter ``DeviceModelConfigPath``
of the OCPP201 module within everest-core.
It shall point to the directory where the component files are located in these
two subdirectories:

* standardized
* custom

By default, the default value for ``DeviceModelConfigPath`` is pointing to the
installation directory of the component files.
You can modify the component according to your specific needs and the design of
your charging station.

Libocpp provides a device model configuration as a starting point
-----------------------------------------------------------------

You can define custom components and variables according to the requirements
and setup of your charging station. There are a lot of
standardized components and variables in OCPP 2.x that are required and used
in functional requirements of the specification. Please have
a look at the OCPP 2.x specifications for more information about each of the
standardized components and variables.
For this reason, it is recommended to use the
`device device model definitions of libocpp <https://github.com/EVerest/everest-core/tree/main/lib/everest/ocpp/config/v2/component_config>`_
as a starting point. This is an examplary device model configuration for two
EVSEs.

The `device model setup from libocpp <https://github.com/EVerest/everest-core/tree/main/lib/everest/ocpp/config/v2/component_config>`_
serves as a good example. 

The split between the two directories only has semantic reasons.
The **standardized** directory usually does not need to be modified since it
contains standardized components and variables that the specification refers
to in its functional requirements.
The **custom** directory is meant to be used for components that are custom
for your specific charging station.

The following sections explain important component and variables in order to
connect to a different CSMS or to enable certain features.

.. _tutorial-ocpp2-different-csms:

Connect to a different CSMS
---------------------------

In order to connect to a different CSMS, you have to modify the connection
details within your device model configuration:

- ``NetworkConnectionProfiles`` in the ``InternalCtrlr``. Note that this is a JSON array, so you can define multiple connection profiles.
  - ``ocppCsmsUrl``: Specifies the endpoint of the CSMS
  - ``securityProfile``: Specifies the SecurityProfile which defines type of transport layer connection between ChargePoint and CSMS
- ``Identity`` in the ``SecurityCtrlr``: The identity of the charging station
- ``BasicAuthPassword`` in the ``SecurityCtrlr``: Specifies the password used for HTTP Basic Authentication (SecurityProfile 1 or 2)

Modify these parameters according to the connection requirements of the CSMS.

.. note::

  For TLS, it might be required to prepare the required certificates and private keys.
  Please see the documentation of the
  :ref:`EvseSecurity module <everest_modules_EvseSecurity>` for more information 
  on how to set up the TLS connection for OCPP.

.. _tutorial-ocpp2-enable-pnc:

Enable Plug&Charge
------------------

In order to enable Plug&Charge, adjust your component files according to the
:doc:`Plug&Charge configuration documentation </how-to-guides/configure-pnc>`.

.. _tutorial-configure-ocpp2-everest:

Configuring the OCPP201 module within EVerest
=============================================

To be able to follow the further explanations, you should be familiar with the configuration of EVerest modules.
Take a look into :doc:`EVerest Module Concept </explanation/detail-module-concept>` for that.

To configure the OCPP201 module of everest-core, find the available configuration parameters
and carefully read the further explanations in the
:ref:`OCPP201 module documentation <everest_modules_OCPP201>`
in order to configure it according to your needs.

In order to enable OCPP2.x in EVerest, you need to load the module in the EVerest configuration file and set up the module connections. The interfaces
provided and required by the OCPP module and its purposes are described in the :ref:`OCPP201 module documentation <everest_modules_OCPP201>`.

The EVerest configuration file `config-sil-ocpp201.yaml <https://github.com/EVerest/everest-core/blob/main/config/config-sil-ocpp201.yaml>`_
which was used previously serves as a good example
for how the connections of the module could be set up.

Here is a quick list of things you should remember when adding OCPP201 to your EVerest configuration file:

1. Load the OCPP201 module by including it in the the configuration file.

2. Make sure to add and connect the module requirements:

   - evse_manager (interface: energy_manager, 1 to 128 connections):
     OCPP201 requires this connection in order to integrate with the charge control
     logic of EVerest.
     One connection represents one EVSE.
     In order to manage multiple EVSEs via one OCPP connection, multiple
     connections need to be configured in the EVerest config file.
     Module implementation typically used to fullfill this requirement:
     :ref:`EvseManager <everest_modules_EvseManager>`, implementation_id: evse
   - evse_energy_sink (interface: external_energy_limits, 0 to 128):
     OCPP optionally requires this connection to communicate smart charging
     limits received from the CSMS within EVerest.
     Typically EnergyNode modules are used to fullfill this requirement.
     Configure one EnergyNode module per EVSE and one extra for *evse id* zero.
     The EnergyNode for *evse id* zero represents the energy sink for the
     complete charging station.
     Module typically used to fullfill this requirement:
     :ref:`EnergyNode <everest_modules_EnergyNode>`, implementation_id: external_limits
     More information about the energy management setup can be found in the
     :ref:`EnergyManager module documentation <everest_modules_EnergyManager>`.
   - auth (interface: auth, 1): This connection is used to set the standardized
     ``ConnectionTimeout`` configuration key if configured and/or changed by the
     CSMS.
     Module typically used to fullfill this requirement:
     :ref:`Auth <everest_modules_Auth>`, implementation_id: main
   - reservation (interface: reservation, 1):
     This connection is used to apply reservation requests from the CSMS.
     Module typically used to fullfill this requirement:
     :ref:`Auth <everest_modules_Auth>`, implementation_id: reservation
   - system (interface: system, 1):
     This connection is used to execute and control system-wide operations that
     can be triggered by the CSMS, like log uploads, firmware updates, and
     resets.
     The :ref:`Linux_Systemd_Rauc module <everest_modules_Linux_Systemd_Rauc>` (implementation_id: main)
     can be used to fullfill this requirement.
     For simulation purposes, the :ref:`System module <everest_modules_System>` (implementation_id: main)
     can be used. Note that the latter is not meant to be used in production systems!
   - security (interface: evse_security, 1):
     This connection is used to execute security-related operations and to
     manage certificates and private keys.
     Module typically used to fullfill this requirement:
     :ref:`EvseSecurity <everest_modules_EvseSecurity>`, implementation_id: main
   - data_transfer (interface: ocpp_data_transfer, 0 to 1):
     This connection  is used to handle **DataTransfer.req** messages from the
     CSMS.
     A module implementing this interface can contain custom logic to handle the
     requests from the CSMS.
     A custom implementation for this interface is required to add custom
     handling.
   - display_message (interface: display_message, 0 to 1):
     This connection is used to allow the CSMS to display pricing or other
     information on the display of the charging station.
     In order to fulfill the requirements of the California Pricing whitepaper,
     it is required to connect a module implementing this interface.
     EVerest currently does not provide a display module that implements this
     interface.

3. Make sure to configure the OCPP201 module as part of the token_provider (implementation_id: auth_provider) and token_validator (implementation_id: auth_validator)
   connections of the Auth module (if you use it). Please see the documentation of the auth module for more information.

4. In case you want to use the Plug&Charge feature, you must also add the EvseManager (implementation_id: token_provider) module to the connections of the
   Auth module.

You can also use the existing config examples as a guide.



----

**Authors**: Piet Gömpel
