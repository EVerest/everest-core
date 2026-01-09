.. _everest_modules_handwritten_RpcApi:

*******************************************
RpcApi
*******************************************

Version Information
===================
Version history of the module:

.. list-table::
   :widths: 15 85
   :header-rows: 1

   * - Version
     - Description
   * - 1.0.0
     - Initial version of the RpcApi module

Table of Contents
=====================

.. contents:: Table of Contents
   :depth: 3
   :local:

Introduction
------------
The RPC API provides a standardized interface for external applications to interact with an EVerest-based
ChargePoint. It is designed as a lightweight and transport-independent communication layer that enables
monitoring, control, and integration into higher-level energy management systems.

The API follows the **JSON-RPC 2.0** standard and uses **WebSocket** as the default transport channel,
offering a persistent and bidirectional connection between client and server. This ensures low latency,
efficient message exchange, and clear request–response semantics.

Typical use cases include:

* Integration of ChargePoints into energy or fleet management platforms
* Building mobile or web applications for end-users
* Monitoring charging sessions
* Controlling operational parameters such as charging power, current, or connector states
* Accessing structured error and status information for diagnostics

Key features:

* **Transport independence** – designed to work over WebSocket today, extendable to other transports in the future
* **Request/response and notifications** – methods for configuration and control, notifications for event-driven updates
* **Structured data model** – standardized objects and enums for consistent integration
* **Scalable design** – suitable for single-user tools as well as multi-user systems

In short, the RPC API serves as the **communication bridge** between the charging infrastructure and
external applications, providing a reliable, extensible, and system-friendly interface for monitoring and control.

General
-------
Feature Overview
~~~~~~~~~~~~~~~~
+---------------------------------------+-----------+
| Feature                               | Supported |
+=======================================+===========+
| WebSocket transport - no TLS          | ✅        |
+---------------------------------------+-----------+
| WebSocket transport - with TLS        | ❌        |
+---------------------------------------+-----------+
| Authentication / Permission handling  | ❌        |
+---------------------------------------+-----------+
| Scope configuration                   | ❌        |
+---------------------------------------+-----------+
| Support for multiple EVSEs            | ✅        |
+---------------------------------------+-----------+
| Support for multiple connectors       | ❌        |
+---------------------------------------+-----------+
| EVSE information retrieval            | ✅        |
+---------------------------------------+-----------+
| EVSE status retrieval                 | ✅        |
+---------------------------------------+-----------+
| Hardware capabilities retrieval       | ✅        |
+---------------------------------------+-----------+
| Meter data retrieval                  | ✅        |
+---------------------------------------+-----------+
| Control of charging current (AC)      | ✅        |
+---------------------------------------+-----------+
| Control of charging power (DC)        | ✅        |
+---------------------------------------+-----------+
| Control of phase count (AC)           | ✅        |
+---------------------------------------+-----------+
| Connector enable/disable              | ✅        |
+---------------------------------------+-----------+
| Error monitoring (active errors)      | ✅        |
+---------------------------------------+-----------+
| Notifications for status/capabilities | ✅        |
+---------------------------------------+-----------+
| DC charge parameters                  | ❌        |
+---------------------------------------+-----------+
| DC charge status                      | ❌        |
+---------------------------------------+-----------+
| Display parameters (ISO15118-20 data) | ❌        |
+---------------------------------------+-----------+

Authentication
~~~~~~~~~~~~~~
(Currently not supported)

The API should optionally support a client authentication mechanism. This can be used to introduce
permission management, which can be used to control which functions a client may access, and which
functions it may not.

If authentication is required, each call, except the initial messages to exchange e.g. the used API
version, is required to contain a valid authentication token. How this authentication token is created
is not part of this specification and must be specified during client and server development.

Tools
~~~~~
The *tools* subdirectory contains a Python-based JSON-RPC GUI client. This client allows testing of the
interface implementation and can serve as a reference example for developing your own client.

API Methods and Notifications
-----------------------------

General
~~~~~~~
The JSON API is generated from the json_rpc_api.yaml specification. This YAML file serves as the primary
reference for all available methods and notifications. It also defines which parameters are required
and which are optional.

Hierarchy of methods & notifications
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* **API** – general methods, no effect on charge point
* **ChargePoint** – affects the entire charging station
* **EVSE** – relates to a specific EVSE (Electric Vehicle Supply Equipment)

