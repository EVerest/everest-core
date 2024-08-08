Global Errors
=============

The `enable_global_errors` flag for this module is true. This module is
therefore able to retrieve and process all reported errors from other
modules loaded in the same EVerest configuration.

In OCPP2.0.1 errors can be reported using the `NotifyEventRequest`
message. The `eventData` property carries the relevant information
about the error and is defined as follows:
* eventId: integer
Identifies the event. This field can be referred to as a cause by other
events.
* timestamp: string
Timestamp of the moment the report was generated.
* trigger: EventTriggerEnumType
Type of monitor that triggered this event. Defined by EventTriggerEnumType.
* cause: integer
Refers to the ID of an event that is considered to be the cause for this event.
* actualValue: string
Actual value of the variable. Maximum length is 2500 characters.
* techCode: string
Technical (error) code as reported by component. Maximum length is 50
characters.
* techInfo: string
Technical detail information as reported by component. Maximum length is 500
characters.
* cleared: boolean
Indicates if the monitored situation has been cleared (true for a return to
normal).
* transactionId: string
If an event notification is linked to a specific transaction, this field can
specify its transaction ID. Maximum length is 36 characters.
* component: ComponentType
Component object. Defined by ComponentType.
* variableMonitoringId: integer
Identifies the VariableMonitoring which triggered the event.
* eventNotificationType: EventNotificationEnumType
Event notification type. Defined by EventNotificationEnumType.
* variable: VariableType
Variable object. Defined by VariableType.

Required Properties
* eventId
* timestamp
* trigger
* actualValue
* eventNotificationType
* component
* variable

This format of reporting errors deviates from the mechanism used within
EVerest. This data structure forces to map an error to a
component-variable combination. This requires a mapping
mechanism between EVerest errors and component-variable
combination.
