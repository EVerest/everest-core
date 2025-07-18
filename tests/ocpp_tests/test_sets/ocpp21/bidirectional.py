# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
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
from ocpp.v21.enums import (Action, ConnectorStatusEnumType, IdTokenType, IdTokenTypeEnum, AuthorizationStatusEnumType, EnergyTransferModeEnumType)
from validations import validate_status_notification_201
from everest.testing.core_utils._configuration.libocpp_configuration_helper import GenericOCPP2XConfigAdjustment
from everest.testing.ocpp_utils.charge_point_utils import wait_for_and_validate, TestUtility
# fmt: on

log = logging.getLogger("bidirectionalTest")


@pytest.mark.asyncio
@pytest.mark.xdist_group(name="ISO15118")
@pytest.mark.ocpp_version("ocpp2.1")
@pytest.mark.everest_core_config("everest-config-ocpp201-sil-dc-d20.yaml")
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
async def test_q01(
    central_system_v21: CentralSystem,
    test_controller: TestController,
    test_utility: TestUtility,
):
    """
    Q01
    ...
    """

    log.info(
        "##################### Q01: V2X Authorization #################"
    )
    id_token = IdTokenType(id_token="8BADF00D", type=IdTokenTypeEnum.iso14443)

    test_controller.start()
    charge_point_v21 = await central_system_v21.wait_for_chargepoint(wait_for_bootnotification=True)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "StatusNotification",
        call21.StatusNotification(
            1,  ConnectorStatusEnumType.available, 1, datetime.now().isoformat()
        ),
        validate_status_notification_201,
    )
    test_controller.plug_in_dc_iso()

    @on(Action.authorize)
    def on_authorize(**kwargs):
        msg = call21.Authorize(**kwargs)
        if msg.id_token is not None:
            return call_result21.Authorize(
                id_token_info=IdTokenInfoType(
                    status=AuthorizationStatusEnumType.accepted,
                    group_id_token=IdTokenType(
                        id_token="123", type=IdTokenTypeEnum.central
                    ),
                ),
                allowed_energy_transfer=[EnergyTransferModeEnumType.dc_bpt]
            )
        else:
            return call_result21.TransactionEvent()

    setattr(charge_point_v21, "on_authorize", on_authorize)
    central_system_v21.chargepoint.route_map = create_route_map(
        central_system_v21.chargepoint
    )
    test_controller.swipe(id_token.id_token)

    assert await wait_for_and_validate(
        test_utility,
        charge_point_v21,
        "NotifyEVChargingNeeds",
        call21.NotifyEVChargingNeeds(
            evse_id="1",
            charging_needs=None,
            custom_data=None
        )
    )
