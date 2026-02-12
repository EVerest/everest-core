.. _tutorial-ocpp16:

###################
OCPP 1.6 in EVerest
###################

.. note::

  EVerest has an implementation of OCPP 1.6J and 2.0.1 and 2.1. This tutorial is about
  the 1.6 implementation. To get documentation about all implemented versions,
  see `lib/ocpp in the everest-core repository <https://github.com/EVerest/everest-core/tree/main/lib/everest/ocpp>`_.

EVerest provides a complete implementation of Open Charge Point Protocol
(OCPP) 1.6J, supporting all feature profiles including Plug&Charge and the
Security Extensions.

The source code of `libocpp` is hosted as part of the everest-core repository:
`<https://github.com/EVerest/everest-core/tree/main/lib/everest/ocpp>`_.

This is a tutorial about how to set up and configure OCPP 1.6 in EVerest.

This tutorial includes:

- How to run EVerest SIL with the default OCPP 1.6J configuration connecting to
  SteVe
- How to load the OCPP 1.6 module as part of the EVerest configuration 
- How to configure OCPP 1.6 configuration keys
- How to connect to different CSMS

.. _tutorial_ocpp16_prerequisites:

Prerequisites
=============

If you're new to EVerest start with our :ref:`Quick Start Guide <htg_getting_started_sw>`
to get a simulation in EVerest running for the first time.
It is important that you have set up the required docker containers for
Mosquitto and SteVe, which we will use as an example CSMS.
If you have done that successfully, you can move on with this tutorial.

.. _tutorial_ocpp16_run_with_steve:

Run EVerest SIL with SteVe
==========================

EVerest's `everest-core` repository provides a configuration that you can use to run EVerest with OCPP.
By default this configuration is connecting to the Open Source CSMS
`SteVe <https://github.com/steve-community/steve>`_.
Make sure that SteVe is running on your machine as the CSMS we connect to.

You have to add the chargepoint id *cp001* in SteVe's webinterface to allow a
charging station to connect.
If you want to simulate charging sessions, you also need to add OCPP tags for
the authorization in SteVe.

Simply run

.. code-block:: bash

    ${EVEREST_WORKSPACE:?}/everest-core/build/run-scripts/run-sil-ocpp.sh

to start EVerest with OCPP 1.6J. You can start playing around with central
system-initiated messages and use the EVerest simulation to start charging
sessions.

You can find the OCPP message log in different formats in the
`/tmp/everest_ocpp_logs` directory. A new logfile is created every time EVerest
is started.

.. _tutorial_ocpp16_configure_ocpp:

OCPP configuration file
=======================

In addition to the EVerest configuration yaml file, OCPP 1.6 is configured
using a JSON configuration file.
This configuration file can contain all configuration keys from the OCPP 1.6
specification.
Examples for that can be found `here <https://github.com/EVerest/everest-core/tree/main/lib/everest/ocpp/config/v16>`_.

This is the one we used to connect to SteVe:

.. code-block:: json

  {
    "Internal": {
        "ChargePointId": "cp001",
        "CentralSystemURI": "127.0.0.1:8180/steve/websocket/CentralSystemService/",
        "ChargeBoxSerialNumber": "cp001",
        "ChargePointModel": "Yeti",
        "ChargePointVendor": "Pionix",
        "FirmwareVersion": "0.1",
        "AllowChargingProfileWithoutStartSchedule": true,
        "UseTPM" : false,
        "LogMessagesFormat": ["html","security"]
    },
    "Core": {
        "AuthorizeRemoteTxRequests": false,
        "ClockAlignedDataInterval": 900,
        "ConnectionTimeOut": 30,
        "ConnectorPhaseRotation": "0.RST,1.RST",
        "GetConfigurationMaxKeys": 100,
        "HeartbeatInterval": 86400,
        "LocalAuthorizeOffline": false,
        "LocalPreAuthorize": false,
        "MeterValuesAlignedData": "Energy.Active.Import.Register",
        "MeterValuesSampledData": "Energy.Active.Import.Register,SoC",
        "MeterValueSampleInterval": 60,
        "NumberOfConnectors": 1,
        "ResetRetries": 1,
        "StopTransactionOnEVSideDisconnect": true,
        "StopTransactionOnInvalidId": true,
        "StopTxnAlignedData": "Energy.Active.Import.Register",
        "StopTxnSampledData": "Energy.Active.Import.Register",
        "SupportedFeatureProfiles": "Core,FirmwareManagement,RemoteTrigger,Reservation,LocalAuthListManagement,SmartCharging",
        "TransactionMessageAttempts": 1,
        "TransactionMessageRetryInterval": 10,
        "UnlockConnectorOnEVSideDisconnect": true,
        "WebsocketPingInterval": 0
    },
    "FirmwareManagement": {
        "SupportedFileTransferProtocols": "FTP"
    },
    "Security": {
        "CpoName": "Pionix",
        "AuthorizationKey": "AABBCCDDEEFFGGHH",
        "SecurityProfile": 1
    },
    "LocalAuthListManagement": {
        "LocalAuthListEnabled": true,
        "LocalAuthListMaxLength": 42,
        "SendLocalListMaxLength": 42
    },
    "SmartCharging": {
        "ChargeProfileMaxStackLevel": 42,
        "ChargingScheduleAllowedChargingRateUnit": "Current,Power",
        "ChargingScheduleMaxPeriods": 42,
        "MaxChargingProfilesInstalled": 42
    },
    "PnC": {
        "ISO15118CertificateManagementEnabled": true,
        "ISO15118PnCEnabled": true,
        "ContractValidationOffline": true
    },
    "CostAndPrice": {
        "CustomDisplayCostAndPrice": false
    },
    "Custom": {
        "ExampleConfigurationKey": "example"
    }
  }

