# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
"""End-to-end tests for OCPP 2.0.1 ↔ ISO 15118 charging schedule handoff (K15).

Covers EVerest#1199: the Charging Station receives a CSMS TxProfile, translates it
into an SAScheduleList for the EV, and reports the EV-selected schedule back via
NotifyEVChargingScheduleRequest.
"""

import logging
import pytest

from everest.testing.ocpp_utils.central_system import CentralSystem
from everest.testing.core_utils.controller.test_controller_interface import TestController

from ocpp.v201 import call as call201
from ocpp.v201.datatypes import (
    ChargingProfileType,
    ChargingScheduleType,
    ChargingSchedulePeriodType,
)
from ocpp.v201.enums import (
    ChargingProfileKindType,
    ChargingProfilePurposeType,
    ChargingRateUnitType,
)

from test_sets.everest_test_utils import *
from everest.testing.ocpp_utils.charge_point_utils import (
    wait_for_and_validate,
    TestUtility,
)
from everest.testing.ocpp_utils.charge_point_v201 import ChargePoint201

from everest_test_utils import *
from everest.testing.ocpp_utils.fixtures import *

log = logging.getLogger("SmartChargingHlcTest")


def _tx_profile_one_schedule(transaction_id: str) -> ChargingProfileType:
    return ChargingProfileType(
        id=1,
        stack_level=0,
        charging_profile_purpose=ChargingProfilePurposeType.tx_profile,
        charging_profile_kind=ChargingProfileKindType.absolute,
        transaction_id=transaction_id,
        charging_schedule=[
            ChargingScheduleType(
                id=100,
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriodType(start_period=0, limit=16),
                ],
            )
        ],
    )


@pytest.mark.asyncio
@pytest.mark.ocpp_version("ocpp2.0.1")
@pytest.mark.xdist_group(name="ISO15118")
class TestSmartChargingHlcOcpp201:

    @pytest.mark.skip(
        "Requires ISO 15118-2 schedule handoff to be fully wired end-to-end on the SIL "
        "(EvseV2G bundle → SAScheduleList happy path). Shell kept for the first test once "
        "the SIL integration is in place."
    )
    @pytest.mark.everest_core_config("everest-config-ocpp201-sil-dc-d20.yaml")
    async def test_tx_profile_flows_through_to_ev_and_back(
        self,
        request,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
        test_controller: TestController,
        test_config,
        test_utility: TestUtility,
    ):
        """Happy path: EV → NotifyEVChargingNeeds → CSMS Accepted → SetChargingProfile(TxProfile)
        → ChargeParameterDiscoveryRes(Finished, SAScheduleList) → EV PowerDeliveryReq →
        NotifyEVChargingScheduleRequest (K15.FR.07/10/19)."""

        test_controller.plug_in_dc_iso()

        # Expect NotifyEVChargingNeeds from the station.
        assert await wait_for_and_validate(
            test_utility,
            charge_point,
            "NotifyEVChargingNeeds",
            None,
            lambda _meta, _msg, _exp: True,
        )

        # CSMS sends TxProfile for the open transaction.
        tx_id = charge_point.current_transaction_id  # adapt to whichever helper exposes it
        profile = _tx_profile_one_schedule(transaction_id=tx_id)
        await charge_point.set_charging_profile_req(
            call201.SetChargingProfile(evse_id=1, charging_profile=profile)
        )

        # Station should eventually send NotifyEVChargingSchedule back once the EV commits.
        assert await wait_for_and_validate(
            test_utility,
            charge_point,
            "NotifyEVChargingSchedule",
            None,
            lambda _meta, _msg, _exp: True,
        )

    @pytest.mark.skip(
        "Requires ISO 15118-2 HLC session on SIL. FR.17 rejection path is unit-tested "
        "in libocpp SmartChargingTest; E2E integration kept for SIL-enabled runs."
    )
    @pytest.mark.everest_core_config("everest-config-ocpp201-sil-dc-d20.yaml")
    async def test_set_charging_profile_before_notify_rejected_invalid_message_seq(
        self,
        request,
        central_system: CentralSystem,
        charge_point: ChargePoint201,
        test_controller: TestController,
        test_config,
        test_utility: TestUtility,
    ):
        """K15.FR.17: CSMS sending a TxProfile before the CS has sent NotifyEVChargingNeeds
        for this transaction should return SetChargingProfileResponse(Rejected, InvalidMessageSeq).
        """
        test_controller.plug_in()  # plug in *without* starting an HLC session
        # Start an OCPP transaction directly, without letting ISO 15118 get as far as
        # publishing charging_needs. Then push a TxProfile; expect Rejected.
        # (Test body to be fleshed out once SIL fixtures expose this phase.)
        pass