Methods
~~~~~~~
This includes all calls that can be executed by the client. Please note that if authentication is active,
a token must be included in the request except for API.Hello. Otherwise, the request will be rejected.
The following chapter headings represent the method name of the JSON-RPC protocol. The response and
requests objects shown in the following chapters map the “params” value in the JSON-RPC object. If
"params" is marked as "{}" it means that no parameters are required and the "params" object can be omitted.
An example JSON-RPC request without "params" is shown in the :ref:`API.Hello request section <example-json-rpc-request-with-params>`
and an example JSON-RPC request with "params" is shown in the :ref:`EVSE.GetInfo request section <example-json-rpc-request-with-params>`.

⚠️ Note: Configuring charging parameters (such as charging current limits) via the API interface cannot
always be guaranteed, since these values may also be influenced by other sources within EVerest. For example,
if EVerest has been configured with an upper limit of 12A, this value cannot be exceeded via the API interface.
In this specific case, if a client attempts to configure 16A through the API, the applied value will
still be limited to 12A. Therefore, it is important to always observe the configured values returned
via the EVSE.StatusChanged notification.

In future versions, unsuccessful configuration attempts may also result in an error response instead of
silently applying the nearest valid limit.

API.Hello
^^^^^^^^^
This method is used to perform an initial handshake with the server. It must be called by the client
within 5 seconds after establishing a connection; otherwise, the server will automatically close the
connection.

The response message contains basic information from the EVerest ChargePoint, such as the API version
in use, to enable further communication.
While the Hello call does not necessarily require a token, it may be called with one. If a token is
provided, the server verifies it, and the reply includes information about the token’s validity as well
as the associated user and permissions.

In case authentication is required, the optional parameter permission_scopes can be used to indicate
the permissions (e.g., read/write access) the client has when using the given token.

⚠️ Note: The fields authenticated, permission_scopes and everest_version are currently not supported.

**Request:**

.. code-block:: json

   {}

.. _example-json-rpc-request-without-params:

**Example JSON RPC Request:**

.. code-block:: json

   {"jsonrpc": "2.0", "method": "API.Hello", "id": 1}

**Response:**

.. code-block:: json

   {
     "authentication_required": "bool",
     "authenticated": "bool", // optional, always false for now
     "permission_scopes": "PermissionScopes", // optional, not yet defined
     "api_version": "string",
     "everest_version": "string", // currently not supported
     "charger_info": "$ChargerInfoObj"
   }

ChargePoint.GetEVSEInfos
^^^^^^^^^^^^^^^^^^^^^^^^
This method is used to obtain general information about all configured EVSE’s of the charge point.

**Request:**

.. code-block:: json

   {}

**Response:**
Returns an array of type “EVSEInfoObj” of all configured EVSE’s of the charge point.

.. code-block:: json

   {
     "infos": "[Array of $EVSEInfoObj]",
     "error": "$ResponseErrorEnum"
   }

ChargePoint.GetActiveErrors
^^^^^^^^^^^^^^^^^^^^^^^^^^^
This method returns a structured list of all currently active error conditions of the charger.
It is intended for diagnostic purposes and remote monitoring.

**Request:**

.. code-block:: json

   {}

**Response:**

.. code-block:: json

   {
     "active_errors": "[Array of $ErrorObj]", // Empty array if no errors
     "error": "$ResponseErrorEnum"
   }

EVSE.GetInfo
^^^^^^^^^^^^
This method is used to obtain general information about an EVSE.

**Request:**

.. code-block:: json

   {
     "evse_index": "int"
   }

.. _example-json-rpc-request-with-params:

**Example JSON RPC Request:**

.. code-block:: json

{"jsonrpc": "2.0", "method": "EVSE.GetInfo", "id": 1, "params": {"evse_index": 1}}


**Response:**

.. code-block:: json

   {
     "info": "$EVSEInfoObj",
     "error": "$ResponseErrorEnum"
   }

EVSE.GetStatus
^^^^^^^^^^^^^^
This method is used to obtain the current status of the EVSE.

**Request:**

.. code-block:: json

   {
     "evse_index": "int"
   }

**Response:**

.. code-block:: json

   {
     "status": "$EVSEStatusObj",
     "error": "$ResponseErrorEnum"
   }

