.. _everest_modules_handwritten_EEBUS:

*******************************************
EEBUS
*******************************************

:ref:`Link <everest_modules_EEBUS>` to the module's reference.

This document describes the EVerest EEBUS module. This module acts as a bridge between the EVerest framework and an external EEBUS gRPC service. It implements the "Limitation of Power consumption" (LPC) use case.

Architecture
============

Below is a diagram showing the architecture of the EEBUS module and its interaction with other components.

.. mermaid:: architecture.mmd

How it works
============

The module's main class is ``EEBUS``. Its ``init()`` method orchestrates the setup.

1.  **Configuration Validation**: It starts by using ``ConfigValidator`` to check the module's configuration provided in the EVerest ``config.yaml``. This includes validating ports, paths to binaries and certificates, and other parameters.

2.  **gRPC Binary Management**: If ``manage_eebus_grpc_api_binary`` is enabled in the config, the module starts the external ``eebus_grpc_api`` Go binary in a separate thread. This binary acts as a gRPC server for the EVerest module and a client to the actual EEBUS service (e.g., a HEMS).

3.  **Connection Handling**: The ``EebusConnectionHandler`` class is responsible for all gRPC communication. It connects to the ``eebus_grpc_api`` service, sends the device configuration (vendor, brand, model, etc.), and registers the "Limitation of Power Consumption" (LPC) use case.

4.  **Use Case Logic**: For the LPC use case, an ``LpcUseCaseHandler`` is created. This class implements the LPC state machine as defined by the EEBUS specification.

5.  **Startup Limit**: ``LpcUseCaseHandler::start()`` immediately applies the failsafe consumption limit per [LPC-901/1]. The CS shall already be operating under the failsafe limit while in Init state, before any HEMS communication has taken place.

6.  **Event Handling**: The main module's event loop (`event_handler`) periodically calls the `sync()` method of the `EebusConnectionHandler`. This `sync()` method, in turn, runs the `EebusConnectionHandler`'s internal event loop (`m_handler`) once. This internal loop is responsible for handling all subsequent events.

7.  **Receiving gRPC Events**: A `UseCaseEventReader` runs in the background listening for incoming events from the gRPC service. When an event is received, it invokes a callback that posts an action to the `EebusConnectionHandler`'s internal event loop (`m_handler`).

8.  **State Machine and Limit Calculation**: The `m_handler` event loop executes the queued action, which calls the `LpcUseCaseHandler` to handle the event. The handler processes the event, runs its internal state machine, and determines the current power limit. The state machine is also periodically triggered by a timer within this same internal event loop.

9.  **Publishing Limits**: The calculated limits are then translated into an EVerest ``ExternalLimits`` schedule using the ``helper::translate_to_external_limits`` function. This schedule is published to the ``EnergyManager`` (or another connected module) via the ``eebus_energy_sink`` required interface.

State Machine Diagram
=====================

The following diagram shows the state machine of the `LpcUseCaseHandler`, which is responsible for the "Limitation of Power Consumption" (LPC) logic.

.. mermaid:: state-machine.mmd

The handler processes the following events:

- ``DataUpdateHeartbeat``: A heartbeat from the EEBUS service. If it's missing for 120 seconds from ``Limited`` or ``Unlimited/controlled`` states, the handler enters ``Failsafe`` state [LPC-911/912]. ``Unlimited/autonomous`` has no heartbeat timeout — it can only be exited by receiving a new limit [LPC-918/919/920].
- ``DataUpdateLimit``: A new power limit from the EEBUS service.
- ``DataUpdateFailsafeDurationMinimum``: Update of the minimum failsafe duration (default 2 hours). When Failsafe is entered, this duration must expire before [LPC-922] exit is possible.
- ``DataUpdateFailsafeConsumptionActivePowerLimit``: Update of the failsafe power limit value applied in ``Init`` and ``Failsafe`` states.
- ``WriteApprovalRequired``: The handler needs to approve pending writes from the EEBUS service.
- ``UseCaseSupportUpdate``: The handler receives an update about use case support. This is currently ignored.

**Failsafe exit conditions** are independent per the spec:

- [LPC-922]: The Failsafe Duration Minimum expires — the CS MAY exit to ``Unlimited/autonomous`` regardless of heartbeat state.
- [LPC-921]: A heartbeat is received after Failsafe entry, but no new limit arrives within 120 s of that first heartbeat — the CS MAY exit to ``Unlimited/autonomous``.
- [LPC-916]: A heartbeat is received and a new limit was already written after Failsafe entry — the CS transitions to ``Limited`` (active limit) or ``Unlimited/controlled`` (inactive limit).

