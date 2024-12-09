# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest_asyncio

# fmt: off
import logging
from copy import deepcopy
from typing import Dict
from unittest.mock import Mock, call as mock_call
import json
import time
import pytest

from everest.testing.core_utils.common import Requirement
from everest.testing.ocpp_utils.central_system import CentralSystem

from test_sets.everest_test_utils import *  # Needs to be before the datatypes below since it overrides the v201 Action enum with the v16 one
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201
from everest.testing.core_utils.probe_module import ProbeModule
from everest.testing.core_utils import EverestConfigAdjustmentStrategy

log = logging.getLogger("ocpp201DataTransferTest")

async def await_mock_called(mock):
    while not mock.call_count:
        await asyncio.sleep(0.1)

# FIXME: redefine probe_module and chargepoint_with_pm here until the ones in conftest.py are fixed

@pytest.fixture
def probe_module(started_test_controller, everest_core) -> ProbeModule:
    # initiate the probe module, connecting to the same runtime session the test controller started
    module = ProbeModule(everest_core.get_runtime_session())

    return module

@pytest_asyncio.fixture
async def chargepoint_with_pm(central_system: CentralSystem, probe_module: ProbeModule):
    """Fixture for ChargePoint16. Requires central_system_v201 and test_controller. Starts test_controller immediately
    """
    probe_module.start()
    await probe_module.wait_to_be_ready()

    # wait for libocpp to go online
    cp = await central_system.wait_for_chargepoint()
    yield cp
    await cp.stop()

class ProbeModuleDataTransferConfigurationAdjustment(EverestConfigAdjustmentStrategy):
    def adjust_everest_configuration(self, everest_config: Dict):
        adjusted_config = deepcopy(everest_config)

        adjusted_config["active_modules"]["ocpp"]["connections"]["data_transfer"] = [{"module_id": "probe", "implementation_id": "ProbeModuleDataTransfer"}]

        return adjusted_config