EVSE.GetHardwareCapabilities
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This method is used to obtain hardware capabilities of the EVSE. Please note that the hardware capabilities
can be updated via notification EVSE.HardwareCapabilitiesChanged by the EVSE.

**Request:**

.. code-block:: json

   {
     "evse_index": "int"
   }

**Response:**

.. code-block:: json

   {
     "hardware_capabilities": "$HardwareCapabilitiesObj",
     "error": "$ResponseErrorEnum"
   }

EVSE.SetChargingAllowed
^^^^^^^^^^^^^^^^^^^^^^^
This method is used to explicitly allow charging on an EVSE or to remove the release.
Regardless of the authorisation status of the EV, this method can be used to delay a charging process
or to initiate a charging pause on EVSE's side.

**Request:**

.. code-block:: json

   {
     "evse_index": "int",
     "charging_allowed": "bool"
   }

**Response:**

.. code-block:: json

   {
     "error": "$ResponseErrorEnum"
   }

EVSE.GetMeterData
^^^^^^^^^^^^^^^^^
**Request:**

.. code-block:: json

   {
     "evse_index": "int"
   }

**Response:**

.. code-block:: json

   {
     "meter_data": "$MeterDataObj",
     "error": "$ResponseErrorEnum"
   }

EVSE.SetACChargingCurrent
^^^^^^^^^^^^^^^^^^^^^^^^^
This method is used to configure the AC charging current of an EVSE.

**Request:**

.. code-block:: json

   {
     "evse_index": "int",
     "max_current": "float"
   }

**Response:**
Returns an error parameter to show if the configuration of the charging current was successful.

.. code-block:: json

   {
     "error": "$ResponseErrorEnum"
   }

EVSE.SetACChargingPhaseCount
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
This method is used to configure the AC phase count of an EVSE.

**Request:**

.. code-block:: json

   {
     "evse_index": "int",
     "phase_count": "int"
   }

**Response:**
Returns an error parameter to show if the configuration of the charging current was successful.

.. code-block:: json

   {
     "error": "$ResponseErrorEnum"
   }

EVSE.SetDCChargingPower
^^^^^^^^^^^^^^^^^^^^^^^
This method is used to configure the DC charging power an EVSE.

**Request:**

.. code-block:: json

   {
     "evse_index": "int",
     "max_power": "float"
   }

**Response:**
Returns an error parameter to show if the configuration was successful.

.. code-block:: json

   {
     "error": "$ResponseErrorEnum"
   }

EVSE.EnableConnector
^^^^^^^^^^^^^^^^^^^^
Method to enable or disable a connector on the EVSE. connector_index is a positive integer identifying
the connector that should be enabled. If the connector_index is 0 the whole EVSE is enabled.

**Request:**

.. code-block:: json

   {
     "evse_index": "int",
     "connector_index": "int",
     "enable": "bool",
     "priority": "int"
   }

**Response:**

Returns an error parameter to show if the configuration of the charging current was successful.

.. code-block:: json

   {
     "error": "$ResponseErrorEnum"
   }

Notifications
~~~~~~~~~~~~~

Notifications are signaled by the server as soon as a property within the parameters has changed.

ChargePoint.ActiveErrorsChanged
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: json

   {
     "active_errors": "[Array of $ErrorObj]"
   }

EVSE.HardwareCapabilitiesChanged
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: json

   {
     "evse_index": "int",
     "hardware_capabilities": "$HardwareCapabilitiesObj"
   }

EVSE.StatusChanged
^^^^^^^^^^^^^^^^^^

.. code-block:: json

   {
     "evse_index": "int",
     "evse_status": "$EVSEStatusObj"
   }

EVSE.MeterDataChanged
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: json

   {
     "evse_index": "int",
     "meter_data": "$MeterDataObj"
   }

Enums
-----

ResponseErrorEnum
~~~~~~~~~~~~~~~~~
Enumeration to differentiate between the various error cases that can occur after a method request.

::

  "NoError",
  "ErrorInvalidParameter",
  "ErrorOutOfRange",
  "ErrorValuesNotApplied",
  "ErrorInvalidEVSEIndex",
  "ErrorInvalidConnectorId",
  "ErrorNoDataAvailable",
  "ErrorUnknownError"

EnergyTransferModeEnum
~~~~~~~~~~~~~~~~~~~~~~
Enumeration to differentiate between the various energy transfer modes

