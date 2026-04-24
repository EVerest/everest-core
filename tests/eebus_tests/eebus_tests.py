# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import logging
import pytest
import os
import pathlib
import asyncio
import datetime

from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.controller.everest_test_controller import EverestTestController

from grpc_servicer.control_service_servicer import ControlServiceServicer
from grpc_server.control_service_server import ControlServiceServer
from grpc_servicer.cs_lpc_control_servicer import CsLpcControlServicer
from grpc_server.cs_lpc_control_server import CsLpcControlServer

from fixtures.eebus_module_test import eebus_test_env, EebusTestProbeModule, everest_config_strategies, eebus_grpc_port, eebus_service_port
from fixtures.grpc_testing_server import control_service_server, control_service_servicer, cs_lpc_control_server, cs_lpc_control_servicer
from helpers.import_helpers import insert_dir_to_sys_path
from helpers.async_helpers import async_get, async_wait_for
from conftest import EebusModuleConfigStrategy

from test_data.test_data import TestData
from test_data.test_data_not_active_load_limit import TestDataNotActiveLoadLimit
from test_data.test_data_active_load_limit import TestDataActiveLoadLimit
from test_data.test_data_active_load_limit_with_duration import TestDataActiveLoadLimitWithDuration
from test_data.test_data_failsafe import TestDataFailsafe
from test_data.test_data_heartbeat import TestDataHeartbeat

insert_dir_to_sys_path(pathlib.Path(os.path.dirname(os.path.realpath(__file__))) / "grpc_libs/generated")
import common_types.types_pb2 as common_types_pb2
import usecases.cs.lpc.messages_pb2 as cs_lpc_messages_pb2
import control_service.types_pb2 as control_service_types_pb2
import control_service.messages_pb2 as control_service_messages_pb2


async def perform_eebus_handshake(
    control_service_servicer: ControlServiceServicer,
    cs_lpc_control_servicer: CsLpcControlServicer,
    cs_lpc_control_server: CsLpcControlServer,
    everest_core: EverestCore,
    expected_register_ski_count: int = 1,
) -> EebusTestProbeModule:
    """This helper function handles the complete EEBUS module init handshake and returns a started probe module.

    ``expected_register_ski_count`` is the number of ``RegisterRemoteSki`` calls the module is
    expected to make before ``StartService`` (one per SKI in the effective allowlist). The caller
    receives the list of observed SKIs via ``probe.registered_skis_at_boot`` after this returns.
    """

    # Handle SetConfig
    control_service_servicer.command_queues["SetConfig"].response_queue.put_nowait(
        control_service_messages_pb2.EmptyResponse()
    )
    req = await async_get(control_service_servicer.command_queues["SetConfig"].request_queue, timeout=10)
    assert req is not None
    assert req.port > 0

    # Start the probe module
    probe = EebusTestProbeModule(everest_core.get_runtime_session())
    probe.start()

    # Handle StartSetup
    req = await async_get(control_service_servicer.command_queues["StartSetup"].request_queue, timeout=5)
    control_service_servicer.command_queues["StartSetup"].response_queue.put_nowait(
        control_service_messages_pb2.EmptyResponse()
    )

    # Handle RegisterRemoteSki — the module calls this once per SKI in the effective allowlist.
    # Tests that customize the allowlist pass a matching ``expected_register_ski_count``.
    probe.registered_skis_at_boot = []
    for _ in range(expected_register_ski_count):
        control_service_servicer.command_queues["RegisterRemoteSki"].response_queue.put_nowait(
            control_service_messages_pb2.EmptyResponse()
        )
    for _ in range(expected_register_ski_count):
        req = await async_get(control_service_servicer.command_queues["RegisterRemoteSki"].request_queue, timeout=5)
        probe.registered_skis_at_boot.append(req.remote_ski)

    # Handle AddUseCase
    req = await async_get(control_service_servicer.command_queues["AddUseCase"].request_queue, timeout=5)
    res = control_service_messages_pb2.AddUseCaseResponse(
        endpoint=f"localhost:{cs_lpc_control_server.get_port()}"
    )
    control_service_servicer.command_queues["AddUseCase"].response_queue.put_nowait(
        res)

    # Handle SetConsumptionNominalMax
    req = await async_get(cs_lpc_control_servicer.command_queues["SetConsumptionNominalMax"].request_queue, timeout=5)
    cs_lpc_control_servicer.command_queues["SetConsumptionNominalMax"].response_queue.put_nowait(
        cs_lpc_messages_pb2.SetConsumptionNominalMaxResponse()
    )

    # Handle SetConsumptionLimit
    req = await async_get(cs_lpc_control_servicer.command_queues["SetConsumptionLimit"].request_queue, timeout=5)
    cs_lpc_control_servicer.command_queues["SetConsumptionLimit"].response_queue.put_nowait(
        cs_lpc_messages_pb2.SetConsumptionLimitResponse()
    )

    # Handle SetFailsafeConsumptionActivePowerLimit
    req = await async_get(cs_lpc_control_servicer.command_queues["SetFailsafeConsumptionActivePowerLimit"].request_queue, timeout=5)
    cs_lpc_control_servicer.command_queues["SetFailsafeConsumptionActivePowerLimit"].response_queue.put_nowait(
        cs_lpc_messages_pb2.SetFailsafeConsumptionActivePowerLimitResponse()
    )

    # Handle SetFailsafeDurationMinimum
    req = await async_get(cs_lpc_control_servicer.command_queues["SetFailsafeDurationMinimum"].request_queue, timeout=5)
    cs_lpc_control_servicer.command_queues["SetFailsafeDurationMinimum"].response_queue.put_nowait(
        cs_lpc_messages_pb2.SetFailsafeDurationMinimumResponse()
    )

    # Handle StartService
    req = await async_get(control_service_servicer.command_queues["StartService"].request_queue, timeout=5)
    control_service_servicer.command_queues["StartService"].response_queue.put_nowait(
        control_service_messages_pb2.EmptyResponse()
    )

    # Handle StartHeartbeat
    req = await async_get(cs_lpc_control_servicer.command_queues["StartHeartbeat"].request_queue, timeout=5)
    cs_lpc_control_servicer.command_queues["StartHeartbeat"].response_queue.put_nowait(
        cs_lpc_messages_pb2.StartHeartbeatResponse()
    )

    await probe.wait_to_be_ready()

    # The module applies the failsafe limit immediately at startup per LPC-901/1.
    # Drain it here so individual tests start with a clean queue and can assert on
    # the limit from their own specific scenario.
    await async_get(probe.external_limits_queue, timeout=5)

    return probe


