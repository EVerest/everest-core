Error Handling
==============

The `enable_global_errors` flag for this module is true. This module is
therefore able to retrieve and process all reported errors from other
modules that are loaded in the same EVerest configuration.

The error reporting via OCPP2.0.1 follows the Minimum Required Error Codes (MRECS): https://inl.gov/chargex/mrec/ . This proposes a unified methodology 
to define and classify a minimum required set of error codes and how to report them via OCPP2.0.1.

StatusNotification
------------------
In contrast to OCPP1.6, error information is not transmitted as part of the StatusNotification.req. 
A `StatusNotification.req` with status `Faulted` will be set to faulted only in case the error received is of the special type `evse_manager/Inoperative`.
This indicates that the EVSE is inoperative (not ready for energy transfer).

In OCPP2.0.1 errors can be reported using the `NotifyEventRequest.req` . This message is used to report all other errros received.  

Current Limitation
------------------

In OCPP2.0.1 errors can be reported using the `NotifyEventRequest`
message. The `eventData` property carries the relevant information.

This format of reporting errors deviates from the mechanism used within
EVerest. This data structure forces to map an error to a
component-variable combination. This requires a mapping
mechanism between EVerest errors and component-variable
combination.

Currently this module maps the Error to one of these three Components:

* ChargingStation (if error.origin.mapping.evse is not set or 0)
* EVSE (error.origin.mapping.evse is set and error.origin.mapping.connector is not set)
* Connector (error.origin.mapping.evse is set and error.origin.mapping.connector is set)

The Variable used as part of the NotifyEventRequest is constantly defined to `Problem` for now.

The goal is to have a more advanced mapping of reported errors to the respective component-variable combinations in the future.

Energy Management and Smart Charging Integration
================================================

OCPP2.0.1 defines the SmartCharging feature profile to allow the CSMS to control or influence the power consumption of the charging station. 
This module integrates the composite schedule(s) within EVerests energy management. For further information about smart charging and the
composite schedule calculation please refer to the OCPP2.0.1 specification.

The integration of the composite schedules are implemented through the optional requirement(s) `evse_energy_sink`(interface: `external_energy_limits`) 
of this module. Depending on the number of EVSEs configured, each composite limit is communicated via a seperate sink, including the composite schedule
for EVSE with id 0 (representing the whole charging station). The easiest way to explain this is with an example. If your charging station
has two EVSEs you need to connect three modules that implement the `external_energy_limits` interface: One representing evse id 0 and 
two representing your actual EVSEs.

📌 **Note:** You have to configure an evse mapping for each module connected via the evse_energy_sink connection. This allows the module to identify
which requirement to use when communication the limits for the EVSEs. For more information about the module mapping please see 
`3-tier module mappings <https://everest.github.io/nightly/general/05_existing_modules.html#tier-module-mappings>`_.

This module defines a callback that gets executed every time charging profiles are changed, added or removed by the CSMS. The callback retrieves
the composite schedules for all EVSEs (including evse id 0) and calls the `set_external_limits` command of the respective requirement that implements
the `external_energy_limits` interface. In addition, the config parameter `CompositeScheduleIntervalS` defines a periodic interval to retrieve
the composite schedule also in case no charging profiles have been changed. The configuration paramater `RequestCompositeScheduleDurationS` defines 
the duration in seconds of the requested composite schedules starting now. The value configured for `RequestCompositeScheduleDurationS` shall be greater
than the value configured for `CompositeScheduleIntervalS` because otherwise time periods could be missed by the application.