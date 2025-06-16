# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
import asyncio
from datetime import datetime, timezone

import traceback
# fmt: off
import logging

from everest.testing.core_utils.controller.test_controller_interface import TestController

from ocpp.v21 import call as call21
from ocpp.v21 import call_result as call_result21
from ocpp.v21.enums import *
from ocpp.v21.datatypes import *
from ocpp.routing import on, create_route_map
from everest.testing.ocpp_utils.fixtures import *
from everest_test_utils import * # Needs to be before the datatypes below since it overrides the v21 Action enum with the v16 one
from ocpp.v21.enums import (Action, SetVariableStatusEnumType, ConnectorStatusEnumType,GetVariableStatusEnumType)
from validations import validate_status_notification_201, validate_notify_report_data_201, wait_for_callerror_and_validate
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP2XConfigAdjustment
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility, OcppTestConfiguration, ValidationMode
from everest.testing.ocpp_utils.charge_point_v21 import ChargePoint21
# fmt: on

log = logging.getLogger("provisioningTest")


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
async def test_cold_boot_01(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    B01.FR.01
    ...
    """

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    try:
        # expect StatusNotification with status available
        assert await wait_for_and_validate(
            test_utility,
            charge_point_v21,
            "StatusNotification",
            call21.StatusNotification(
                1,  ConnectorStatusEnumType.available, 1, datetime.now().isoformat()
            ),
            validate_status_notification_201,
        )
    except Exception as e:
        traceback.print_exc()
        logging.critical(e)

    # TOOD(piet): Check configured HeartbeatInterval of BootNotificationResponse


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
async def test_cold_boot_pending_01(
    test_config: OcppTestConfiguration,
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):

    @on(Action.boot_notification)
    def on_boot_notification_pending(**kwargs):
        return call_result21.BootNotification(
            current_time=datetime.now().isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.pending,
        )

    @on(Action.boot_notification)
    def on_boot_notification_accepted(**kwargs):
        return call_result21.BootNotification(
            current_time=datetime.now(timezone.utc).isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.accepted,
        )

    test_utility.forbidden_actions.append("SecurityEventNotification")

    central_system_v21.function_overrides.append(
        ("on_boot_notification", on_boot_notification_pending)
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    setattr(charge_point_v21, "on_boot_notification",
            on_boot_notification_accepted)
    central_system_v21.chargepoint.route_map = create_route_map(
        central_system_v21.chargepoint
    )

    assert await wait_for_and_validate(
        test_utility, charge_point_v21, "BootNotification", {}
    )

    test_utility.forbidden_actions.clear()

    test_controller.plug_in()

    assert await wait_for_and_validate(
        test_utility, charge_point_v21, "SecurityEventNotification", {}
    )


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
async def test_cold_boot_rejected_01(
    test_config: OcppTestConfiguration,
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):

    @on(Action.boot_notification)
    def on_boot_notification_pending(**kwargs):
        return call_result21.BootNotification(
            current_time=datetime.now().isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.rejected,
        )

    @on(Action.boot_notification)
    def on_boot_notification_accepted(**kwargs):
        return call_result21.BootNotification(
            current_time=datetime.now(timezone.utc).isoformat(),
            interval=5,
            status=RegistrationStatusEnumType.accepted,
        )

    central_system_v21.function_overrides.append(
        ("on_boot_notification", on_boot_notification_pending)
    )

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint()

    setattr(charge_point_v21, "on_boot_notification",
            on_boot_notification_accepted)
    central_system_v21.chargepoint.route_map = create_route_map(
        central_system_v21.chargepoint
    )

    assert await wait_for_and_validate(
        test_utility, charge_point_v21, "BootNotification", {}
    )