async def transition_to_unlimited_controlled(
    control_service_servicer: ControlServiceServicer,
    cs_lpc_control_servicer: CsLpcControlServicer,
    probe: EebusTestProbeModule,
) -> None:
    """Move the module from Init to UnlimitedControlled and drain the resulting limit.

    After perform_eebus_handshake the module is in Init state.  Init has no outgoing
    Failsafe edge — instead it transitions to UnlimitedAutonomous after 120 s
    [LPC-906].  Tests that need to reach Failsafe via heartbeat timeout must first
    leave Init by delivering a heartbeat and an inactive limit, which drives the
    Init → UnlimitedControlled transition [LPC-902/905].  From UnlimitedControlled
    the normal heartbeat-timeout → Failsafe edge [LPC-911] is active.
    """
    hb_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
        remote_ski="this-is-a-ski-42",
        remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
        use_case_event=control_service_types_pb2.UseCaseEvent(
            use_case=control_service_types_pb2.UseCase(
                actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
            ),
            event="DataUpdateHeartbeat",
        ),
    )
    control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(hb_event)

    limit_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
        remote_ski="this-is-a-ski-42",
        remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
        use_case_event=control_service_types_pb2.UseCaseEvent(
            use_case=control_service_types_pb2.UseCase(
                actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
            ),
            event="DataUpdateLimit",
        ),
    )
    control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(limit_event)

    req = await async_get(cs_lpc_control_servicer.command_queues["ConsumptionLimit"].request_queue, timeout=5)
    assert req is not None
    cs_lpc_control_servicer.command_queues["ConsumptionLimit"].response_queue.put_nowait(
        cs_lpc_messages_pb2.ConsumptionLimitResponse(
            load_limit=common_types_pb2.LoadLimit(is_active=False, value=0),
        )
    )

    # Drain the UnlimitedControlled limit the transition emits.
    await async_get(probe.external_limits_queue, timeout=5)


