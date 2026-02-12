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

from fixtures.eebus_module_test import eebus_test_env, EebusTestProbeModule
from fixtures.grpc_testing_server import control_service_server, control_service_servicer, cs_lpc_control_server, cs_lpc_control_servicer
from helpers.import_helpers import insert_dir_to_sys_path
from helpers.async_helpers import async_get

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


async def perform_eebus_handshake(control_service_servicer: ControlServiceServicer, cs_lpc_control_servicer: CsLpcControlServicer, cs_lpc_control_server: CsLpcControlServer, everest_core: EverestCore) -> EebusTestProbeModule:
    """This helper function handles the complete EEBUS module init handshake and returns a started probe module."""

    # Handle SetConfig
    control_service_servicer.command_queues["SetConfig"].response_queue.put_nowait(
        control_service_messages_pb2.EmptyResponse()
    )
    req = await async_get(control_service_servicer.command_queues["SetConfig"].request_queue, timeout=10)
    assert req is not None
    assert req.port == 4715

    # Start the probe module
    probe = EebusTestProbeModule(everest_core.get_runtime_session())
    probe.start()

    # Handle StartSetup
    req = await async_get(control_service_servicer.command_queues["StartSetup"].request_queue, timeout=5)
    control_service_servicer.command_queues["StartSetup"].response_queue.put_nowait(
        control_service_messages_pb2.EmptyResponse()
    )

    # Handle RegisterRemoteSki
    req = await async_get(control_service_servicer.command_queues["RegisterRemoteSki"].request_queue, timeout=5)
    control_service_servicer.command_queues["RegisterRemoteSki"].response_queue.put_nowait(
        control_service_messages_pb2.EmptyResponse()
    )

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
    return probe


@pytest.mark.eebus_rpc_port(50051)
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
        limits = await async_get(probe.external_limits_queue, timeout=2)

        assert test_data.get_expected_external_limits() == limits
        test_data.run_additional_assertions(limits)

    @pytest.mark.asyncio
    async def test_update_failsafe_duration_minimum(
        self,
        eebus_test_env: dict,
    ):
        """
        This test verifies that the failsafe duration minimum can be updated.
        """
        everest_core = eebus_test_env["everest_core"]
        control_service_servicer = eebus_test_env["control_service_servicer"]
        cs_lpc_control_servicer = eebus_test_env["cs_lpc_control_servicer"]
        cs_lpc_control_server = eebus_test_env["cs_lpc_control_server"]

        probe = await perform_eebus_handshake(control_service_servicer, cs_lpc_control_servicer, cs_lpc_control_server, everest_core)

        # Simulate a DataUpdateFailsafeDurationMinimum event
        new_failsafe_duration_nanoseconds = 5 * 1000 * 1000 * 1000 # 5 seconds
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

        # The module should now ask for the failsafe duration minimum
        req = await async_get(cs_lpc_control_servicer.command_queues["FailsafeDurationMinimum"].request_queue, timeout=5)
        res = cs_lpc_messages_pb2.FailsafeDurationMinimumResponse(
            duration_nanoseconds=new_failsafe_duration_nanoseconds,
            is_changeable=True,
        )
        cs_lpc_control_servicer.command_queues["FailsafeDurationMinimum"].response_queue.put_nowait(res)

        # Trigger failsafe state to check if the new duration is applied
        # Simulate missing heartbeat
        test_data_failsafe = TestDataFailsafe()
        failsafe_limits = test_data_failsafe.get_expected_failsafe_limits(4200) # this is the config default
        limits = await async_get(probe.external_limits_queue, timeout=65) # Wait for 60s + buffer
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

        await asyncio.sleep(55)

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

        await asyncio.sleep(55)

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

        # Now wait for the new failsafe duration to expire
        unlimited_autonomous_limits = test_data_failsafe.get_expected_unlimited_autonomous_limits()
        limits = await async_get(probe.external_limits_queue, timeout=20) # timeout after failsafe entry is 120s after duration minimum
        assert unlimited_autonomous_limits == limits

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

        # Trigger failsafe state to check if the new limit is applied
        # Simulate missing heartbeat
        test_data_failsafe = TestDataFailsafe()
        failsafe_limits = test_data_failsafe.get_expected_failsafe_limits(new_failsafe_control_limit)
        limits = await async_get(probe.external_limits_queue, timeout=65)
        assert failsafe_limits == limits

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

        # Simulate missing heartbeat to enter failsafe state
        test_data_failsafe = TestDataFailsafe()
        failsafe_limits = test_data_failsafe.get_expected_failsafe_limits(4200)
        limits = await async_get(probe.external_limits_queue, timeout=65) # Wait for 60s + buffer
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

        # Simulate missing heartbeat to enter failsafe state
        test_data_failsafe = TestDataFailsafe()
        failsafe_limits = test_data_failsafe.get_expected_failsafe_limits(4200)
        limits = await async_get(probe.external_limits_queue, timeout=65) # Wait for 60s + buffer
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
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        await asyncio.sleep(50)

        # Simulate a new DataUpdateHeartbeat event
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        await asyncio.sleep(20)

        # Simulate a new DataUpdateHeartbeat event
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        # No limits are sent, so the module should transition from Init to UnlimitedAutonomous
        # The limit would be the same as deactivated since there are no limits
        test_data_not_active_load_limit = TestDataNotActiveLoadLimit()
        unlimited_autonomous_limits = test_data_not_active_load_limit.get_expected_external_limits()
        limits = await async_get(probe.external_limits_queue, timeout=125)
        assert unlimited_autonomous_limits == limits

