# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from datetime import datetime, timedelta, timezone

from ocpp.v16.enums import *
from ocpp.v16.datatypes import *
from ocpp.v16 import call, call_result


def comb_req1_test1():
    return call.SetChargingProfile(
        connector_id=0,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.charge_point_max_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) +
                      timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                duration=200,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=10),  # 6900
                    ChargingSchedulePeriod(
                        start_period=80, limit=20, number_phases=1  # 4600
                    ),
                    ChargingSchedulePeriod(
                        start_period=100, limit=20, number_phases=3  # 13800
                    ),
                ],
            ),
        ),
    )


def comb_req2_test1():
    return call.SetChargingProfile(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=2,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) +
                      timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                duration=300,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.watts,
                charging_schedule_period=[
                    ChargingSchedulePeriod(
                        start_period=0, limit=11000, number_phases=3
                    ),
                    ChargingSchedulePeriod(
                        start_period=60, limit=6900, number_phases=1
                    ),
                    ChargingSchedulePeriod(start_period=120, limit=5520),
                    ChargingSchedulePeriod(start_period=180, limit=17250),
                    ChargingSchedulePeriod(start_period=260, limit=5520),
                ],
            ),
        ),
    )


def comb_exp1_test1():
    return call_result.GetCompositeSchedule(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=400,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.watts,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=6900, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=80, limit=4600, number_phases=1),
                ChargingSchedulePeriod(
                    start_period=100, limit=6900, number_phases=1),
                ChargingSchedulePeriod(
                    start_period=120, limit=5520, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=180, limit=13800, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=200, limit=17250, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=260, limit=5520, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=300, limit=33120, number_phases=3),
            ],
        ),
    )


def comb_exp2_test1():
    return call_result.GetCompositeSchedule(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=400,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=10, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=80, limit=20, number_phases=1),
                ChargingSchedulePeriod(
                    start_period=100, limit=30, number_phases=1),
                ChargingSchedulePeriod(
                    start_period=120, limit=8, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=180, limit=20, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=200, limit=25, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=260, limit=8, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=300, limit=48, number_phases=3),
            ],
        ),
    )


def comb_req1_test2():
    return call.SetChargingProfile(
        connector_id=0,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.charge_point_max_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            charging_schedule=ChargingSchedule(
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=10),
                    ChargingSchedulePeriod(
                        start_period=80, limit=20, number_phases=2),
                    ChargingSchedulePeriod(
                        start_period=160, limit=20, number_phases=3),
                ],
            ),
        ),
    )


def comb_exp_test2():
    return call_result.GetCompositeSchedule(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=0,
        charging_schedule=ChargingSchedule(
            duration=400,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=10),
                ChargingSchedulePeriod(
                    start_period=80, limit=20, number_phases=2),
                ChargingSchedulePeriod(
                    start_period=160, limit=20, number_phases=3),
            ],
        ),
    )