**Note on stale limits**: When entering Failsafe, the handler discards the previously received limit. After exiting Failsafe, the HEMS must explicitly resend a limit to drive transitions out of ``Unlimited/autonomous``.

Based on its state and the received limits, the module publishes ``ExternalLimits`` to the ``eebus_energy_sink``, which is typically connected to an ``EnergyManager`` module.

Code Flow Diagram
=================

This sequence diagram illustrates the code flow when an event is received from the EEBUS service.

.. mermaid:: code-flow.mmd

Class Diagram
=============

This diagram shows the main classes within the EEBUS module and their relationships.

.. mermaid:: class-diagram.mmd

Robustness
==========

The module includes several features to make it resilient against connection losses and process crashes.

- **gRPC Process Restart**: If the module is configured to manage the ``eebus_grpc_api`` binary (via ``manage_eebus_grpc_api_binary: true``), it will automatically restart the binary if it crashes or exits unexpectedly. The delay between restart attempts is configurable via ``restart_delay_s`` (default: 5 seconds).

- **gRPC Reconnection**: The ``EebusConnectionHandler`` will automatically try to reconnect to the gRPC service if the connection is lost. The delay between reconnect attempts is configurable via ``reconnect_delay_s`` (default: 5 seconds). Once reconnected, it will re-establish the configured use cases.

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
   * - ``eebus_ems_ski_allowlist``
     - (string) Comma-separated list of pre-trusted EEBUS EMS SKIs. Each entry is a 40-character lowercase SHA-1 hex digest; whitespace around entries is trimmed. Every allowlisted SKI is registered with the sidecar at startup. Default: ``""``
   * - ``accept_unknown_ems``
     - (boolean) If ``true``, every EG SKI discovered at runtime that is not already trusted and not in the allowlist is auto-registered for the duration of the session. Security-sensitive — only safe on isolated/trusted networks. Default: ``false``
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
   * - ``restart_delay_s``
     - (integer) Delay in seconds before restarting the ``eebus_grpc_api`` binary after it exits. Used when ``manage_eebus_grpc_api_binary`` is true. Default: ``5``
   * - ``reconnect_delay_s``
     - (integer) Delay in seconds before retrying a lost gRPC connection to the ``eebus_grpc_api`` service. Default: ``5``

SKI allowlist and discovery
===========================

The module trusts peer EEBUS Energy Guards (EGs, typically HEMS-class
controllers) via a combination of a static allowlist and optional runtime
auto-trust.

``eebus_ems_ski_allowlist``
---------------------------

Comma-separated list of pre-trusted EEBUS EMS SKIs. Each entry is a
40-character lowercase SHA-1 hex digest; whitespace around entries is
tolerated and trimmed.

.. code-block:: yaml

    eebus_ems_ski_allowlist: "abcdef0123456789abcdef0123456789abcdef01, aabbccddeeff00112233445566778899aabbccdd"

At startup the module iterates over the effective allowlist and calls
``RegisterRemoteSki`` once per entry with the sidecar before ``StartService``.
At runtime the module subscribes to discovery events from the sidecar; events
for SKIs already in the allowlist are treated as no-ops when the SKI is
already trusted by the sidecar, and trigger a re-register otherwise.

``accept_unknown_ems``
----------------------

Boolean flag (default ``false``). When ``true``, any EG SKI that appears in a
discovery event and is neither already trusted nor in the allowlist is
auto-registered for the duration of this session and a warning is logged.

.. warning::

   This flag is security-sensitive. Only enable it on isolated or trusted
   networks where every EEBUS peer that could appear on the LAN is known to
   be safe. Leave it ``false`` on production and shared networks.

The flag interacts with the allowlist as follows: allowlisted SKIs are
always auto-registered at startup regardless of this flag; the flag only
controls the "not in allowlist" branch of the runtime discovery classifier.

Discovery flow
==============

1. At startup every SKI in ``eebus_ems_ski_allowlist`` is registered with
   the sidecar before the service is started.
2. The module subscribes to discovery events from the sidecar via
   ``SubscribeDiscoveryEvents``.
3. For each ``DISCOVERED`` event the module applies one of four actions,
   based on allowlist membership and the ``accept_unknown_ems`` flag:

   .. list-table::
      :widths: 40 20 40
      :header-rows: 1

      * - Condition
        - Action
        - Log level
      * - SKI already trusted by sidecar
        - no-op
        - debug
      * - SKI in allowlist (not yet trusted)
        - register
        - info
      * - SKI unknown, ``accept_unknown_ems=true``
        - register
        - warning
      * - SKI unknown, ``accept_unknown_ems=false``
        - ignore
        - info

4. The sidecar initiates pairing handshakes with every trusted SKI.

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