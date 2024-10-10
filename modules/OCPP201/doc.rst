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