::

   "AC_single_phase_core",
   "AC_two_phase",
   "AC_three_phase_core",
   "DC_core",
   "DC_extended",
   "DC_combo_core",
   "DC_unique",
   "DC",
   "AC_BPT",
   "AC_BPT_DER",
   "AC_DER",
   "DC_BPT",
   "DC_ACDP",
   "DC_ACDP_BPT",
   "WPT"

ChargeProtocolEnum
~~~~~~~~~~~~~~~~~~

::

  "Unknown",
  "IEC61851",
  "DIN70121",
  "ISO15118",
  "ISO15118_20"

EVSEStateEnum
~~~~~~~~~~~~~

::

  "Unplugged",
  "Disabled",
  "Preparing",
  "Reserved",
  "AuthRequired",
  "WaitingForEnergy",
  "Charging",
  "ChargingPausedEV",
  "ChargingPausedEVSE",
  "Finished",
  "SwitchingPhases"

ConnectorTypeEnum
~~~~~~~~~~~~~~~~~

::

   "cCCS1",
   "cCCS2",
   "cG105",
   "cTesla",
   "cType1",
   "cType2",
   "s309_1P_16A",
   "s309_1P_32A",
   "s309_3P_16A",
   "s309_3P_32A",
   "sBS1361",
   "sCEE_7_7",
   "sType2",
   "sType3",
   "Other1PhMax16A",
   "Other1PhOver16A",
   "Other3Ph",
   "Pan",
   "wInductive",
   "wResonant",
   "Undetermined",
   "Unknown"

EVSEErrorEnum
~~~~~~~~~~~~~
EVSEErrorEnum can be used to show more details of an EVSE error for example in a service tool application
for the technician. The enum naming is identical to EVerest error handler semantic.

Example (excerpt)::

  "NoError",
  "power_supply_DC/HardwareFault",
  "power_supply_DC/OverTemperature",
  "power_supply_DC/UnderTemperature",
  "power_supply_DC/UnderVoltageAC",
  "power_supply_DC/OverVoltageAC",
  "power_supply_DC/UnderVoltageDC",
  "power_supply_DC/OverVoltageDC",
  "power_supply_DC/OverCurrentAC",
  "power_supply_DC/OverCurrentDC",
  "power_supply_DC/VendorError",
  "power_supply_DC/VendorWarning",
  "evse_board_support/MREC2GroundFailure",
  "evse_board_support/MREC3HighTemperature",
  "evse_board_support/MREC4OverCurrentFailure",
  "evse_board_support/MREC5OverVoltage",
  "evse_board_support/MREC6UnderVoltage",
  "evse_board_support/MREC8EmergencyStop",
  "evse_board_support/MREC10InvalidVehicleMode",
  "evse_board_support/MREC14PilotFault",
  "evse_board_support/MREC15PowerLoss",
  "evse_board_support/MREC17EVSEContactorFault",
  "evse_board_support/MREC18CableOverTempDerate",
  "evse_board_support/MREC19CableOverTempStop",
  "evse_board_support/MREC20PartialInsertion",
  "evse_board_support/MREC23ProximityFault",
  "evse_board_support/MREC24ConnectorVoltageHigh",
  "evse_board_support/MREC25BrokenLatch",
  "evse_board_support/MREC26CutCable",
  ...

JSON Objects
------------

EVSEInfoObj
~~~~~~~~~~~
This object contains static information about a EVSE of a charge point. This parameter is derived from
the EvseManager identifier from the EVerest configuration. The "index"  parameter is essential to perform
EVSE specific method calls. The “id” parameter is the EVSE ID. The EVSE ID is a globally unique identifier
defined in ISO 15118 to represent a specific EVSE. The “supported_energy_transfer_modes”  must be used to
distinguish between DC and AC charging. Depending on this, the optional parameters of object “EVSEStatusObj”
are configured. In addition, it is possible to determine whether BPT is supported.

.. code-block:: json

   {
     "index": "int",
     "id": "string",
     "description": "string", // optional
     "available_connectors": "[ConnectorInfoObj]",
     "supported_energy_transfer_modes": "[EnergyTransferModeEnum]"
   }

