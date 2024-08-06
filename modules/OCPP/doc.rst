Global Errors and Error Reporting
=================================

The `enable_global_errors` flag for this module is enabled. This module is therefore able to retrieve and process all reported errors
from other modules loaded in the same EVerest configuration.

In OCPP1.6 errors and can be reported using the `StatusNotification.req` message. If this module gets notified about a raised error,
it initiates a `StatusNotification.req` that contains information about the error that has been raised.

The field `status` of the `StatusNotification.req` will be set to faulted only in case the error is of the special type `evse_manager/Inoperative`.
The field `connectorId` is set based on the mapping (for evse id and connector id) of the origin of the Error. 
If no mapping is provided, the error will be reported on connectorId 0. Note that the mapping can be configured per module 
inside the EVerest config file. 
The field `errorCode` is set based in the `type` property of the error.

The fields `info`, `vendorId` and `vendorErrorCode` are set based on the error type and the provided error properties. Please see the definiton
of `get_error_info` to see how the `StatusNotification.req` is constructed based on the given error.

The `StatusNotification.req` message has some limitations with respect to reporting errors:
* Single errors can not simply be cleared. If multiple errors are raised it is not possible to clear individual errors
* Some fields of the message have relatively small character limits (e.g. `info` with 50 characters, `vendorErrorCode` with 50 characters)

This module attempts to follow the Minimum Required Error Codes (MRECS): https://inl.gov/chargex/mrec/ . This proposes a unified methodology 
to define and classify a minimum required set of error codes and how to report them via OCPP1.6.

This module currently deviates from the MREC specification in the following points:
* Simultaneous errors: MREC requires to report simultaneous errors by reporting them in a single `StatusNotification.req` by separating the 
information of the fields `vendorId`and `info` by a semicolon. This module sends one `StatusNotifcation.req` per individual errors because
of the limited maximum characters of the `info` field.
* MREC requires to always use the value `Faulted` for the `status` field when reporting an error. The OCPP1.6 specification defines the 
`Faulted` value as follows: "When a Charge Point or connector has reported an error and is not available for energy delivery . (Inoperative).".
This module therefore only reports `Faulted` when the Charge Point is not available for energy delivery.

Interaction with EVSE Manager
=============================

This module sets callbacks into libocpp to receive `ChangeAvailability.req` updates from the CSMS.

These are sent to the EVSE Manager in `enable_disable` commands with a priority of 5000. ('types/energy_manager.yaml' contains the valid range.)

5000 is mid-range.