The configuration keys are split up into the feature profiles that are
specified in OCPP 1.6 plus the extra profiles *Internal*, *Security*, *PnC* and
*CostAndPrice*.
Here's a short overview of the purpose of each profile in the configuration file:

- Internal: Used for internal configuration keys that are not specified in
  OCPP 1.6
- Core: Includes Core configuration keys of OCPP 1.6
- FirmwareManagement: Includes configuration keys that apply when the feature
  profile FirmwareManagement is implemented
- Security: Includes configuration parameters that have been introduced within
  the OCPP 1.6J Security Whitepaper
- LocalAuthListManagement: Includes configuration parameters that apply when
  the feature profile LocalAuthListManagement is implemented
- SmartCharging: Includes configuration parameters that apply when the feature
  profile SmartCharging is implemented
- PnC: Used for Plug&Charge and includes configuration parameters that have
  been introduced within the OCPP 1.6J Plug&Charge Whitepaper
- CostAndPrice: Used for enabling support for the California Pricing Whitepaper

EVerest's `libocpp` supports almost all configuration parameters that are specified
within OCPP 1.6. Despite that, it is possible to omit configuration parameters
that are not required and it is even possible to omit a whole feature profile
in the configuration file if it is not supported. This means that the
configuration of the `libocpp` provides maximum flexibility and can be
tailored to your specific charging station.

.. note::

  There is a lot to configure with OCPP. Make sure to thoroughly read through
  the OCPP 1.6 specification and the
  `profile schemas <https://github.com/EVerest/everest-core/tree/main/lib/everest/ocpp/config/v16/profile_schemas>`_
  and configure OCPP according to your needs.

.. _tutorial_ocpp16_connect_different_csms:

Connect to a different CSMS
===========================

In order to connect to a different CSMS, you have to modify the connection
details within the OCPP configuration file. The OCPP config is a separate
JSON file in which all configuration keys of OCPP 1.6 plus some internal parameters
can be configured.

You can specify the path to this configuration file inside the ``everest-core``
configuration file using the configuration parameter ``ChargePointConfigPath``
of the OCPP module within everest-core. This defaults to *ocpp-config.json*.
If this path is relative, the default path for the OCPP configuration
*dist/share/everest/modules/OCPP* will be prepended.

To connect to a different CSMS, you have to modify the connection details of
the ocpp configuration file. This includes the parameter ``CentralSystemURI``
and it might also include to change the parameters ``AuthorizationKey`` and
``SecurityProfile``. Here's a short overview of the purpose of the parameters:

- ChargePointId: Identity of the charging station
- CentralSystemURI: Specifies the endpoint of the CSMS
  - Can optionally include the ChargePointId after the last back-slash of the URI

