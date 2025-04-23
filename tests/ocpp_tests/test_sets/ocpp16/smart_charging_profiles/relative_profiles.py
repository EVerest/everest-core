# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from datetime import datetime, timedelta, timezone

from ocpp.v16.enums import GetCompositeScheduleStatus
from ocpp.v16.datatypes import *
from ocpp.v16 import call, call_result


def rel_req1_test1():
    return call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            transaction_id=1,
            charging_profile_purpose=ChargingProfilePurposeType.tx_profile,
            charging_profile_kind=ChargingProfileKindType.relative,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=16),
                    ChargingSchedulePeriod(start_period=50, limit=20),
                ],
            ),
        ),
    )


def rel_exp_test1():
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=400,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=16, number_phases=3),
                ChargingSchedulePeriod(start_period=50, limit=20, number_phases=3),
            ],
        ),
    )


def rel_req1_test2():
    return call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.relative,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=16),
                    ChargingSchedulePeriod(start_period=100, limit=20),
                ],
            ),
        ),
    )


def rel_req2_test2():
    return call.SetChargingProfilePayload(
        connector_id=0,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=2,
            stack_level=1,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.relative,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                duration=200,
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=10),
                    ChargingSchedulePeriod(start_period=50, limit=6),
                ],
            ),
        ),
    )


def rel_exp_test2():
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=300,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=10, number_phases=3),
                ChargingSchedulePeriod(start_period=50, limit=6, number_phases=3),
                ChargingSchedulePeriod(start_period=200, limit=20, number_phases=3),
            ],
        ),
    )


def rel_req1_test3():
    return call.SetChargingProfilePayload(
        connector_id=0,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=1,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.relative,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                charging_rate_unit=ChargingRateUnitType.watts,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=11000),
                    ChargingSchedulePeriod(start_period=90, limit=22000),
                ],
            ),
        ),
    )


def rel_req2_test3():
    return call.SetChargingProfilePayload(
        connector_id=0,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=2,
            stack_level=2,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.relative,
            valid_from=datetime.now(timezone.utc).isoformat(),
            charging_schedule=ChargingSchedule(
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=32),
                    ChargingSchedulePeriod(start_period=6, limit=20),
                    ChargingSchedulePeriod(start_period=12, limit=8),
                ],
            ),
        ),
    )


def rel_exp_test3():
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=90,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=32, number_phases=3),
                ChargingSchedulePeriod(start_period=6, limit=20, number_phases=3),
                ChargingSchedulePeriod(start_period=12, limit=8, number_phases=3),
            ],
        ),
    )
