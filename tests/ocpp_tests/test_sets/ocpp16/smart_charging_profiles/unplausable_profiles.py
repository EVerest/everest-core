# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from datetime import datetime, timedelta, timezone

from ocpp.v16.enums import GetCompositeScheduleStatus
from ocpp.v16.datatypes import *
from ocpp.v16 import call, call_result


def unp_req1_test1():
    return call.SetChargingProfilePayload(
        connector_id=0,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.charge_point_max_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                duration=86400,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=10)
                ],
            ),
        ),
    )


def unp_req2_test1():
    return call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                duration=300,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(
                        start_period=0,
                        limit=6,
                    ),
                    ChargingSchedulePeriod(start_period=60, limit=10),
                    ChargingSchedulePeriod(start_period=120, limit=8),
                ],
            ),
        ),
    )


def unp_req3_test1():
    return call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                duration=300,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(
                        start_period=0,
                        limit=12,
                    ),
                    ChargingSchedulePeriod(start_period=60, limit=10),
                    ChargingSchedulePeriod(start_period=120, limit=6),
                ],
            ),
        ),
    )


def unp_exp_test1():
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=300,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=12, number_phases=3),
                ChargingSchedulePeriod(start_period=60, limit=10, number_phases=3),
                ChargingSchedulePeriod(start_period=120, limit=6, number_phases=3),
            ],
        ),
    )