EVSEStatusObj
~~~~~~~~~~~~~
This object contains all information about the current status of a charge point EVSE. These parameters
change dynamically, depending on the current EVSE state, which is indicated by the “state” parameter.
The parameters “ac_charge_param" and “ac_charge_status" are only configured in a AC charging session
and parameters “dc_charge_param" and “dc_charge_status" are only configured in a DC charging session.
These parameters mainly contain parameters that are transmitted in an HLC session. The connector info
(e.g. to identify if it is a DC or AC charger) is part of object “EVSEInfoObj“.  The “active_connector_index”
information can also be used by GUI applications to display the active connector correctly.

.. code-block:: json

   {
     "charged_energy_wh": "float",
     "discharged_energy_wh": "float",
     "charging_duration_s": "int",
     "charging_allowed": "bool",
     "available": "bool",
     "active_connector_index": "int",
     "error_present": "bool",
     "charge_protocol": "$ChargeProtocolEnum",
     "ac_charge_param": "$ACChargeParametersObj", // optional, only if AC supported
     "dc_charge_param": "$DCChargeParametersObj", // optional, only if DC supported
     "ac_charge_status": "$ACChargeStatusObj", // optional, only if AC supported
     "dc_charge_status": "$DCChargeStatusObj", // optional, only if DC supported
     "display_parameters": "$DisplayParametersObj",
     "state": "$EVSEStateEnum"
   }

ConnectorInfoObj
~~~~~~~~~~~~~~~~
This object contains static information about a connector of an EVSE. This parameter is derived from
the from the EVerest configuration. The "index"  parameter is essential to perform connector specific
method calls. The “type”  must be used to distinguish between DC and AC charging. Depending on this,
the optional parameters of object “EVSEStatusObj” are configured.

.. code-block:: json

   {
     "index": "int",
     "type": "ConnectorTypeEnum",
     "description": "string" // optional
   }

HardwareCapabilitiesObj
~~~~~~~~~~~~~~~~~~~~~~~
This object contains all hardware related limits of a charge point EVSE.

.. code-block:: json

   {
     "max_current_A_export": "float",
     "max_current_A_import": "float",
     "max_phase_count_export": "int",
     "max_phase_count_import": "int",
     "min_current_A_export": "float",
     "min_current_A_import": "float",
     "min_phase_count_export": "int",
     "min_phase_count_import": "int",
     "phase_switch_during_charging": "bool"
   }

MeterDataObj
~~~~~~~~~~~~
This object contains the following meter data of a charge point EVSE:

timestamp: Timestamp of measurement, represented as RFC3339 string
energy_Wh_import: Imported energy in Wh (from grid)
meter_id: A (user defined) meter if (e.g. id printed on the case)
serial_number: Serial number of the meter
phase_seq_error: AC only: true for 3 phase rotation error (ccw)
energy_Wh_export:  Exported energy in Wh (to grid)
power_W: Instantaneous power in Watt. Negative values are exported, positive values imported Energy.
voltage_V: Voltage in Volts
current_A: Current in Ampere
frequency_Hz:  Grid frequency in Hertz

.. code-block:: json

   {
     "current_A": {"L1": "float","L2": "float","L3": "float","N": "float"},
     "energy_Wh_import": {"L1": "float","L2": "float","L3": "float","total": "float"},
     "energy_Wh_export": {"L1": "float","L2": "float","L3": "float","total": "float"}, // optional
     "frequency_Hz": {"L1": "float","L2": "float","L3": "float"}, // optional
     "meter_id": "string",
     "serial_number": "string", // optional
     "phase_seq_error": "bool", // optional
     "power_W": {"L1": "float","L2": "float","L3": "float","total": "float"},  // optional
     "timestamp": "string",
     "voltage_V": {"L1": "float","L2": "float","L3": "float"}  // optional
   }

ACChargeParametersObj
~~~~~~~~~~~~~~~~~~~~~
This object contains all AC related parameters of a charge point EVSE. Parameters like “evse_maximum_discharge_power”
are only transmitted if a BPT (bidirectional power transfer) session is active. Currently only “evse_max_current”
and “evse_max_phase_count“ are supported.

.. code-block:: json

   {
     "evse_nominal_voltage": "float",
     "evse_max_current": "float",
     "evse_max_phase_count": "int",
     "evse_maximum_charge_power": "float",
     "evse_minimum_charge_power": "float",
     "evse_nominal_frequency": "float",
     "evse_maximum_discharge_power": "float",
     "evse_minimum_discharge_power": "float"
   }

DCChargeParametersObj
~~~~~~~~~~~~~~~~~~~~~
Currently not supported.