@pytest.mark.everest_core_config("config-test-eebus-module-001.yaml")
@pytest.mark.probe_module
class TestEEBUSModule:
    """
    This test class is used to test the EEBUS module.
    """
    @pytest.mark.parametrize(
        "test_data",
        [
            pytest.param(
                TestDataNotActiveLoadLimit(),
                id="not_active_load_limit",
            ),
            pytest.param(
                TestDataActiveLoadLimit(),
                id="active_load_limit",
            ),
            pytest.param(
                TestDataActiveLoadLimitWithDuration(),
                id="active_load_limit_with_duration",
            ),
            pytest.param(
                TestDataFailsafe(),
                id="failsafe_test_data",
            ),
            pytest.param(
                TestDataHeartbeat(),
                id="heartbeat_test_data",
            )
        ]
    )
    @pytest.mark.asyncio
    async def test_set_load_limit(
        self,
        eebus_test_env: dict,
        test_data: TestData,
    ):
        """
        This test sets the load limit and checks if the EEBUS module
        sends the correct limits to the probe module.
        """
        # Unpack the test environment from the fixture
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        # Perform the handshake and get the probe module
        probe = await perform_eebus_handshake(control_service_servicer, cs_lpc_control_servicer, cs_lpc_control_server, everest_core)

        # --- Handshake complete, now test the load limit logic ---

        # Simulate a WriteApprovalRequired event from the HEMS
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(
                entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="WriteApprovalRequired",
            )
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(
            uc_event)

        # The module should now ask for the pending limit
        req = await async_get(cs_lpc_control_servicer.command_queues["PendingConsumptionLimit"].request_queue, timeout=5)
        assert req is not None

        # We respond with the test data's limit
        res = cs_lpc_messages_pb2.PendingConsumptionLimitResponse(
            load_limits={0: test_data.get_load_limit()})
        cs_lpc_control_servicer.command_queues["PendingConsumptionLimit"].response_queue.put_nowait(
            res)

        # The module should now approve the limit
        req = await async_get(cs_lpc_control_servicer.command_queues["ApproveOrDenyConsumptionLimit"].request_queue, timeout=5)
        assert req is not None
        assert req.msg_counter == 0
        assert req.approve
        cs_lpc_control_servicer.command_queues["ApproveOrDenyConsumptionLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.ApproveOrDenyConsumptionLimitResponse()
        )

        # Simulate a DataUpdateLimit event from the HEMS
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(
                entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="DataUpdateLimit",
            )
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(
            uc_event)

        # The module should now ask for the consumption limit
        req = await async_get(cs_lpc_control_servicer.command_queues["ConsumptionLimit"].request_queue, timeout=5)
        assert req is not None

        # We respond with the test data's limit
        res = cs_lpc_messages_pb2.ConsumptionLimitResponse(
            load_limit=test_data.get_load_limit(),
        )
        cs_lpc_control_servicer.command_queues["ConsumptionLimit"].response_queue.put_nowait(
            res)

        # Finally, the module should have sent the translated limits to our probe module
        expected = test_data.get_expected_external_limits()
        limits = await async_wait_for(probe.external_limits_queue, expected, timeout=10)
        test_data.run_additional_assertions(limits)

    @pytest.mark.asyncio
    async def test_rejects_negative_load_limit(
        self,
        eebus_test_env: dict,
    ):
        """[LPC-001] CS must NACK a negative Active Power Consumption Limit."""
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(
            control_service_servicer, cs_lpc_control_servicer, cs_lpc_control_server, everest_core
        )

        # Push WriteApprovalRequired so the module will read PendingConsumptionLimit.
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="WriteApprovalRequired",
            ),
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        req = await async_get(cs_lpc_control_servicer.command_queues["PendingConsumptionLimit"].request_queue, timeout=5)
        assert req is not None

        negative_limit = common_types_pb2.LoadLimit(
            duration_nanoseconds=0,
            is_changeable=True,
            is_active=True,
            value=-100.0,
            delete_duration=False,
        )
        res = cs_lpc_messages_pb2.PendingConsumptionLimitResponse(load_limits={0: negative_limit})
        cs_lpc_control_servicer.command_queues["PendingConsumptionLimit"].response_queue.put_nowait(res)

        approve_req = await async_get(
            cs_lpc_control_servicer.command_queues["ApproveOrDenyConsumptionLimit"].request_queue, timeout=10
        )
        assert approve_req is not None
        assert approve_req.approve is False
        assert approve_req.msg_counter == 0
        assert "LPC-001" in approve_req.reason
        cs_lpc_control_servicer.command_queues["ApproveOrDenyConsumptionLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.ApproveOrDenyConsumptionLimitResponse()
        )

    @pytest.mark.asyncio
    async def test_ignores_out_of_range_failsafe_duration(
        self,
        eebus_test_env: dict,
    ):
        """[LPC-022] CS must ignore EG-written FailsafeDurationMinimum outside [2h, 24h]
        and keep the previously cached 2h default."""
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(control_service_servicer, cs_lpc_control_servicer, cs_lpc_control_server, everest_core)

        # Inject an out-of-range DataUpdateFailsafeDurationMinimum event (5 s — below the 2 h minimum).
        out_of_range_nanoseconds = 5 * 1000 * 1000 * 1000  # 5 seconds
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="DataUpdateFailsafeDurationMinimum",
            )
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        req = await async_get(cs_lpc_control_servicer.command_queues["FailsafeDurationMinimum"].request_queue, timeout=5)
        res = cs_lpc_messages_pb2.FailsafeDurationMinimumResponse(
            duration_nanoseconds=out_of_range_nanoseconds,
            is_changeable=True,
        )
        cs_lpc_control_servicer.command_queues["FailsafeDurationMinimum"].response_queue.put_nowait(res)

        # Move out of Init so the heartbeat-timeout → Failsafe edge [LPC-911] is reachable.
        await transition_to_unlimited_controlled(control_service_servicer, cs_lpc_control_servicer, probe)

        # Let the 120 s heartbeat timeout expire — the module must enter Failsafe.
        test_data_failsafe = TestDataFailsafe()
        failsafe_limits = test_data_failsafe.get_expected_failsafe_limits(4200)  # config default
        limits = await async_wait_for(probe.external_limits_queue, failsafe_limits, timeout=140)

        # The C++ predicate must have rejected the 5 s write and kept the 2 h default.
        # Therefore the module must still be in Failsafe 10 s later — NOT in UnlimitedAutonomous.
        await asyncio.sleep(10)
        with pytest.raises(TimeoutError):
            await async_get(probe.external_limits_queue, timeout=1)

    @pytest.mark.asyncio
    async def test_update_failsafe_consumption_active_power_limit(
        self,
        eebus_test_env: dict,
    ):
        """
        This test verifies that the failsafe consumption active power limit can be updated.
        """
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(control_service_servicer, cs_lpc_control_servicer, cs_lpc_control_server, everest_core)

        # Simulate a DataUpdateFailsafeConsumptionActivePowerLimit event
        new_failsafe_control_limit = 5000
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="DataUpdateFailsafeConsumptionActivePowerLimit",
            )
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        # The module should now ask for the failsafe consumption active power limit
        req = await async_get(cs_lpc_control_servicer.command_queues["FailsafeConsumptionActivePowerLimit"].request_queue, timeout=5)
        res = cs_lpc_messages_pb2.FailsafeConsumptionActivePowerLimitResponse(
            limit=new_failsafe_control_limit,
            is_changeable=True,
        )
        cs_lpc_control_servicer.command_queues["FailsafeConsumptionActivePowerLimit"].response_queue.put_nowait(res)

        # Move out of Init so the heartbeat-timeout → Failsafe edge [LPC-911] is reachable.
        await transition_to_unlimited_controlled(control_service_servicer, cs_lpc_control_servicer, probe)

        # Trigger failsafe state to check if the new limit is applied
        # Simulate missing heartbeat
        test_data_failsafe = TestDataFailsafe()
        failsafe_limits = test_data_failsafe.get_expected_failsafe_limits(new_failsafe_control_limit)
        limits = await async_get(probe.external_limits_queue, timeout=130) # Wait for 120s heartbeat timeout + buffer
        assert failsafe_limits == limits

    @pytest.mark.asyncio
    async def test_ignores_negative_failsafe_consumption_limit(
        self,
        eebus_test_env: dict,
    ):
        """[LPC-001] CS must ignore a negative FailsafeConsumptionActivePowerLimit write
        and keep the previously cached (4200 W default) value."""
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(
            control_service_servicer, cs_lpc_control_servicer, cs_lpc_control_server, everest_core
        )

        # Push DataUpdateFailsafeConsumptionActivePowerLimit event with a negative value.
        negative_limit = -1000.0
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="DataUpdateFailsafeConsumptionActivePowerLimit",
            ),
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        req = await async_get(cs_lpc_control_servicer.command_queues["FailsafeConsumptionActivePowerLimit"].request_queue, timeout=5)
        res = cs_lpc_messages_pb2.FailsafeConsumptionActivePowerLimitResponse(
            limit=negative_limit,
            is_changeable=True,
        )
        cs_lpc_control_servicer.command_queues["FailsafeConsumptionActivePowerLimit"].response_queue.put_nowait(res)

        # Move out of Init so the heartbeat-timeout → Failsafe edge [LPC-911] is reachable.
        await transition_to_unlimited_controlled(control_service_servicer, cs_lpc_control_servicer, probe)

        # When Failsafe triggers, the applied limit must still be the 4200 W config default — NOT -1000.
        test_data_failsafe = TestDataFailsafe()
        expected_failsafe_limits = test_data_failsafe.get_expected_failsafe_limits(4200)  # config default
        limits = await async_wait_for(probe.external_limits_queue, expected_failsafe_limits, timeout=140)

    @pytest.mark.asyncio
    async def test_failsafe_to_unlimited_controlled(
        self,
        eebus_test_env: dict,
    ):
        """
        This test verifies that the module transitions from failsafe to unlimited controlled
        when a heartbeat is received.
        """
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(control_service_servicer, cs_lpc_control_servicer, cs_lpc_control_server, everest_core)

        # Move out of Init so the heartbeat-timeout → Failsafe edge [LPC-911] is reachable.
        await transition_to_unlimited_controlled(control_service_servicer, cs_lpc_control_servicer, probe)

        # Simulate missing heartbeat to enter failsafe state
        test_data_failsafe = TestDataFailsafe()
        failsafe_limits = test_data_failsafe.get_expected_failsafe_limits(4200)
        limits = await async_get(probe.external_limits_queue, timeout=130) # Wait for 120s heartbeat timeout + buffer
        assert failsafe_limits == limits

        # Simulate a DataUpdateHeartbeat event
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="DataUpdateHeartbeat",
            )
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        # Simulate a WriteApprovalRequired event from the HEMS
        uc_event_write_approval = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="WriteApprovalRequired",
            )
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event_write_approval)

        req_pending = await async_get(cs_lpc_control_servicer.command_queues["PendingConsumptionLimit"].request_queue, timeout=5)
        res_pending = cs_lpc_messages_pb2.PendingConsumptionLimitResponse(
            load_limits={0: common_types_pb2.LoadLimit(is_active=False, value=1000)} # Dummy limit
        )
        cs_lpc_control_servicer.command_queues["PendingConsumptionLimit"].response_queue.put_nowait(res_pending)

        req_approve = await async_get(cs_lpc_control_servicer.command_queues["ApproveOrDenyConsumptionLimit"].request_queue, timeout=5)
        assert req_approve.approve
        cs_lpc_control_servicer.command_queues["ApproveOrDenyConsumptionLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.ApproveOrDenyConsumptionLimitResponse()
        )
        # Simulate a DataUpdateLimit event to set an initial limit
        test_data_active = TestDataActiveLoadLimit()
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="DataUpdateLimit",
            )
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)
        # The module should now ask for the consumption limit
        req = await async_get(cs_lpc_control_servicer.command_queues["ConsumptionLimit"].request_queue, timeout=5)
        assert req is not None

        # We respond with the test data's limit
        res = cs_lpc_messages_pb2.ConsumptionLimitResponse(
            load_limit=common_types_pb2.LoadLimit(is_active=False, value=1000),
        )
        cs_lpc_control_servicer.command_queues["ConsumptionLimit"].response_queue.put_nowait(
            res)

        # The module should transition to UnlimitedControlled state
        test_data_not_active_load_limit = TestDataNotActiveLoadLimit()
        unlimited_controlled_limits = test_data_not_active_load_limit.get_expected_external_limits()
        limits = await async_get(probe.external_limits_queue, timeout=5)
        assert unlimited_controlled_limits == limits

    @pytest.mark.asyncio
    async def test_failsafe_to_limited(
        self,
        eebus_test_env: dict,
    ):
        """
        This test verifies that the module transitions from failsafe to unlimited controlled
        when a heartbeat is received.
        """
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(control_service_servicer, cs_lpc_control_servicer, cs_lpc_control_server, everest_core)

        # Move out of Init so the heartbeat-timeout → Failsafe edge [LPC-911] is reachable.
        await transition_to_unlimited_controlled(control_service_servicer, cs_lpc_control_servicer, probe)

        # Simulate missing heartbeat to enter failsafe state
        test_data_failsafe = TestDataFailsafe()
        failsafe_limits = test_data_failsafe.get_expected_failsafe_limits(4200)
        limits = await async_get(probe.external_limits_queue, timeout=130) # Wait for 120s heartbeat timeout + buffer
        assert failsafe_limits == limits

        # Simulate a DataUpdateHeartbeat event
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="DataUpdateHeartbeat",
            )
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        # Simulate a WriteApprovalRequired event from the HEMS
        uc_event_write_approval = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="WriteApprovalRequired",
            )
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event_write_approval)

        test_data_active_load_limit = TestDataActiveLoadLimit()
        req_pending = await async_get(cs_lpc_control_servicer.command_queues["PendingConsumptionLimit"].request_queue, timeout=5)
        res_pending = cs_lpc_messages_pb2.PendingConsumptionLimitResponse(
            load_limits={0: test_data_active_load_limit.get_load_limit()} # Dummy limit
        )
        cs_lpc_control_servicer.command_queues["PendingConsumptionLimit"].response_queue.put_nowait(res_pending)

        req_approve = await async_get(cs_lpc_control_servicer.command_queues["ApproveOrDenyConsumptionLimit"].request_queue, timeout=5)
        assert req_approve.approve
        cs_lpc_control_servicer.command_queues["ApproveOrDenyConsumptionLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.ApproveOrDenyConsumptionLimitResponse()
        )
        # Simulate a DataUpdateLimit event to set an initial limit
        test_data_active = TestDataActiveLoadLimit()
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="DataUpdateLimit",
            )
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)
        # The module should now ask for the consumption limit
        req = await async_get(cs_lpc_control_servicer.command_queues["ConsumptionLimit"].request_queue, timeout=5)
        assert req is not None

        # We respond with the test data's limit
        res = cs_lpc_messages_pb2.ConsumptionLimitResponse(
            load_limit=test_data_active_load_limit.get_load_limit(),
        )
        cs_lpc_control_servicer.command_queues["ConsumptionLimit"].response_queue.put_nowait(
            res)

        # The module should transition to UnlimitedControlled state
        limited_limits = test_data_active_load_limit.get_expected_external_limits()
        limits = await async_get(probe.external_limits_queue, timeout=5)
        assert limited_limits == limits

    @pytest.mark.asyncio
    async def test_use_case_support_update_reinitializes(
        self,
        eebus_test_env: dict,
    ):
        """
        When UseCaseSupportUpdate fires (e.g. EG reconnects at SPINE level while the gRPC channel
        to the sidecar stays up), the module must re-call configure_use_case() and start_heartbeat()
        so the sidecar has current values and the heartbeat subscription is renewed.
        """
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(
            control_service_servicer, cs_lpc_control_servicer, cs_lpc_control_server, everest_core
        )

        # Simulate the EG reconnecting at SPINE level — the sidecar fires UseCaseSupportUpdate.
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="UseCaseSupportUpdate",
            ),
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        # The module must re-run configure_use_case() — service all four setter calls.
        req = await async_get(cs_lpc_control_servicer.command_queues["SetConsumptionNominalMax"].request_queue, timeout=5)
        assert req is not None
        cs_lpc_control_servicer.command_queues["SetConsumptionNominalMax"].response_queue.put_nowait(
            cs_lpc_messages_pb2.SetConsumptionNominalMaxResponse()
        )

        req = await async_get(cs_lpc_control_servicer.command_queues["SetConsumptionLimit"].request_queue, timeout=5)
        assert req is not None
        cs_lpc_control_servicer.command_queues["SetConsumptionLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.SetConsumptionLimitResponse()
        )

        req = await async_get(cs_lpc_control_servicer.command_queues["SetFailsafeConsumptionActivePowerLimit"].request_queue, timeout=5)
        assert req is not None
        cs_lpc_control_servicer.command_queues["SetFailsafeConsumptionActivePowerLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.SetFailsafeConsumptionActivePowerLimitResponse()
        )

        req = await async_get(cs_lpc_control_servicer.command_queues["SetFailsafeDurationMinimum"].request_queue, timeout=5)
        assert req is not None
        cs_lpc_control_servicer.command_queues["SetFailsafeDurationMinimum"].response_queue.put_nowait(
            cs_lpc_messages_pb2.SetFailsafeDurationMinimumResponse()
        )

        # The module must also re-subscribe to the heartbeat.
        req = await async_get(cs_lpc_control_servicer.command_queues["StartHeartbeat"].request_queue, timeout=5)
        assert req is not None
        cs_lpc_control_servicer.command_queues["StartHeartbeat"].response_queue.put_nowait(
            cs_lpc_messages_pb2.StartHeartbeatResponse()
        )

    @pytest.mark.asyncio
    async def test_init_state_timeout(
        self,
        eebus_test_env: dict,
    ):
        """
        This test verifies that the module transitions from Init to UnlimitedAutonomous
        if no events are received within LPC_TIMEOUT (120), but heartbeats are received as to not go into failsafe.
        """
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(control_service_servicer, cs_lpc_control_servicer, cs_lpc_control_server, everest_core)

        # Simulate a DataUpdateHeartbeat event
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="DataUpdateHeartbeat",
            )
        )
        # Send periodic heartbeats in the background to prevent failsafe while
        # waiting for the Init → UnlimitedAutonomous transition (120s LPC_INIT_TIMEOUT).
        async def send_heartbeats():
            for _ in range(3):
                control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)
                await asyncio.sleep(50)

        hb_task = asyncio.create_task(send_heartbeats())
        try:
            # No limits are sent, so the module should transition from Init to UnlimitedAutonomous
            test_data_not_active_load_limit = TestDataNotActiveLoadLimit()
            unlimited_autonomous_limits = test_data_not_active_load_limit.get_expected_external_limits()
            limits = await async_wait_for(probe.external_limits_queue, unlimited_autonomous_limits, timeout=140)
        finally:
            hb_task.cancel()
            try:
                await hb_task
            except asyncio.CancelledError:
                pass