- SecurityProfile: Specifies the SecurityProfile which defines type of
  transport layer connection between ChargePoint and CSMS

  - Can have the value 0, 1, 2 or 3
  - SecurityProfile 0: Unsecure transport without Basic Authentication (ws://)
  - SecurityProfile 1: Unsecure transport with Basic Authentication (ws://)
  - SecurityProfile 2: TLS with Basic authentication (wss://)
  - SecurityProfile 3: TLS with client side certificates (wss://)

- AuthorizationKey: Specifies the password used for HTTP Basic Authentication

  - Must be set if SecurityProfile is 1 or 2, can be omitted if
    SecurityProfile is 0 or 3
  - Minimal length: 16 bytes

Modify these parameters according to the connection requirements of the CSMS. Find all available configuration keys
and their descriptions in `here <https://github.com/EVerest/everest-core/tree/main/lib/everest/ocpp/config/v16/profile_schemas>`_

.. note::

  For TLS, it might be required to prepare the required certificates and
  private keys. Please see the documentation of the
  :ref:`EvseSecurity module <everest_modules_EvseSecurity>`
  for more information on how to set up the TLS connection for OCPP.

.. _tutorial_ocpp16_configure_ocpp_everest:

Configuring OCPP 1.6 within EVerest
===================================

To be able to follow the further explanations, you should be familiar with the configuration of EVerest modules.
Take a look into :doc:`EVerest Module Concept </explanation/detail-module-concept>` for that.

To configure the OCPP module of everest-core, find the available configuration parameters
in the :ref:`module documentation <everest_modules_OCPP>`, and read through them
carefully in order to configure it according to your needs.

In order to enable OCPP 1.6 in EVerest, you need to load the module in the
EVerest configuration file and set up the module connections.
The interfaces provided and required by the OCPP module and its purposes are
described in the
:ref:`module documentation <everest_modules_OCPP>`.

The EVerest configuration file
`config-sil-ocpp.yaml <https://github.com/EVerest/everest-core/blob/main/config/config-sil-ocpp.yaml>`_
which was used previously serves as a good example for how the connections of
the module could be set up.

Here is a quick list of things you should remember when adding OCPP to your
EVerest configuration file:

1. **Load the OCPP module** by including it in the configuration file.

2. **Add and connect the required modules:**

   - **evse_manager** (interface: ``energy_manager``, 1 to 128 connections):

     OCPP requires this connection to integrate with the charge control logic of EVerest.
     One connection represents one EVSE.
     To manage multiple EVSEs via one OCPP connection, multiple connections must be configured
     in the EVerest configuration file.

     *Module implementation typically used to fulfill this requirement:*  
     ``EvseManager``, ``implementation_id: evse``

   - **evse_energy_sink** (interface: ``external_energy_limits``, 0 to 128):

     OCPP optionally requires this connection to communicate smart charging limits received
     from the CSMS within EVerest.

     Typically, ``EnergyNode`` modules are used to fulfill this requirement.
     Configure one ``EnergyNode`` module per EVSE and one extra for *evse id* zero.
     The ``EnergyNode`` for *evse id* zero represents the energy sink for the
     complete charging station.

     *Module typically used to fulfill this requirement:*  
     :ref:`EnergyNode <everest_modules_EnergyNode>`

     More information about the energy management setup can be found in the
     :doc:`EnergyManager module documentation </explanation/energymanagement/index>`.

   - **auth** (interface: ``auth``, 1):

     This connection is used to set the standardized ``ConnectionTimeout`` configuration key
     if configured and/or changed by the CSMS.

     *Module typically used to fulfill this requirement:*  
     :ref:`Auth module <everest_modules_Auth>`, ``implementation_id: main``

   - **reservation** (interface: ``reservation``, 1):

     This connection is used to apply reservation requests from the CSMS.

     *Module typically used to fulfill this requirement:*  
     :ref:`Auth module <everest_modules_Auth>`, ``implementation_id: reservation``

   - **system** (interface: ``system``, 1):

     This connection is used to execute and control system-wide operations that
     can be triggered by the CSMS, like log uploads, firmware updates, and resets.

     *Modules fulfilling this requirement:*

     - :ref:`Linux_Systemd_Rauc module <everest_modules_Linux_Systemd_Rauc>` (``implementation_id: main``)
     - For simulation purposes: :ref:`System module <everest_modules_System>` (``implementation_id: main``)

     .. note::
        The System module is **not meant to be used in production systems**.

   - **security** (interface: ``evse_security``, 1):

     This connection is used to execute security-related operations and to manage
     certificates and private keys.

     *Module typically used to fulfill this requirement:*  
     :ref:`EvseSecurity module <everest_modules_EvseSecurity>`, ``implementation_id: main``

   - **data_transfer** (interface: ``ocpp_data_transfer``, 0 to 1):

     This connection is used to handle **DataTransfer.req** messages from the CSMS.
     A module implementing this interface can contain custom logic to handle such requests.
     A custom implementation is required to add custom handling.

   - **display_message** (interface: ``display_message``, 0 to 1):

     This connection is used to allow the CSMS to display pricing or other information
     on the display of the charging station.

     To fulfill the requirements of the California Pricing whitepaper, it is required
     to connect a module implementing this interface.

     .. note::
        EVerest currently does **not** provide a display module that implements this interface.

3. **Configure OCPP with the Auth module:**

   Make sure to configure the OCPP module as part of the ``token_provider``
   (``implementation_id: auth_provider``) and ``token_validator``
   (``implementation_id: auth_validator``) connections of the Auth module (if you use it).

   Please see the documentation of the :ref:`Auth module <everest_modules_Auth>` for more information.

4. **Enable Plug & Charge (optional):**

   If you want to use the Plug&Charge feature, you must also add the
   ``EvseManager`` (``implementation_id: token_provider``) module to the connections
   of the Auth module.

   See :doc:`Configure Plug&Charge in EVerest </how-to-guides/configure-pnc>` for more
   information about how to set this up.

You can also use the existing configuration examples as a guide.

----

**Authors:** Piet Gömpel
