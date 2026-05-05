# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

"""OCPP 2.1 G01: connector AvailabilityState reported via NotifyEventRequest."""

import logging
import pytest

# fmt: off
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP2XConfigAdjustment
from everest.testing.core_utils.controller.test_controller_interface import TestController
from everest.testing.ocpp_utils.charge_point_utils import TestUtility, wait_for_and_validate
from everest.testing.ocpp_utils.fixtures import *
from everest_test_utils import *
from ocpp.v21.enums import ConnectorStatusEnumType
# fmt: on

log = logging.getLogger("availabilityNotifyEventTest")


def _validate_notify_event_for_availability_state(meta_data, msg, exp_payload):
    payload = msg.payload
    for ev in payload.get("eventData", []):
        component = ev.get("component", {})
        variable = ev.get("variable", {})
        if component.get("name") == "Connector" and variable.get("name") == "AvailabilityState":
            assert ev["trigger"] == "Delta"
            assert ev["eventNotificationType"] == "HardWiredNotification"
            assert ev["actualValue"] in {s.value for s in ConnectorStatusEnumType}
            return True
    return False


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config(
    get_everest_config_path_str("everest-config-ocpp201.yaml")
)
@pytest.mark.ocpp_config_adaptions(
    GenericOCPP2XConfigAdjustment(
        [
            (
                OCPP2XConfigVariableIdentifier(
                    "InternalCtrlr", "SupportedOcppVersions", "Actual"
                ),
                "ocpp2.1",
            )
        ]
    )
)
async def test_g01_connector_status_emits_notify_event(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """G01: When the charging station boots up on OCPP 2.1, the connector
    AvailabilityState transition shall be reported via NotifyEventRequest
    (component=Connector, variable=AvailabilityState). The deprecated
    StatusNotificationRequest path must not be used on 2.1+."""

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "NotifyEvent",
        {},
        _validate_notify_event_for_availability_state,
    )