# ---------------------------------------------------------------------------
# Phase 3A helpers: allowlist / discovery / active-EMS connection tests
# ---------------------------------------------------------------------------


def ski(first_char: str) -> str:
    """Expand a single hex char to a 40-char lowercase SKI for readable inputs."""
    return first_char * 40


SKI_A = ski("a")
SKI_B = ski("b")
SKI_C = ski("c")
SKI_UNKNOWN = ski("e")


def _discovery_event(
    ski_value: str,
    is_trusted: bool = False,
    event_type=None,
    brand: str = "TestBrand",
    model: str = "TestModel",
) -> "control_service_messages_pb2.DiscoveryEvent":
    return control_service_messages_pb2.DiscoveryEvent(
        type=event_type if event_type is not None else control_service_messages_pb2.DiscoveryEvent.DISCOVERED,
        remote_ski=ski_value,
        brand=brand,
        model=model,
        is_trusted=is_trusted,
    )


def _usecase_event(ski_value: str, event_name: str) -> "control_service_messages_pb2.SubscribeUseCaseEventsResponse":
    return control_service_messages_pb2.SubscribeUseCaseEventsResponse(
        remote_ski=ski_value,
        remote_entity_address=common_types_pb2.EntityAddress(entity_address=[1]),
        use_case_event=control_service_types_pb2.UseCaseEvent(
            use_case=control_service_types_pb2.UseCase(
                actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
            ),
            event=event_name,
        ),
    )


