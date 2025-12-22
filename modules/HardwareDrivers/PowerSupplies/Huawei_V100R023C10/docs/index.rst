.. _everest_modules_handwritten_Huawei_V100R023C10:

######################
Huawei V100R023C10 PSU
######################

Voltage measurements
====================

The Huawei V100R023C10 does not provide voltage measurements, instead it needs an external voltage 
measurement device that measures the "upstream" voltage (meaning directly after the PSU, before any relay).
Also, Everest needs a voltage and current measurement regularly.

For the upstream voltage two options are available which (see `upstream_voltage_source` config option):

- Using an isolation monitoring device (``IMD``)
- Using an overvoltage monitoring device (``OVM``)

For the everest measurements two options are available:

- None (not recommended, needs ``HACK_publish_requested_voltage_current`` to work properly)
- Using a carside powermeter (ideally the powermeter that is connected to the EvseManager's ``powermeter_car_side``)
- Using a carside powermeter but during cable check using an ``OVM`` (see ``HACK_use_ovm_while_cable_check`` config option)

Telemetry
=========

The module can publish telemetry data on a specified mqtt base topic, set via the config option ``telemetry_topic_prefix``.
The concrete telemetry data is published only when the data changes to reduce mqtt traffic.

The data published looks like this (example for base topic ``base_topic``):

``base_topic/connector/1``

.. code-block:: json

    {
      "max_rated_psu_current": 100.0,
      "max_rated_psu_voltage": 1000.0,
      "min_rated_psu_current": 1.0,
      "min_rated_psu_voltage": 100.0,
      "psu_port_available": "AVAILABLE",
      "rated_output_power_psu": 60000.0
    }

``base_topic/connector/1/dispenser_to_psu``

.. code-block:: json

    {
      "bsp_event": "PowerOn",
      "dc_output_contactor_fault_alarm": false,
      "everest_mode": "Export",
      "everest_phase": "Charging",
      "export_current": 20.0,
      "export_voltage": 400.0,
      "output_current": 0.0,
      "output_voltage": 0.0,
      "upstream_voltage": 0.0
    }

``base_topic/psu``

.. code-block:: json

    {
      "ac_input_current_a": 10.0,
      "ac_input_current_b": 10.5,
      "ac_input_current_c": 9.5,
      "ac_input_voltage_a": 230.0,
      "ac_input_voltage_b": 231.0,
      "ac_input_voltage_c": 229.0,
      "psu_running_mode": "RUNNING",
      "total_historic_input_energy": 100000.0
    }

``base_topic/dispenser/published_alarms``

.. code-block:: json

    {
      "door_status_alarm": false,
      "epo_alarm": false,
      "tilt_alarm": false,
      "water_alarm": false
    }

The units are SI units (Amps, Volts, Watts, Watt-hours).

.. note::

    All telemetry values can be null, indicating that no value has been received or sent yet.

BSP Errors
==========

This driver supports setting specific errors to the Power supply unit as Dispenser and Connector Alarms as a reaction to EVerest BSP errors:

+-------------------------------------------------+---------------------------+---------------+
|                Everest BSP Error                | PSU Modbus Register name  |     Scope     |
+=================================================+===========================+===============+
| ``evse_board_support/EnclosureOpen``            | Door status alarm         | Dispenser     |
+-------------------------------------------------+---------------------------+---------------+
| ``evse_board_support/WaterIngressDetected``     | Water alarm               | Dispenser     |
+-------------------------------------------------+---------------------------+---------------+
| ``evse_board_support/MREC8EmergencyStop``       | EPO alarm                 | Dispenser     |
+-------------------------------------------------+---------------------------+---------------+
| ``evse_board_support/TiltDetected``             | Tilt alarm                | Dispenser     |
+-------------------------------------------------+---------------------------+---------------+
| ``evse_board_support/MREC17EVSEContactorFault`` | DC output contactor fault | Per Connector |
+-------------------------------------------------+---------------------------+---------------+

The connector alarms are published 1:1 to the connectors (if the BSP for connector 1 has the error, connector 1 gets the alarm, etc).

For the dispenser alarms, if any of the BSPs has the error, the alarm is published to the dispenser. If all BSPs clear the error, the alarm is cleared.

Power Supply Mock
==================

The mock is a single executable that simulates the communication behaviour of a Huawei V100R023C10 power supply.
It is used to test the software stack without needing the actual hardware.

It opens a socket on port 8502 to accept connections from the everest module and receives and answers goose messages.

The mock is built together with the everest module, but can also be build separately if needed.
The mock is not installed by default but can be if ``INSTALL_FUSION_CHARGER_MOCK`` is set to ``ON`` in cmake.

Note that the mock uses a constant hmac key instead of generating a new one for each charge session.

Build separately from module
----------------------------

.. code-block:: bash

    cd modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/fusion_charger_lib
    mkdir build
    cd build
    cmake ..
    make -j$(nproc)

Binary is located in:

.. code-block:: bash

    modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/fusion_charger_lib/build/fusion-charger-dispenser-library/power_stack_mock/fusion_charger_mock

Mock options
------------

The mock has a few environment variables (enable or disable by setting them to `1`/`true` or `0`/`false`):

- ``FUSION_CHARGER_MOCK_DISABLE_SEND_HMAC``: If set the mock will disable securing the goose messages with an hmac.
  They are still sent, just not secured.
- ``FUSION_CHARGER_MOCK_DISABLE_VERIFY_HMAC``: If set the mock will disable verifying the hmac of the received goose
  messages. This also allows to receive completely unsigned messages.
- ``FUSION_CHARGER_MOCK_ETH``: The ethernet interface to use for receiving and sending goose messages. Defaults to
  ``veth0``.

It also has one optional command line argument, being the path to a folder with certificates and keys for mTLS.

Mock mTLS
---------

The mock can be run with mTLS enabled. For this, one needs to create a folder with the following files:

- ``dispenser_ca.crt.pem``: The CA certificate used to sign the dispensers' certificates.
- ``psu.crt.pem``: The certificate used by the mock to identify itself as a PSU.
- ``psu.key.pem``: The private key of the PSU certificate.

These files can be generated with dummy values using the script located here (Note that this also generates the 
corresponding files for the dispenser): 

.. code-block:: bash

    modules/HardwareDrivers/PowerSupplies/Huawei_V100R023C10/fusion_charger_lib/fusion-charger-dispenser-library/user-acceptance-tests/test_certificates/generate.sh