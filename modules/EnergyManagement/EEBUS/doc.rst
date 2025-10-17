.. _everest_modules_handwritten_EEBUS:

*******************************************
EEBUS
*******************************************

:ref:`Link <everest_modules_EEBUS>` to the module's reference.

This document describes the EVerest EEBUS module. This module acts as a bridge between the EVerest framework and an external EEBUS gRPC service. It implements the "Limitation of Power consumption" (LPC) use case.

Architecture
============

Below is a diagram showing the architecture of the EEBUS module and its interaction with other components. You can render this diagram using a PlantUML renderer.

.. code-block:: plantuml

   @startuml
   !include https://raw.githubusercontent.com/plantuml-stdlib/C4-PlantUML/master/C4_Container.puml

   title Architecture Diagram for EEBUS Module

   Container(energy_manager, "Energy Manager", "EVerest Module", "Manages energy distribution")
   Container(eebus_module, "EEBUS Module", "EVerest C++ Module", "Bridge to EEBUS service for LPC")
   System_Ext(eebus_grpc_api, "eebus-grpc-api", "External Go binary, gRPC server/client")
   System_Ext(eebus_service, "EEBUS Service (HEMS)", "External System", "Implements EEBUS standard, e.g. LPC")

   Rel(eebus_module, energy_manager, "Publishes ExternalLimits", "everest-interface")
   Rel(eebus_module, eebus_grpc_api, "Connects and receives events", "gRPC")
   Rel(eebus_grpc_api, eebus_service, "Communicates", "EEBUS (SHIP)")

   @enduml

How it works
============

The module connects to an external EEBUS gRPC service and can manage its lifecycle. It registers itself to the control service and adds the LPC (Limitation of Power Consumption) use case. It then subscribes to events from the use case.

The ``LpcUseCaseHandler`` class manages the state of the LPC use case. The states are:

- ``Init``: Initial state.
- ``UnlimitedControlled``: The charging is not limited, but it is controlled by the EEBUS service.
- ``Limited``: The charging power is limited by the EEBUS service.
- ``Failsafe``: A failsafe limit is applied. This happens when the heartbeat from the EEBUS service is missed.
- ``UnlimitedAutonomous``: The charging is not limited and not controlled by the EEBUS service.

The handler processes the following events:

- ``DataUpdateHeartbeat``: A heartbeat from the EEBUS service. If it's missing for a certain time, the handler goes into ``Failsafe`` state.
- ``DataUpdateLimit``: A new power limit from the EEBUS service.
- ``WriteApprovalRequired``: The handler needs to approve pending writes.

Based on its state and the received limits, the module publishes ``ExternalLimits`` to the ``eebus_energy_sink``, which is typically connected to an ``EnergyManager`` module.

State Machine Diagram
=====================

The following diagram shows the state machine of the `LpcUseCaseHandler`.

.. code-block:: plantuml

   @startuml
   title State Machine for LpcUseCaseHandler

   state "Init" as Init
   state "Unlimited / Controlled" as UnlimitedControlled
   state "Limited" as Limited
   state "Failsafe" as Failsafe
   state "Unlimited / Autonomous" as UnlimitedAutonomous

   [*] --> Init

   Init --> UnlimitedControlled : on DataUpdateLimit (not active)
   Init --> Limited : on DataUpdateLimit (active)
   Init --> UnlimitedAutonomous : on Timeout (120s)

   UnlimitedControlled --> Limited : on DataUpdateLimit (active)
   Limited --> UnlimitedControlled : on DataUpdateLimit (not active)

   UnlimitedAutonomous --> UnlimitedControlled : on DataUpdateHeartbeat
   UnlimitedAutonomous --> Limited : on DataUpdateLimit (active)

   Failsafe --> UnlimitedControlled : on DataUpdateHeartbeat

   note "From any state except Failsafe" as N1
   N1 ..> Failsafe : on Heartbeat timeout

   Init .. N1
   UnlimitedControlled .. N1
   Limited .. N1
   UnlimitedAutonomous .. N1

   @enduml

Code Flow Diagram
=================

This sequence diagram illustrates the code flow when an event is received from the EEBUS service.

.. code-block:: plantuml

   @startuml
   title Code Flow Diagram

   participant EebusConnectionHandler
   participant LpcUseCaseHandler
   participant UseCaseEventReader
   participant "gRPC Service" as GrpcService

   EebusConnectionHandler -> LpcUseCaseHandler : create
   EebusConnectionHandler -> LpcUseCaseHandler : start()
   activate LpcUseCaseHandler

   EebusConnectionHandler -> UseCaseEventReader : create
   EebusConnectionHandler -> UseCaseEventReader : start()
   activate UseCaseEventReader

   UseCaseEventReader -> GrpcService : SubscribeUseCaseEvents()

   ... event arrives ...

   GrpcService -> UseCaseEventReader : OnReadDone()
   UseCaseEventReader -> LpcUseCaseHandler : handle_event(event)

   LpcUseCaseHandler -> LpcUseCaseHandler : push event to queue & notify
   deactivate UseCaseEventReader

   LpcUseCaseHandler -> LpcUseCaseHandler : run() loop wakes up
   LpcUseCaseHandler -> LpcUseCaseHandler : process event, set_state()
   LpcUseCaseHandler -> LpcUseCaseHandler : apply_limit_for_current_state()
   LpcUseCaseHandler -> EEBUS : callbacks.update_limits_callback()
   deactivate LpcUseCaseHandler

   @enduml

