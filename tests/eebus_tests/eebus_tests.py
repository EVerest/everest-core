# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import logging
import pytest
import os
import pathlib

from everest.testing.core_utils.fixtures import *
from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.controller.everest_test_controller import EverestTestController

from grpc_servicer.control_service_servicer import ControlServiceServicer
from grpc_server.control_service_server import ControlServiceServer
from grpc_servicer.cs_lpc_control_servicer import CsLpcControlServicer
from grpc_server.cs_lpc_control_server import CsLpcControlServer

from fixtures.eebus_module_test import eebus_module_test
from fixtures.grpc_testing_server import control_service_server, control_service_servicer, cs_lpc_control_server, cs_lpc_control_servicer
from helpers.import_helpers import insert_dir_to_sys_path
from prepared_eebus_module_test import PreparedEEBUSModuleTest

from test_data.test_data import TestData
from test_data.test_data_not_active_load_limit import TestDataNotActiveLoadLimit
from test_data.test_data_active_load_limit import TestDataActiveLoadLimit
from test_data.test_data_active_load_limit_with_duration import TestDataActiveLoadLimitWithDuration

insert_dir_to_sys_path(pathlib.Path(os.path.dirname(os.path.realpath(__file__))) / "grpc_libs/generated")
import control_service.messages_pb2 as control_service_messages_pb2
import control_service.types_pb2 as control_service_types_pb2
import usecases.cs.lpc.messages_pb2 as cs_lpc_messages_pb2
import common_types.types_pb2 as common_types_pb2


@pytest.mark.eebus_rpc_port(50051)
@pytest.mark.everest_core_config("config-test-eebus-module-001.yaml")
@pytest.mark.probe_module
class TestEEBUSModule:
    """
    This test class is used to test the EEBUS module.

    It contains the following tests:
    - test_module_init: Test the initialization of the EEBUS module.
    - test_set_load_limit: Test the setting of the load limit, with different test data.
    """
    @pytest.mark.asyncio
    async def test_module_init(
        self,
        control_service_server: ControlServiceServer,
        control_service_servicer: ControlServiceServicer,
        cs_lpc_control_server: CsLpcControlServer,
        cs_lpc_control_servicer: CsLpcControlServicer,
        test_controller: EverestTestController,
        everest_core: EverestCore,
    ):
        """
        Test the initialization of the EEBUS module.

        Furthermore, it tests the initialization of the EVerest instance,
        until it is ready to accecpt load limits.
        """
        test = PreparedEEBUSModuleTest(
            everest_core=everest_core,
            test_controller=test_controller,
            control_service_server=control_service_server,
            control_service_servicer=control_service_servicer,
            cs_lpc_control_server=cs_lpc_control_server,
            cs_lpc_control_servicer=cs_lpc_control_servicer,
            activate_assertions=True,
        )
        assert test._setup == False
        assert test._tear_down == False
        assert test._assertions == True
        await test.setup()
        assert test._setup == True
        assert test._tear_down == False
        test.tear_down()
        assert test._setup == True
        assert test._tear_down == True


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
            )
        ]
    )
    @pytest.mark.asyncio
    async def test_set_load_limit(
        self,
        eebus_module_test: PreparedEEBUSModuleTest,
        control_service_servicer: ControlServiceServicer,
        cs_lpc_control_servicer: CsLpcControlServicer,
        test_data: TestData,
    ):
        """
        This test sets the load limit and checks if the EEBUS module
        sends the correct limits to the probe module.

        This test takes a ready setup / started EVerest with EEBUS
        module and takes a load limit from the test data and sends it
        to the EEBUS module. The EEBUS module should then send the
        limits to the probe module. The test checks if the limits
        sent to the probe module are correct and identical to the
        expected limits from the test data.
        """
        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(
                entity_address=[1]
            ),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="WriteApprovalRequired",
            )
        )

        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        req = cs_lpc_control_servicer.command_queues["PendingConsumptionLimit"].request_queue.get(timeout=5)
        assert req is not None
        res = cs_lpc_messages_pb2.PendingConsumptionLimitResponse(
            load_limits={
                0:  test_data.get_load_limit(),
            }
        )
        cs_lpc_control_servicer.command_queues["PendingConsumptionLimit"].response_queue.put_nowait(res)

        req = cs_lpc_control_servicer.command_queues["ApproveOrDenyConsumptionLimit"].request_queue.get(timeout=5)
        assert req is not None
        assert req.msg_counter == 0
        assert req.approve == True
        assert req.reason == ""
        cs_lpc_control_servicer.command_queues["ApproveOrDenyConsumptionLimit"].response_queue.put_nowait(
            cs_lpc_messages_pb2.ApproveOrDenyConsumptionLimitResponse()
        )

        uc_event = control_service_messages_pb2.SubscribeUseCaseEventsResponse(
            remote_ski="this-is-a-ski-42",
            remote_entity_address=common_types_pb2.EntityAddress(
                entity_address=[1]
            ),
            use_case_event=control_service_types_pb2.UseCaseEvent(
                use_case=control_service_types_pb2.UseCase(
                    actor=control_service_types_pb2.UseCase.ActorType.ControllableSystem,
                    name=control_service_types_pb2.UseCase.NameType.limitationOfPowerConsumption,
                ),
                event="DataUpdateLimit",
            )
        )
        control_service_servicer.command_queues["SubscribeUseCaseEvents"].response_queue.put_nowait(uc_event)

        req = cs_lpc_control_servicer.command_queues["ConsumptionLimit"].request_queue.get(timeout=5)
        assert req is not None
        res = cs_lpc_messages_pb2.ConsumptionLimitResponse(
            load_limit=test_data.get_load_limit(),
        )
        cs_lpc_control_servicer.command_queues["ConsumptionLimit"].response_queue.put_nowait(res)

        limits = eebus_module_test.probe_module.external_limits_queue.get(timeout=5)

        assert test_data.get_expected_external_limits() == limits
        test_data.run_additional_assertions(limits)
