# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from unittest.mock import call as mock_call, ANY
import pytest
from everest.testing.core_utils.common import Requirement
from everest.testing.core_utils.controller.test_controller_interface import (
    TestController,
)
from everest.testing.core_utils.probe_module import ProbeModule

from ocpp.routing import create_route_map

from ocpp.v16 import call
from ocpp.v16.enums import *

# fmt: off
from validations import (validate_standard_start_transaction)
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility, OcppTestConfiguration
from everest.testing.ocpp_utils.fixtures import charge_point_v16
from everest.testing.ocpp_utils.central_system import CentralSystem
from everest.testing.ocpp_utils.charge_point_v16 import ChargePoint16
from everest_test_utils import *
# fmt: on


@pytest.mark.asyncio
async def test_call_error_to_transaction_message(
    test_config: OcppTestConfiguration,
    charge_point_v16: ChargePoint16,
    test_utility: TestUtility,
    test_controller: TestController,
):

    setattr(charge_point_v16, "on_start_transaction", None)
    charge_point_v16.route_map = create_route_map(charge_point_v16)

    await charge_point_v16.change_configuration_req(
        key="TransactionMessageAttempts", value="3"
    )

    test_controller.plug_in()

    test_controller.swipe(test_config.authorization_info.valid_id_tag_1)
    # expect authorize.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "Authorize",
        call.Authorize(test_config.authorization_info.valid_id_tag_1),
    )

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransaction(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransaction(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    # expect StartTransaction.req
    assert await wait_for_and_validate(
        test_utility,
        charge_point_v16,
        "StartTransaction",
        call.StartTransaction(
            1, test_config.authorization_info.valid_id_tag_1, 0, ""
        ),
        validate_standard_start_transaction,
    )

    test_utility.messages.clear()
    test_utility.forbidden_actions.append("StartTransaction")

    test_controller.plug_out()

    # expect StopTransaction.req
    assert await wait_for_and_validate(
        test_utility, charge_point_v16, "StopTransaction", {
            "reason": "EVDisconnected"}
    )


async def wait_for_mock_called(mock, call=None, timeout=2):
    async def _await_called():
        while not mock.call_count or (call and call not in mock.mock_calls):
            await asyncio.sleep(0.1)

    await asyncio.wait_for(_await_called(), timeout=timeout)


@pytest.mark.ocpp_version("ocpp1.6")
@pytest.mark.everest_core_config("everest-config-sil-ocpp.yaml")
@pytest.mark.inject_csms_mock
@pytest.mark.probe_module(connections={"ocpp": [Requirement("ocpp", "main")]})
@pytest.mark.asyncio
async def test_security_event_delivery_after_reconnect(
    everest_core, test_controller, central_system: CentralSystem
):
    """Tests A04.FR.02 of OCPP 1.6 Security White Paper"""

    # Setup: Init Probe module, start EVerest and CSMS
    test_controller.start()
    csms_mock = central_system.mock

    probe_module = ProbeModule(everest_core.get_runtime_session())

    probe_module.start()
    await probe_module.wait_to_be_ready()
    await central_system.wait_for_chargepoint()

    # Act: disconnect, send security event
    test_controller.disconnect_websocket()

    csms_mock.on_security_event_notification.reset_mock()
    # Since on boot we expect a count of security events
    await probe_module.call_command(
        "ocpp", "security_event", {
            "type": "SecurityLogWasCleared", "info": "test_info"}
    )

    # Verify: CSMS has not received any event (since offline), reconnect and verify event is received
    await asyncio.sleep(1)
    csms_mock.on_security_event_notification.assert_not_called()

    test_controller.connect_websocket()

    await wait_for_mock_called(
        csms_mock.on_security_event_notification,
        mock_call(tech_info="test_info", timestamp=ANY,
                  type="SecurityLogWasCleared"),
        10,
    )