@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.everest_core_config("everest-config-ocpp201-data-transfer.yaml")
@pytest.mark.inject_csms_mock
class TestOcpp201DataTransferIntegration:
    """
    Integration tests for the OCPP201 Module's implementation of the P-test cases (data transfer)
    Uses the probe module and a mock CSMS.
    """

    @pytest.mark.parametrize("response_status",
                             ["Accepted", "Rejected", "UnknownMessageId", "UnknownVendorId"],
                             ids=["successful", "failed", "unknown_message_id", "unknown_vendor_id"])
    @pytest.mark.parametrize("message_id",
                             ["message123", None],
                             ids=["with_msg_id", "no_msg_id"])
    @pytest.mark.parametrize("data",
                             ["string_data", 42, 1.2345, False, None],
                             ids=["string_data", "int_data", "float_data", "bool_data", "no_data"])
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleDataTransferConfigurationAdjustment())
    @pytest.mark.asyncio
    async def test_p1(self, response_status, message_id, data,  central_system: CentralSystem, probe_module):
        """
        Use case P01: Data transfer to the Charging Station
        """
        probe_module_mock_fn = Mock()
        probe_module_mock_fn.side_effect = [{
            "status": response_status,
            "data": json.dumps("response123")
        }]
        probe_module.implement_command("ProbeModuleDataTransfer", "data_transfer", probe_module_mock_fn)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        # wait for libocpp to go online
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        data_transfer_result: call_result201.DataTransferPayload = await chargepoint_with_pm.data_transfer_req(
            message_id=message_id,
            data=data,
            vendor_id="vendor123"
        )

        assert data_transfer_result == call_result201.DataTransferPayload(
            data="response123",
            status=response_status
        )
        if message_id is None:
            if data is None:
                assert probe_module_mock_fn.mock_calls == [mock_call({
                    "request": {
                        "vendor_id": "vendor123"
                    }
                })]
            else:
                assert probe_module_mock_fn.mock_calls == [mock_call({
                    "request": {
                        "vendor_id": "vendor123",
                        "data": json.dumps(data)
                    }
                })]
        else:
            if data is None:
                assert probe_module_mock_fn.mock_calls == [mock_call({
                    "request": {
                        "message_id": message_id,
                        "vendor_id": "vendor123"
                    }
                })]
            else:
                assert probe_module_mock_fn.mock_calls == [mock_call({
                    "request": {
                        "message_id": message_id,
                        "vendor_id": "vendor123",
                        "data": json.dumps(data)
                    }
                })]

    @pytest.mark.parametrize("response_status",
                             ["Accepted", "Rejected", "UnknownMessageId", "UnknownVendorId"],
                             ids=["successful", "failed", "unknown_message_id", "unknown_vendor_id"])
    @pytest.mark.parametrize("message_id",
                             ["message987", None],
                             ids=["with_msg_id", "no_msg_id"])
    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleDataTransferConfigurationAdjustment())
    @pytest.mark.asyncio
    async def test_p1_json(self, response_status, message_id, central_system: CentralSystem, probe_module):
        """
        Use case P01: Data transfer to the Charging Station
        """
        probe_module_mock_fn = Mock()
        probe_module_mock_fn.side_effect = [{
            "status": response_status,
            "data": "{\"response987\":\"hello\"}"
        }]
        probe_module.implement_command("ProbeModuleDataTransfer", "data_transfer", probe_module_mock_fn)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        # wait for libocpp to go online
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        data_transfer_result: call_result201.DataTransferPayload = await chargepoint_with_pm.data_transfer_req(
            message_id=message_id,
            data={"request987":"hi"},
            vendor_id="vendor123"
        )

        assert data_transfer_result == call_result201.DataTransferPayload(
            data={'response987':'hello'},
            status=response_status
        )
        if message_id is None:
            assert probe_module_mock_fn.mock_calls == [mock_call({
                "request": {
                    "vendor_id": "vendor123",
                    "data": "{\"request987\":\"hi\"}"
                }
            })]
        else:
            assert probe_module_mock_fn.mock_calls == [mock_call({
                "request": {
                    "message_id": message_id,
                    "vendor_id": "vendor123",
                    "data": "{\"request987\":\"hi\"}"
                }
            })]

    @pytest.mark.parametrize("response_status",
                             ["Accepted", "Rejected", "UnknownMessageId", "UnknownVendorId"],
                             ids=["successful", "failed", "unknown_message_id", "unknown_vendor_id"])
    @pytest.mark.parametrize("message_id",
                             ["message123", None],
                             ids=["with_msg_id", "no_msg_id"])
    @pytest.mark.parametrize("data",
                             ["string_data", 42, 1.2345, False, None],
                             ids=["string_data", "int_data", "float_data", "bool_data", "no_data"])
    @pytest.mark.probe_module(
        connections={
            "ocpp_data_transfer": [Requirement(module_id="ocpp", implementation_id="data_transfer")]
        }
    )
    @pytest.mark.asyncio
    async def test_p2(self, response_status, message_id, data, central_system: CentralSystem,
                      chargepoint_with_pm: ChargePoint201, probe_module):
        """
        Use case P02: Data transfer to the CSMS
        """
        central_system.mock.on_data_transfer.side_effect = [
            call_result201.DataTransferPayload(status=response_status, data="response123")
        ]

        response = json.dumps("response123")
        if message_id is None:
            if data is None:
                assert await probe_module.call_command("ocpp_data_transfer", "data_transfer", {
                    "request": {"vendor_id": "vendor123"}
                }) == {"status": response_status, "data": response}
            else:
                assert await probe_module.call_command("ocpp_data_transfer", "data_transfer", {
                    "request": {"vendor_id": "vendor123", "data": json.dumps(data)}
                }) == {"status": response_status, "data": response}
        else:
            if data is None:
                assert await probe_module.call_command("ocpp_data_transfer", "data_transfer", {
                    "request": {"vendor_id": "vendor123", "message_id": message_id}
                }) == {"status": response_status, "data": response}
            else:
                assert await probe_module.call_command("ocpp_data_transfer", "data_transfer", {
                    "request": {"vendor_id": "vendor123", "message_id": message_id, "data": json.dumps(data)}
                }) == {"status": response_status, "data": response}

    @pytest.mark.parametrize("response_status",
                             ["Accepted", "Rejected", "UnknownMessageId", "UnknownVendorId"],
                             ids=["successful", "failed", "unknown_message_id", "unknown_vendor_id"])
    @pytest.mark.parametrize("message_id",
                             ["message987", None],
                             ids=["with_msg_id", "no_msg_id"])
    @pytest.mark.probe_module(
        connections={
            "ocpp_data_transfer": [Requirement(module_id="ocpp", implementation_id="data_transfer")]
        }
    )
    @pytest.mark.asyncio
    async def test_p2_json(self, response_status, message_id, central_system: CentralSystem,
                      chargepoint_with_pm: ChargePoint201, probe_module):
        """
        Use case P02: Data transfer to the CSMS
        """
        central_system.mock.on_data_transfer.side_effect = [
            call_result201.DataTransferPayload(status=response_status, data={'response987':'hello'})
        ]

        if message_id is None:
            assert await probe_module.call_command("ocpp_data_transfer", "data_transfer", {
                "request": {"vendor_id": "vendor123", "data": "{\"request987\":\"hi\"}"}
            }) == {"status": response_status, "data": "{\"response987\":\"hello\"}"}
        else:
            assert await probe_module.call_command("ocpp_data_transfer", "data_transfer", {
                "request": {"vendor_id": "vendor123", "message_id": message_id, "data": "{\"request987\":\"hi\"}"}
            }) == {"status": response_status, "data": "{\"response987\":\"hello\"}"}

    @pytest.mark.asyncio
    async def test_p1_no_callback(self, charge_point: ChargePoint201):
        """
        Use case P01: Data transfer to the Charging Station
        """
        data_transfer_result: call_result201.DataTransferPayload = await charge_point.data_transfer_req(
            message_id="message123",
            data="request123",
            vendor_id="vendor123"
        )

        assert data_transfer_result == call_result201.DataTransferPayload(
            data=None,
            status="UnknownVendorId"
        )

    @pytest.mark.probe_module(
        connections={
            "ocpp_data_transfer": [Requirement(module_id="ocpp", implementation_id="data_transfer")]
        }
    )

    @pytest.mark.probe_module
    @pytest.mark.everest_config_adaptions(ProbeModuleDataTransferConfigurationAdjustment())
    @pytest.mark.asyncio
    @pytest.mark.skip("Fails because callback sleeps for 400s and test case expects response. Check expected behavior")
    async def test_p1_no_response(self,  central_system: CentralSystem, probe_module):
        """
        Use case P01: Data transfer to the Charging Station but Charging Station does not respond
        """

        def data_transfer_side_effect(*args, **kwargs):
            time.sleep(400)
            return call_result201.DataTransferPayload(status="Accepted", data={'response987':'hello'})

        probe_module_mock_fn = Mock()
        probe_module_mock_fn.side_effect = data_transfer_side_effect
        probe_module.implement_command("ProbeModuleDataTransfer", "data_transfer", probe_module_mock_fn)

        probe_module.start()
        await probe_module.wait_to_be_ready()

        # wait for libocpp to go online
        chargepoint_with_pm = await central_system.wait_for_chargepoint()

        data_transfer_result: call_result201.DataTransferPayload = await chargepoint_with_pm.data_transfer_req(
            message_id="message123",
            data="data",
            vendor_id="vendor123"
        )

        assert data_transfer_result == call_result201.DataTransferPayload(
            status="Rejected"
        )
