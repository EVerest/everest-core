# Getting Started with OCPP2.0.1

## Integrate this library with your Charging Station Implementation for OCPP2.0.1

OCPP is a protocol that affects, controls and monitors many areas of a charging station's operation.

If you want to integrate this library with your charging station implementation, you have to register a couple of **callbacks** and integrate **event handlers**. This is necessary for the library to interact with your charging station according to the requirements of OCPP.

Libocpp needs registered **callbacks** in order to execute control commands defined within OCPP (e.g ResetRequest or RemoteStartTransactionRequest)

The implementation must call **event handlers** of libocpp so that the library can track the state of the charging station and trigger OCPP messages accordingly (e.g. MeterValuesRequest , StatusNotificationRequest)

Your reference within libocpp to interact is a single instance to the class ocpp::v201::ChargePoint defined in `v201/charge_point.hpp` for OCPP 2.0.1.
