.. _everest_modules_handwritten_EvManager:

=====
EvManager
=====

This Module implements the car simulator for a charging session.

Configuration
_____________

``connector_id``
    The connector id of the EVSE Manager
    to which the simulator connects to.

External MQTT
-------------

The module listens to the following MQTT topics:

``everest_external/nodered/{connector_id}/carsim/cmd/enable``
    | Used to enable the car simulator.
    | Possible values are:

    - ``true``
    - ``false``

``everest_external/nodered/{connector_id}/carsim/cmd/execute_charging_session``
    | Used to execute a charging session based on the semicolon separated provided command string.
    ::

        "sleep 1;iso_wait_slac_matched;iso_start_v2g_session DC_extended;iso_wait_pwr_ready;sleep 36000"

    | (For all available commands see: `Simulator Commands`_)

``everest_external/nodered/{connector_id}/carsim/cmd/modify_charging_session``
    | Used to modify the current charging session.
    | Follows the same format as ``execute_charging_session``.

Simulator Commands
------------------
``sleep {time in seconds}``
    | Sleeps for the specified time.
    | Example: ``sleep 10``
``test``