async def _assert_no_register_remote_ski(control_service_servicer: ControlServiceServicer, window_s: float = 2.0) -> None:
    """Assert the module does NOT call RegisterRemoteSki within ``window_s`` seconds."""
    try:
        req = await asyncio.wait_for(
            control_service_servicer.command_queues["RegisterRemoteSki"].request_queue.get(),
            timeout=window_s,
        )
        pytest.fail(f"Unexpected RegisterRemoteSki call for SKI={req.remote_ski}")
    except asyncio.TimeoutError:
        pass  # expected — no register call


@pytest.mark.probe_module
class TestEEBUSAllowlistDiscovery:
    """Phase 3A tests for the multi-SKI allowlist, runtime discovery events, and
    the LPC active-EMS connection behavior."""

    # --- Test 1 --------------------------------------------------------------

    @pytest.mark.everest_core_config("config-test-eebus-module-001.yaml")
    @pytest.mark.everest_config_adaptions(
        EebusModuleConfigStrategy({
            "eebus_ems_ski_allowlist": f"{SKI_A},{SKI_B},{SKI_C}",
        })
    )
    @pytest.mark.asyncio
    async def test_allowlist_registers_multiple_skis_at_boot(
        self,
        eebus_test_env: dict,
    ):
        """[Phase 3A] The module must call RegisterRemoteSki once per allowlist entry before StartService."""
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(
            control_service_servicer,
            cs_lpc_control_servicer,
            cs_lpc_control_server,
            everest_core,
            expected_register_ski_count=3,
        )

        assert set(probe.registered_skis_at_boot) == {SKI_A, SKI_B, SKI_C}

    # --- Test 2 --------------------------------------------------------------

    @pytest.mark.everest_core_config("config-test-eebus-module-001.yaml")
    @pytest.mark.everest_config_adaptions(
        EebusModuleConfigStrategy({
            "eebus_ems_ski_allowlist": f"{SKI_A},{SKI_B}",
        })
    )
    @pytest.mark.asyncio
    async def test_discovery_event_allowlisted_auto_registers(
        self,
        eebus_test_env: dict,
    ):
        """[Phase 3A] A DISCOVERED (is_trusted=False) event for an allowlisted SKI triggers a runtime RegisterRemoteSki."""
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(
            control_service_servicer,
            cs_lpc_control_servicer,
            cs_lpc_control_server,
            everest_core,
            expected_register_ski_count=2,
        )

        # Pre-seed a response for the expected runtime RegisterRemoteSki call.
        control_service_servicer.command_queues["RegisterRemoteSki"].response_queue.put_nowait(
            control_service_messages_pb2.EmptyResponse()
        )
        # Push DISCOVERED event for allowlisted SKI_B (is_trusted=False).
        control_service_servicer.command_queues["SubscribeDiscoveryEvents"].response_queue.put_nowait(
            _discovery_event(SKI_B, is_trusted=False)
        )

        req = await async_get(
            control_service_servicer.command_queues["RegisterRemoteSki"].request_queue, timeout=5
        )
        assert req.remote_ski == SKI_B

    # --- Test 3 --------------------------------------------------------------

    @pytest.mark.everest_core_config("config-test-eebus-module-001.yaml")
    @pytest.mark.everest_config_adaptions(
        EebusModuleConfigStrategy({
            "eebus_ems_ski_allowlist": SKI_A,
        })
    )
    @pytest.mark.asyncio
    async def test_discovery_event_trusted_no_op(
        self,
        eebus_test_env: dict,
    ):
        """[Phase 3A] DISCOVERED events where the sidecar already trusts the SKI (is_trusted=True) are no-ops."""
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(
            control_service_servicer,
            cs_lpc_control_servicer,
            cs_lpc_control_server,
            everest_core,
            expected_register_ski_count=1,
        )
        assert probe.registered_skis_at_boot == [SKI_A]

        # Trusted discovery event — must NOT trigger any further RegisterRemoteSki call.
        control_service_servicer.command_queues["SubscribeDiscoveryEvents"].response_queue.put_nowait(
            _discovery_event(SKI_A, is_trusted=True)
        )
        await _assert_no_register_remote_ski(control_service_servicer, window_s=2.0)

    # --- Test 4 --------------------------------------------------------------

    @pytest.mark.everest_core_config("config-test-eebus-module-001.yaml")
    @pytest.mark.everest_config_adaptions(
        EebusModuleConfigStrategy({
            "eebus_ems_ski_allowlist": "",
            "accept_unknown_ems": True,
        })
    )
    @pytest.mark.asyncio
    async def test_discovery_event_unknown_accepts_when_flag_true(
        self,
        eebus_test_env: dict,
    ):
        """[Phase 3A] accept_unknown_ems=true: unknown discovered EGs are auto-registered at runtime."""
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(
            control_service_servicer,
            cs_lpc_control_servicer,
            cs_lpc_control_server,
            everest_core,
            expected_register_ski_count=0,
        )
        assert probe.registered_skis_at_boot == []

        # Pre-seed the runtime RegisterRemoteSki response.
        control_service_servicer.command_queues["RegisterRemoteSki"].response_queue.put_nowait(
            control_service_messages_pb2.EmptyResponse()
        )
        control_service_servicer.command_queues["SubscribeDiscoveryEvents"].response_queue.put_nowait(
            _discovery_event(SKI_UNKNOWN, is_trusted=False)
        )

        req = await async_get(
            control_service_servicer.command_queues["RegisterRemoteSki"].request_queue, timeout=5
        )
        assert req.remote_ski == SKI_UNKNOWN

    # --- Test 5 --------------------------------------------------------------

    @pytest.mark.everest_core_config("config-test-eebus-module-001.yaml")
    @pytest.mark.everest_config_adaptions(
        EebusModuleConfigStrategy({
            "eebus_ems_ski_allowlist": SKI_A,
            "accept_unknown_ems": False,
        })
    )
    @pytest.mark.asyncio
    async def test_discovery_event_unknown_ignores_when_flag_false(
        self,
        eebus_test_env: dict,
    ):
        """[Phase 3A] accept_unknown_ems=false: unknown discovered EGs are logged and ignored."""
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(
            control_service_servicer,
            cs_lpc_control_servicer,
            cs_lpc_control_server,
            everest_core,
            expected_register_ski_count=1,
        )
        assert probe.registered_skis_at_boot == [SKI_A]

        control_service_servicer.command_queues["SubscribeDiscoveryEvents"].response_queue.put_nowait(
            _discovery_event(SKI_UNKNOWN, is_trusted=False)
        )
        await _assert_no_register_remote_ski(control_service_servicer, window_s=2.0)

    # --- Test 6 --------------------------------------------------------------

    @pytest.mark.everest_core_config("config-test-eebus-module-001.yaml")
    @pytest.mark.everest_config_adaptions(
        EebusModuleConfigStrategy({
            "eebus_ems_ski_allowlist": f"{SKI_A},{SKI_B}",
        })
    )
    @pytest.mark.asyncio
    async def test_active_ems_connection(
        self,
        eebus_test_env: dict,
    ):
        """[Phase 3A] LpcUseCaseHandler connects the first-seen data-event SKI; later data events from other
        SKIs must be ignored (no downstream ConsumptionLimit read, no external limit update)."""
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(
            control_service_servicer,
            cs_lpc_control_servicer,
            cs_lpc_control_server,
            everest_core,
            expected_register_ski_count=2,
        )

        # Heartbeat from SKI-A connects the active EG. Not a ConsumptionLimit-triggering
        # event, so no response pre-seed needed here.
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(
            _usecase_event(SKI_A, "DataUpdateHeartbeat")
        )

        # DataUpdateLimit from SKI-B — must be ignored (connection is to SKI-A). If the module
        # did NOT ignore it, it would call cs_lpc_control ConsumptionLimit to re-read.
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(
            _usecase_event(SKI_B, "DataUpdateLimit")
        )

        try:
            req = await asyncio.wait_for(
                cs_lpc_control_servicer.command_queues["ConsumptionLimit"].request_queue.get(),
                timeout=2.0,
            )
            pytest.fail(
                f"ConsumptionLimit was read after a DataUpdateLimit from the non-active SKI "
                f"(remote_ski field: {getattr(req, 'remote_ski', '<n/a>')}); the connected EG should have dropped it."
            )
        except asyncio.TimeoutError:
            pass  # expected

        # Confirm the connected EG still accepts events from SKI-A: a DataUpdateLimit from SKI-A
        # should drive a ConsumptionLimit read through the stub.
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(
            _usecase_event(SKI_A, "DataUpdateLimit")
        )
        req = await async_get(
            cs_lpc_control_servicer.command_queues["ConsumptionLimit"].request_queue, timeout=5
        )
        assert req is not None
        cs_lpc_control_servicer.command_queues["ConsumptionLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.ConsumptionLimitResponse(
                load_limit=common_types_pb2.LoadLimit(is_active=False, value=0),
            )
        )

    # --- Test 7 --------------------------------------------------------------

    @pytest.mark.everest_core_config("config-test-eebus-module-001.yaml")
    @pytest.mark.everest_config_adaptions(
        EebusModuleConfigStrategy({
            "eebus_ems_ski_allowlist": f"{SKI_A},{SKI_B}",
        })
    )
    @pytest.mark.asyncio
    async def test_active_ems_failover_through_failsafe(
        self,
        eebus_test_env: dict,
    ):
        """[Phase 3A] When the active EG goes quiet and the heartbeat timeout fires, the module enters
        Failsafe which clears the connected EG. A different allowlisted EG can then take over."""
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(
            control_service_servicer,
            cs_lpc_control_servicer,
            cs_lpc_control_server,
            everest_core,
            expected_register_ski_count=2,
        )

        # Drive Init -> UnlimitedControlled via SKI-A (connects SKI-A).
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(
            _usecase_event(SKI_A, "DataUpdateHeartbeat")
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(
            _usecase_event(SKI_A, "DataUpdateLimit")
        )
        req = await async_get(
            cs_lpc_control_servicer.command_queues["ConsumptionLimit"].request_queue, timeout=5
        )
        assert req is not None
        cs_lpc_control_servicer.command_queues["ConsumptionLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.ConsumptionLimitResponse(
                load_limit=common_types_pb2.LoadLimit(is_active=False, value=0),
            )
        )
        # Drain the UnlimitedControlled transition limit the module emits.
        await async_get(probe.external_limits_queue, timeout=5)

        # No further heartbeats -> the 120 s heartbeat timeout must drive the module into
        # Failsafe, which clears the active-EMS connected EG.
        test_data_failsafe = TestDataFailsafe()
        failsafe_limits = test_data_failsafe.get_expected_failsafe_limits(4200)
        limits = await async_wait_for(probe.external_limits_queue, failsafe_limits, timeout=140)

        # After Failsafe, SKI-B (different EG) heartbeats. The connected EG is unset, so SKI-B becomes the new active EG.
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(
            _usecase_event(SKI_B, "DataUpdateHeartbeat")
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(
            _usecase_event(SKI_B, "DataUpdateLimit")
        )

        req = await async_get(
            cs_lpc_control_servicer.command_queues["ConsumptionLimit"].request_queue, timeout=10
        )
        assert req is not None
        # Return an active limit so the module transitions Failsafe -> Limited.
        test_data_active_load_limit = TestDataActiveLoadLimit()
        cs_lpc_control_servicer.command_queues["ConsumptionLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.ConsumptionLimitResponse(
                load_limit=test_data_active_load_limit.get_load_limit(),
            )
        )

        expected_limits = test_data_active_load_limit.get_expected_external_limits()
        limits = await async_wait_for(probe.external_limits_queue, expected_limits, timeout=10)

