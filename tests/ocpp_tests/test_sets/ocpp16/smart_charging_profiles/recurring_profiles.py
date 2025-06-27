# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

from datetime import datetime, timedelta, timezone

from ocpp.v16.enums import *
from ocpp.v16.datatypes import *
from ocpp.v16 import call, call_result


def rec_req1_test1():
    return call.SetChargingProfile(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.recurring,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) +
                      timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=16),
                    ChargingSchedulePeriod(start_period=100, limit=20),
                ],
            ),
        ),
    )


def rec_req1_test2():
    return call.SetChargingProfile(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.recurring,
            recurrency_kind=RecurrencyKind.daily,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) +
                      timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=14),
                    ChargingSchedulePeriod(start_period=5000, limit=16),
                    ChargingSchedulePeriod(start_period=15000, limit=20),
                ],
            ),
        ),
    )


def rec_req2_test2():
    return call.SetChargingProfile(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=2,
            stack_level=1,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.recurring,
            recurrency_kind=RecurrencyKind.daily,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) +
                      timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                start_schedule=datetime.now(timezone.utc).isoformat(),
                duration=86400,
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=10),
                    ChargingSchedulePeriod(start_period=10000, limit=22),
                    ChargingSchedulePeriod(start_period=20000, limit=6),
                ],
            ),
        ),
    )


def rec_exp_test2():
    return call_result.GetCompositeSchedule(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=172800,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=10),
                ChargingSchedulePeriod(start_period=10000, limit=22),
                ChargingSchedulePeriod(start_period=20000, limit=6),
                ChargingSchedulePeriod(start_period=86400, limit=10),
                ChargingSchedulePeriod(start_period=96400, limit=22),
                ChargingSchedulePeriod(start_period=106400, limit=6),
            ],
        ),
    )


def rec_req1_test3():
    return call.SetChargingProfile(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=1,
            stack_level=0,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.recurring,
            recurrency_kind=RecurrencyKind.weekly,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) +
                      timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                start_schedule=datetime.now(timezone.utc).isoformat(),
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=14),
                    ChargingSchedulePeriod(start_period=5000, limit=16),
                    ChargingSchedulePeriod(start_period=15000, limit=20),
                ],
            ),
        ),
    )


def rec_req2_test3():
    return call.SetChargingProfile(
        connector_id=1,
        cs_charging_profiles=ChargingProfile(
            charging_profile_id=2,
            stack_level=1,
            charging_profile_purpose=ChargingProfilePurposeType.tx_default_profile,
            charging_profile_kind=ChargingProfileKindType.recurring,
            recurrency_kind=RecurrencyKind.weekly,
            valid_from=datetime.now(timezone.utc).isoformat(),
            valid_to=(datetime.now(timezone.utc) +
                      timedelta(days=3)).isoformat(),
            charging_schedule=ChargingSchedule(
                start_schedule=datetime.now(timezone.utc).isoformat(),
                duration=86400,
                charging_rate_unit=ChargingRateUnitType.amps,
                charging_schedule_period=[
                    ChargingSchedulePeriod(start_period=0, limit=10),
                    ChargingSchedulePeriod(start_period=10000, limit=22),
                    ChargingSchedulePeriod(start_period=20000, limit=6),
                ],
            ),
        ),
    )


def rec_exp_test3():
    return call_result.GetCompositeSchedule(
        status=GetCompositeScheduleStatus.accepted,
        schedule_start=datetime.now(timezone.utc).isoformat(),
        connector_id=1,
        charging_schedule=ChargingSchedule(
            duration=172800,
            start_schedule=datetime.now(timezone.utc).isoformat(),
            charging_rate_unit=ChargingRateUnitType.amps,
            charging_schedule_period=[
                ChargingSchedulePeriod(start_period=0, limit=10),
                ChargingSchedulePeriod(start_period=10000, limit=22),
                ChargingSchedulePeriod(start_period=20000, limit=6),
                ChargingSchedulePeriod(start_period=86400, limit=20),
            ],
        ),
    )
