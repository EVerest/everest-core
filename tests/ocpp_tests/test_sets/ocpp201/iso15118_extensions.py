# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import logging
import pytest

from everest.testing.ocpp_utils.central_system import CentralSystem
from everest.testing.core_utils.controller.test_controller_interface import TestController

from ocpp.v201 import call as call201

from everest.testing.core_utils._configuration.libocpp_configuration_helper import (
    GenericOCPP201ConfigAdjustment,
)

from test_sets.everest_test_utils import *  # Needs to be before the datatypes below since it overrides the v201 Action enum with the v16 one
from everest.testing.ocpp_utils.charge_point_utils import (
    wait_for_and_validate,
    TestUtility,
)
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201

from everest_test_utils import *
from everest.testing.ocpp_utils.fixtures import *

log = logging.getLogger("iso15118CertificateManagementTest")

async def await_mock_called(mock):
    while not mock.call_count:
        await asyncio.sleep(0.1)


"""
TODO(ioan):
1. start evcore with proper iso config
2. a) start a charging session with iso-d20
2. b) start a charging session with iso-d2
3. wait for notifyevcharging needs to be send to CSMS

NOTE: look in 'ocpp16/plug_and_charge_tests.py'
"""

def validate_notify_ev_charging_needs(meta_data, msg, exp_payload):
    return True

@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
class TestIso15118ExtenstionsOcppIntegration:    
    
    @pytest.mark.everest_core_config("everest-config-sil-dc-d20.yaml")
    async def test_charge_params_sent_dc_d20(
        self,
        request,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
        test_controller: TestController,
        test_config,
        test_utility: TestUtility,
    ):
        await asyncio.sleep(3)
        test_controller.plug_in_ac_iso()

        assert await wait_for_and_validate(
            test_utility,
            charge_point,
            "NotifyEVChargingNeeds",
            call201.NotifyEVChargingNeedsPayload(
                evse_id="1",
                charging_needs=None,
                custom_data=None
            ),
            validate_notify_ev_charging_needs,
        )

        test_utility.messages.clear()
        test_controller.plug_out_iso()