Class Diagram
=============

This diagram shows the main classes within the EEBUS module and their relationships.

.. code-block:: plantuml

   @startuml
   title Class Diagram for EEBUS Module

   class EEBUS {
     + p_main: unique_ptr<emptyImplBase>
     + r_eebus_energy_sink: unique_ptr<external_energy_limitsIntf>
     + config: Conf
     - eebus_grpc_api_thread: thread
     - connection_handler: unique_ptr<EebusConnectionHandler>
     - callbacks: eebus::EEBusCallbacks
     + init()
     + ready()
   }

   class EebusConnectionHandler {
     - config: shared_ptr<ConfigValidator>
     - lpc_handler: unique_ptr<LpcUseCaseHandler>
     - event_reader: unique_ptr<UseCaseEventReader>
     - control_service_stub: shared_ptr<control_service::ControlService::Stub>
     + initialize_connection()
     + start_service()
     + add_lpc_use_case()
     + subscribe_to_events()
     + stop()
   }

   class LpcUseCaseHandler {
     - config: shared_ptr<ConfigValidator>
     - callbacks: eebus::EEBusCallbacks
     - stub: shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub>
     - state: State
     - event_queue: queue<UseCaseEvent>
     + start()
     + stop()
     + handle_event()
     - run()
     - set_state()
     - apply_limit_for_current_state()
   }

   class UseCaseEventReader {
     - stub: shared_ptr<control_service::ControlService::Stub>
     - event_callback: function
     + start()
     + stop()
     + OnReadDone()
   }

   class ConfigValidator {
     - config: Conf
     + validate()
   }

   struct Conf {
     // ... fields
   }

   EEBUS o-- EebusConnectionHandler
   EEBUS ..> Conf
   EebusConnectionHandler o-- LpcUseCaseHandler
   EebusConnectionHandler o-- UseCaseEventReader
   EebusConnectionHandler *-- ConfigValidator
   LpcUseCaseHandler *-- ConfigValidator
   LpcUseCaseHandler ..> eebus.EEBusCallbacks
   UseCaseEventReader ..> LpcUseCaseHandler : event_callback

   @enduml

Configuration
=============

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Key
     - Description
   * - ``manage_eebus_grpc_api_binary``
     - (boolean) Whether the module should manage the eebus grpc api binary. Default: ``true``
   * - ``eebus_service_port``
     - (integer) Port for the control service, this will be sent in the SetConfig call. Default: ``4715``
   * - ``grpc_port``
     - (integer) Port for grpc control service connection. This is the port on which we will create our control service channel and start the grpc binary with. Default: ``50051``
   * - ``eebus_ems_ski``
     - (string, required) EEBUS EMS SKI.
   * - ``certificate_path``
     - (string) Path to the certificate file used by eebus go client. If relative will be prefixed with everest prefix + etc/everest/certs. Otherwise absolute file path is used. Default: ``ebus/evse_cert``
   * - ``private_key_path``
     - (string) Path to the private key file used by eebus go client. If relative will be prefixed with everest prefix + etc/everest/certs. Otherwise absolute file path is used. Default: ``eebus/evse_key``
   * - ``eebus_grpc_api_binary_path``
     - (string) Path to the eebus grpc api binary. If relative will be prefixed with everest prefix + libexec. Otherwise absolute file path is used. Default: ``eebus_grpc_api``
   * - ``vendor_code``
     - (string, required) Vendor code for the configuration of the control service.
   * - ``device_brand``
     - (string, required) Device brand for the configuration of the control service.
   * - ``device_model``
     - (string, required) Device model for the configuration of the control service.
   * - ``serial_number``
     - (string, required) Serial number for the configuration of the control service.
   * - ``failsafe_control_limit``
     - (integer) Failsafe control limit for LPC use case. This will also be used for the default consumption limit, unit is Watts. Default: ``4200``
   * - ``max_nominal_power``
     - (integer) Maximum nominal power of the charging station. This is the max power the CS can consume. Default: ``32000``

Provided and required interfaces
================================

- Provides ``main`` (``empty`` interface).
- Requires ``eebus_energy_sink`` (``external_energy_limits`` interface). This is used to publish the calculated energy limits.

Acknowledgment
==============

This module has thankfully received support from the German Federal Ministry
for Economic Affairs and Climate Action.
Information on the corresponding research project can be found here (in
German only):
`InterBDL research project <https://www.thu.de/de/org/iea/smartgrids/Seiten/InterBDL.aspx>`_

.. image:: https://raw.githubusercontent.com/EVerest/EVerest/main/docs/img/bmwk-logo-incl-supporting.png
    :name: bmwk-logo
    :align: left
    :alt: Supported by Federal Ministry for Economic Affairs and Climate Action.