This object contains all DC related parameters of a charge point EVSE. Parameters like “evse_maximum_discharge_power”
are only transmitted if a BPT (bidirectional power transfer) session is active.

.. code-block:: json

   {
     "evse_maximum_charge_current": "float",
     "evse_maximum_charge_power": "float",
     "evse_maximum_voltage": "float",
     "evse_minimum_charge_current": "float",
     "evse_minimum_charge_power": "float",
     "evse_minimum_voltage": "float",
     "evse_energy_to_be_delivered": "float",
     "evse_maximum_discharge_current": "float",
     "evse_maximum_discharge_power": "float",
     "evse_minimum_discharge_current": "float",
     "evse_minimum_discharge_power": "float"
   }

ACChargeStatusObj
~~~~~~~~~~~~~~~~~
This object contains all DC related parameters of a charge point EVSE. Parameters like “evse_maximum_discharge_power”
are only transmitted if a BPT (bidirectional power transfer) session is active. Currently only “evse_max_current”
and “evse_max_phase_count“ are supported.

.. code-block:: json

   {
     "evse_active_phase_count": "int"
   }

DCChargeStatusObj
~~~~~~~~~~~~~~~~~
Currently not supported.
This object contains all DC related parameters during charging of a charge point EVSE. 

.. code-block:: json

   {
     "evse_present_current": "float",
     "evse_present_voltage": "float",
     "evse_power_limit_achieved": "bool",
     "evse_current_limit_achieved": "bool",
     "evse_voltage_limit_achieved": "bool"
   }

DisplayParametersObj
~~~~~~~~~~~~~~~~~~~~
Currently not supported.

This object contains additional information which can be displayed in a GUI. These parameters are for
display purposes only and must not, under any circumstances, influence the EVSE behavior. Most of the
parameters are only transmitted in an ISO15118-20 charging session.

.. code-block:: json

   {
     "start_soc": "int",
     "present_soc": "int",
     "minimum_soc": "int",
     "target_soc": "int",
     "maximum_soc": "int",
     "remaining_time_to_minimum_soc": "int",
     "remaining_time_to_target_soc": "int",
     "remaining_time_to_maximum_soc": "int",
     "charging_complete": "bool",
     "battery_energy_capacity": "float",
     "inlet_hot": "bool"
   }

ChargerInfoObj
~~~~~~~~~~~~~~
This object contains well-known general charger information, e.g. vendor and model name, firmware version etc.

.. code-block:: json

   {
     "vendor": "string",
     "model": "string",
     "serial": "string",
     "friendly_name": "string",
     "manufacturer": "string",
     "manufacturer_url": "string",
     "model_url": "string",
     "model_no": "string",
     "revision": "string",
     "board_revision": "string",
     "firmware_version": "string"
   }

ErrorObj
~~~~~~~~
The ErrorObj structure represents a detailed description of an charger error. It includes the error
type, origin, severity, and timestamp, along with optional context like EVSE or connector index. Each
error is uniquely identified by a UUID and may include a vendor-specific ID and custom message.

.. code-block:: json

   {
     "type": "string",
     "sub_type": "string",
     "message": "string",
     "description": "string",
     "origin": {
        "module_id": "string",
        "implementation_id": "string",
        "evse_index": "int", // optional
        "connector_index": "int" // optional
     },
     "vendor_id": "string",
     "severity": "SeverityEnum",
     "timestamp": "string",
     "uuid": "string"
   }

What can a simple sequence look like?
-------------------------------------
The sequence diagram below is a simple sequence diagram based on the defined WebSocket methods and notifications.
The diagram is simplified for better visualization and therefore only shows the relevant parameters within
the objects.

Initialization
~~~~~~~~~~~~~~
The first diagram illustrates how a client establishes a connection to the server and how the server
initializes a single EVSE. After that, the client application is prepared to process incoming notifications
of the API, for example caused by plugging in an EV.

.. image:: img/initialization.drawio.svg
   :alt: RPC Communication Flow
   :align: center
   :width: 80%

Session handling
~~~~~~~~~~~~~~~~
The second sequence diagram shows the notifications that are triggered as soon as an EV is plugged in
and recognized by the EVSE. It also shows how the client can actively adjust the charging current of a
running session.

.. image:: img/ac_session_handling.drawio.svg
   :alt: RPC Communication Flow
   :align: center
   :width: 80%
