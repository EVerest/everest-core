# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from datetime import datetime, timedelta, timezone

from ocpp.v16.enums import GetCompositeScheduleStatus
from ocpp.v16.datatypes import *
from ocpp.v16 import call, call_result


def abs_req1_test1():
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
                    ChargingSchedulePeriod(start_period=0, limit=10, number_phases=3)
                ],
            ),
        ),
    )


def abs_req2_test1():
    return call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=2,
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
                    ChargingSchedulePeriod(start_period=0, limit=6, number_phases=3),
                    ChargingSchedulePeriod(start_period=60, limit=10, number_phases=3),
                    ChargingSchedulePeriod(start_period=120, limit=8, number_phases=3),
                    ChargingSchedulePeriod(start_period=180, limit=25, number_phases=3),
                    ChargingSchedulePeriod(start_period=260, limit=8, number_phases=3),
                ],
            ),
        ),
    )


def abs_req2_test_con0():
    return call.SetChargingProfilePayload(
        connector_id=0,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=2,
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
                    ChargingSchedulePeriod(start_period=0, limit=20)
                ],
            ),
        ),
    )


def abs_req3_test1():
    return call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=3,
            transaction_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                duration=240,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=8),
                    ChargingSchedulePeriod(start_period=50, limit=11),
                    ChargingSchedulePeriod(start_period=140, limit=16),
                    ChargingSchedulePeriod(start_period=200, limit=6),
                    ChargingSchedulePeriod(start_period=240, limit=12),
                ],
            ),
        ),
    )


def abs_exp_test1(passed_seconds):
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=400,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=8, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=50 - passed_seconds, limit=10, number_phases=3
                ),
                ChargingSchedulePeriod(
                    start_period=200 - passed_seconds, limit=6, number_phases=3
                ),
                ChargingSchedulePeriod(
                    start_period=240 - passed_seconds, limit=10, number_phases=3
                ),
                ChargingSchedulePeriod(
                    start_period=260 - passed_seconds, limit=8, number_phases=3
                ),
                ChargingSchedulePeriod(
                    start_period=300 - passed_seconds, limit=10, number_phases=3
                ),
            ],
        ),
    )


def abs_req1_test2():
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
                        start_period=0, limit=6, number_phases=3),
                    ChargingSchedulePeriod(
                        start_period=60, limit=10, number_phases=3),
                    ChargingSchedulePeriod(
                        start_period=120, limit=8, number_phases=3),
                ],
            ),
        ),
    )


def abs_exp_test2(passed_seconds):
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=300,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=6, number_phases=3),
                ChargingSchedulePeriod(
                    start_period=60 - passed_seconds, limit=10, number_phases=3
                ),
                ChargingSchedulePeriod(
                    start_period=120 - passed_seconds, limit=8, number_phases=3
                ),
            ],
        ),
    )


def abs_req1_test3():
    return call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=3,
            transaction_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                duration=260,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=6),
                    ChargingSchedulePeriod(start_period=60, limit=10),
                    ChargingSchedulePeriod(start_period=120, limit=8),
                ],
            ),
        ),
    )


def abs_exp_test3(passed_seconds):
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=300,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=6),
                ChargingSchedulePeriod(
                    start_period=60 - passed_seconds, limit=10
                ),
                ChargingSchedulePeriod(
                    start_period=120 - passed_seconds, limit=8
                ),
                ChargingSchedulePeriod(
                    start_period=260 - passed_seconds, limit=48
                ),
            ],
        ),
    )


def abs_req1_test5():
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
                duration=400,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=6),
                    ChargingSchedulePeriod(start_period=50, limit=5),
                    ChargingSchedulePeriod(start_period=100, limit=8),
                    ChargingSchedulePeriod(start_period=200, limit=10),
                ],
            ),
        ),
    )


def abs_req2_test5():
    return call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=2,
            stack_level=1,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                duration=150,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=7),
                    ChargingSchedulePeriod(start_period=100, limit=9),
                ],
            ),
        ),
    )


def abs_exp_test5_1(passed_seconds):
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=350,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=7),
                ChargingSchedulePeriod(
                    start_period=100 - passed_seconds, limit=9
                ),
                ChargingSchedulePeriod(
                    start_period=150 - passed_seconds, limit=8
                ),
                ChargingSchedulePeriod(
                    start_period=200 - passed_seconds, limit=10
                ),
            ],
        ),
    )


def abs_exp_test5_2(passed_seconds):
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=550,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=7),
                ChargingSchedulePeriod(
                    start_period=100 - passed_seconds, limit=9
                ),
                ChargingSchedulePeriod(
                    start_period=150 - passed_seconds, limit=8
                ),
                ChargingSchedulePeriod(
                    start_period=200 - passed_seconds, limit=10
                ),
                ChargingSchedulePeriod(
                    start_period=400 - passed_seconds, limit=48
                ),
            ],
        ),
    )


def abs_req1_test6():
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
                duration=800,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=18),
                    ChargingSchedulePeriod(start_period=50, limit=24),
                    ChargingSchedulePeriod(start_period=100, limit=14),
                    ChargingSchedulePeriod(start_period=200, limit=24),
                ],
            ),
        ),
    )


def abs_req2_test6():
    return call.SetChargingProfilePayload(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=2,
            stack_level=1,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                duration=400,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=16),
                    ChargingSchedulePeriod(start_period=100, limit=20),
                ],
            ),
        ),
    )


def abs_req3_test6():
    return call.SetChargingProfilePayload(
        connector_id=2,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=3,
            stack_level=1,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.absolute,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) + timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                duration=500,
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=10),
                    ChargingSchedulePeriod(start_period=100, limit=16),
                ],
            ),
        ),
    )


def abs_exp_test6_con0():
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=0,
        charging_schedule=ChargingSchedule(
            duration=700,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=18),
                ChargingSchedulePeriod(
                    start_period=50, limit=24),
                ChargingSchedulePeriod(
                    start_period=100, limit=14),
                ChargingSchedulePeriod(
                    start_period=200, limit=24),
            ],
        ),
    )


def abs_exp_test6_con1(passed_seconds):
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=900,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=16),
                ChargingSchedulePeriod(
                    start_period=100 - passed_seconds, limit=14
                ),
                ChargingSchedulePeriod(
                    start_period=200 - passed_seconds, limit=20
                ),
                ChargingSchedulePeriod(
                    start_period=400 - passed_seconds, limit=24
                ),
                ChargingSchedulePeriod(
                    start_period=800 - passed_seconds, limit=48
                ),
            ],
        ),
    )


def abs_exp_test6_con2(passed_seconds):
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=2,
        charging_schedule=ChargingSchedule(
            duration=400,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=10),
                ChargingSchedulePeriod(
                    start_period=100 - passed_seconds, limit=14
                ),
                ChargingSchedulePeriod(
                    start_period=200 - passed_seconds, limit=16
                ),
            ],
        ),
    )


def abs_exp_test1_con0(passed_seconds):
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=400,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=10)
            ],
        ),
    )


def abs_exp_test2_con0(passed_seconds):
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=400,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=20)
            ],
        ),
    )


def abs_exp_test3_con0(passed_seconds):
    return call_result.GetCompositeSchedulePayload(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=90,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(
                    start_period=0, limit=48)
            ],
        ),
    )
