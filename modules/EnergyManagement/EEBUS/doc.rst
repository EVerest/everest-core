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

The module's main class is ``EEBUS``. Its ``init()`` method orchestrates the setup.

1.  **Configuration Validation**: It starts by using ``ConfigValidator`` to check the module's configuration provided in the EVerest ``config.yaml``. This includes validating ports, paths to binaries and certificates, and other parameters.

2.  **gRPC Binary Management**: If ``manage_eebus_grpc_api_binary`` is enabled in the config, the module starts the external ``eebus_grpc_api`` Go binary in a separate thread. This binary acts as a gRPC server for the EVerest module and a client to the actual EEBUS service (e.g., a HEMS).

3.  **Connection Handling**: The ``EebusConnectionHandler`` class is responsible for all gRPC communication. It connects to the ``eebus_grpc_api`` service, sends the device configuration (vendor, brand, model, etc.), and registers the "Limitation of Power Consumption" (LPC) use case.

4.  **Use Case Logic**: For the LPC use case, an ``LpcUseCaseHandler`` is created. This class implements the LPC state machine as defined by the EEBUS specification.

6.  **Event Handling**: The main module's event loop (`event_handler`) periodically calls the `sync()` method of the `EebusConnectionHandler`. This `sync()` method, in turn, runs the `EebusConnectionHandler`'s internal event loop (`m_handler`) once. This internal loop is responsible for handling all subsequent events.

7.  **Receiving gRPC Events**: A `UseCaseEventReader` runs in the background listening for incoming events from the gRPC service. When an event is received, it invokes a callback that posts an action to the `EebusConnectionHandler`'s internal event loop (`m_handler`).

8.  **State Machine and Limit Calculation**: The `m_handler` event loop executes the queued action, which calls the `LpcUseCaseHandler` to handle the event. The handler processes the event, runs its internal state machine, and determines the current power limit. The state machine is also periodically triggered by a timer within this same internal event loop.

9.  **Publishing Limits**: The calculated limits are then translated into an EVerest ``ExternalLimits`` schedule using the ``helper::translate_to_external_limits`` function. This schedule is published to the ``EnergyManager`` (or another connected module) via the ``eebus_energy_sink`` required interface.

State Machine Diagram
=====================

The following diagram shows the state machine of the `LpcUseCaseHandler`, which is responsible for the "Limitation of Power Consumption" (LPC) logic.

.. code-block:: plantuml

   @startuml
   title State Machine for LpcUseCaseHandler

   state "Init" as Init
   state "Unlimited / Controlled" as UnlimitedControlled
   state "Limited" as Limited
   state "Failsafe" as Failsafe
   state "Unlimited / Autonomous" as UnlimitedAutonomous

   [*] --> Init

   Init --> Limited : on DataUpdateLimit (active)
   Init --> UnlimitedControlled : on DataUpdateLimit (not active)
   Init --> UnlimitedAutonomous : on LPC Timeout (120s)
   Init --> Failsafe : on Heartbeat Timeout

   Limited --> UnlimitedControlled : on DataUpdateLimit (not active) or Limit expired
   Limited --> Failsafe : on Heartbeat Timeout

   UnlimitedControlled --> Limited : on DataUpdateLimit (active)
   UnlimitedControlled --> Failsafe : on Heartbeat Timeout

   UnlimitedAutonomous --> Limited : on DataUpdateLimit (active)
   UnlimitedAutonomous --> UnlimitedControlled : on DataUpdateLimit (not active) or Limit expired
   UnlimitedAutonomous --> Failsafe : on Heartbeat Timeout

   Failsafe --> Limited : on Heartbeat restored AND new active limit received
   Failsafe --> UnlimitedControlled : on Heartbeat restored AND new inactive limit received
   Failsafe --> UnlimitedAutonomous : on Failsafe Duration Timeout + LPC Timeout

   @enduml

The handler processes the following events:

- ``DataUpdateHeartbeat``: A heartbeat from the EEBUS service. If it's missing for a certain time, the handler goes into ``Failsafe`` state.
- ``DataUpdateLimit``: A new power limit from the EEBUS service.
- ``DataUpdateFailsafeDurationMinimum``: Update of the minimum failsafe duration.
- ``DataUpdateFailsafeConsumptionActivePowerLimit``: Update of the failsafe power limit.
- ``WriteApprovalRequired``: The handler needs to approve pending writes from the EEBUS service.
- ``UseCaseSupportUpdate``: The handler receives an update about use case support. This is currently ignored.

Based on its state and the received limits, the module publishes ``ExternalLimits`` to the ``eebus_energy_sink``, which is typically connected to an ``EnergyManager`` module.

Code Flow Diagram
=================

This sequence diagram illustrates the code flow when an event is received from the EEBUS service.

.. code-block:: plantuml

   @startuml
   title Code Flow Diagram

   participant EEBUS
   participant EebusConnectionHandler
   participant LpcUseCaseHandler
   participant UseCaseEventReader
   participant "gRPC Service" as GrpcService

   EEBUS ->> EebusConnectionHandler : create()
   activate EebusConnectionHandler
   EebusConnectionHandler -> EebusConnectionHandler : initialize_connection()
   deactivate EebusConnectionHandler

   EEBUS ->> EebusConnectionHandler : add_use_case(LPC)
   activate EebusConnectionHandler
   EebusConnectionHandler ->> LpcUseCaseHandler : create()
   EebusConnectionHandler ->> UseCaseEventReader : create()
   EebusConnectionHandler ->> GrpcService : AddUseCase()
   EebusConnectionHandler ->> UseCaseEventReader : start()
   activate UseCaseEventReader
   UseCaseEventReader ->> GrpcService : SubscribeUseCaseEvents()
   deactivate UseCaseEventReader
   deactivate EebusConnectionHandler

   EEBUS ->> EebusConnectionHandler : done_adding_use_case()
   activate EebusConnectionHandler
   EebusConnectionHandler -> EebusConnectionHandler : start_service()
   EebusConnectionHandler ->> GrpcService : StartService()
   EebusConnectionHandler ->> LpcUseCaseHandler : start()
   deactivate EebusConnectionHandler

   ... event arrives ...

   GrpcService ->> UseCaseEventReader : OnReadDone(event)
   activate UseCaseEventReader
   UseCaseEventReader ->> EebusConnectionHandler : event_callback(event)
   deactivate UseCaseEventReader
   activate EebusConnectionHandler
   EebusConnectionHandler -> EebusConnectionHandler : m_handler.add_action()
   note right: Event is queued in the event loop

   ... event loop runs ...

   EebusConnectionHandler ->> LpcUseCaseHandler : handle_event(event)
   activate LpcUseCaseHandler
   LpcUseCaseHandler -> LpcUseCaseHandler : run_state_machine()
   LpcUseCaseHandler -> LpcUseCaseHandler : apply_limit_for_current_state()
   LpcUseCaseHandler ->> EEBUS : callbacks.update_limits_callback()
   deactivate LpcUseCaseHandler
   deactivate EebusConnectionHandler


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
     - event_handler: fd_event_handler
     - event_handler_thread: thread
     + init()
     + ready()
   }

   class EebusConnectionHandler {
     - config: shared_ptr<ConfigValidator>
     - lpc_handler: unique_ptr<LpcUseCaseHandler>
     - event_reader: unique_ptr<UseCaseEventReader>
     - control_service_stub: shared_ptr<control_service::ControlService::Stub>
     - m_handler: fd_event_handler
     - state_machine_timer: timer_fd
     - reconnection_timer: timer_fd
     - last_use_case: EEBusUseCase
     - last_callbacks: EEBusCallbacks
     - use_case_added: bool
     + add_use_case()
     + stop()
     - initialize_connection()
     - configure_service()
     - create_channel_and_stub()
     - reconnect()
     - reset()
   }

   class LpcUseCaseHandler {
     - config: shared_ptr<ConfigValidator>
     - callbacks: eebus::EEBusCallbacks
     - stub: shared_ptr<cs_lpc::ControllableSystemLPCControl::Stub>
     - state: State
     + start()
     + handle_event()
     + run_state_machine()
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
   UseCaseEventReader ..> EebusConnectionHandler : event_callback
   EebusConnectionHandler ..> LpcUseCaseHandler

   @enduml

Robustness
==========

The module includes several features to make it resilient against connection losses and process crashes.

- **gRPC Process Restart**: If the module is configured to manage the ``eebus_grpc_api`` binary (via ``manage_eebus_grpc_api_binary: true``), it will automatically restart the binary if it crashes or exits unexpectedly. There is a 5-second delay between restart attempts.

- **gRPC Reconnection**: The ``EebusConnectionHandler`` will automatically try to reconnect to the gRPC service if the connection is lost. It will attempt to reconnect every 5 seconds. Once reconnected, it will re-establish the configured use cases.

Configuration
=============

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Key
     - Description
   * - ``manage_eebus_grpc_api_binary``
     - (boolean) Whether the module should manage the ``eebus_grpc_api`` binary. Default: ``true``
   * - ``eebus_service_port``
     - (integer) Port for the control service, this will be sent in the ``SetConfig`` call. Default: ``4715``
   * - ``grpc_port``
     - (integer) Port for gRPC control service connection. Required if ``manage_eebus_grpc_api_binary`` is true. Default: ``50051``
   * - ``eebus_ems_ski``
     - (string, required) The SKI of the EEBUS energy management system (e.g. HEMS) to connect to.
   * - ``certificate_path``
     - (string) Path to the certificate file. If relative, it will be prefixed with ``<etc>/everest/certs``. Required if ``manage_eebus_grpc_api_binary`` is true. Default: ``eebus/evse_cert``
   * - ``private_key_path``
     - (string) Path to the private key file. If relative, it will be prefixed with ``<etc>/everest/certs``. Required if ``manage_eebus_grpc_api_binary`` is true. Default: ``eebus/evse_key``
   * - ``eebus_grpc_api_binary_path``
     - (string) Path to the ``eebus_grpc_api`` binary. If relative, it will be prefixed with ``<libexec>``. Required if ``manage_eebus_grpc_api_binary`` is true. Default: ``eebus_grpc_api``
   * - ``vendor_code``
     - (string, required) Vendor code for the configuration of the control service.
   * - ``device_brand``
     - (string, required) Device brand for the configuration of the control service.
   * - ``device_model``
     - (string, required) Device model for the configuration of the control service.
   * - ``serial_number``
     - (string, required) Serial number for the configuration of the control service.
   * - ``failsafe_control_limit_W``
     - (integer) Failsafe control limit for the LPC use case in Watts. This is also used for the default consumption limit. Default: ``4200``
   * - ``max_nominal_power_W``
     - (integer) Maximum nominal power of the charging station in Watts. This is the maximum power the CS can consume. Default: ``32000``

Provided and required interfaces
================================

- Provides ``main`` (``empty`` interface).
- Requires ``eebus_energy_sink`` (``external_energy_limits`` interface). This is used to publish the calculated energy limits.

Adding a python test
====================

The python test suite for the EEBUS module is located in ``tests/eebus_tests``. The tests are written using the ``pytest`` framework.

To add a new test, you can add a new test function to the ``TestEEBUSModule`` class in ``eebus_tests.py`` or add a new test file.

A new test function could look like this:

.. code-block:: python

    @pytest.mark.asyncio
    async def test_my_new_feature(
        self,
        eebus_test_env: dict,
    ):
        """
        This test verifies my new feature.
        """
        # Unpack the test environment from the fixture
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        # Perform the handshake and get the probe module
        probe = await perform_eebus_handshake(control_service_servicer, cs_lpc_control_servicer, cs_lpc_control_server, everest_core)

        # Your test logic here

The ``eebus_test_env`` fixture provides a dictionary with the necessary components for the test:

- ``everest_core``: An instance of the ``EverestCore`` class, which manages the EVerest framework.
- ``control_service_servicer``: A mock gRPC control service.
- ``cs_lpc_control_servicer``: A mock gRPC LPC control service.
- ``cs_lpc_control_server``: The gRPC server for the LPC control service.

The ``perform_eebus_handshake`` helper function can be used to perform the initial handshake between the EEBUS module and the mock gRPC services.

For new test cases you can create a new class that inherits from ``TestData`` and implement the necessary methods to provide the test data. Then, you can add your new test data to the ``@pytest.mark.parametrize`` decorator in the ``test_set_load_limit`` test function.

To run the tests, you can use the ``ctest`` command from the build directory.

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