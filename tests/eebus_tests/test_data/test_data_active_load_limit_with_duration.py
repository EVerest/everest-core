# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import os, pathlib
import datetime

from .test_data import TestData
from helpers.types import ExternalLimits, ScheduleReqEntry, LimitsReq, TotalPowerW

from helpers.import_helpers import insert_dir_to_sys_path
insert_dir_to_sys_path(pathlib.Path(os.path.dirname(os.path.realpath(__file__))) / "../grpc_libs/generated")
import common_types.types_pb2 as common_types_pb2

def assert_time_delta(time1: datetime.datetime, time2: datetime.datetime, expected_delta: datetime.timedelta):
    delta = time2 - time1
    assert int(delta.total_seconds()) == int(expected_delta.total_seconds())

class TestDataActiveLoadLimitWithDuration(TestData):
    """
    In this scenario, the load limit is active and has a duration.

    This means an external limits with to entries is expected:
    - The first entry to activate the limit
    - The second entry to deactivate the limit

    The difference between the two entries should be 2 hours.
    """
    def get_load_limit(self) -> common_types_pb2.LoadLimit:
        return common_types_pb2.LoadLimit(
            duration_nanoseconds=int(datetime.timedelta(hours=2).total_seconds()) * 1000 * 1000 * 1000, # 2 hours
            is_changeable=True,
            is_active=True,
            value=4200,
            delete_duration=False,
        )

    def get_expected_external_limits(self) -> ExternalLimits:
        return ExternalLimits(
            schedule_import=[
                ScheduleReqEntry(
                    timestamp=datetime.datetime.now(),
                    limits_to_root=LimitsReq(
                        total_power_W=TotalPowerW(
                            source='EEBUS LPC',
                            value=4200.0,
                        ),
                    ),
                    limits_to_leaves=LimitsReq(
                        total_power_W=TotalPowerW(
                            source='EEBUS LPC',
                            value=4200.0,
                        ),
                    ),
                    conversion_efficiency=None,
                    prive_per_kwh=None
                ),
                ScheduleReqEntry(
                    timestamp=datetime.datetime.now() + datetime.timedelta(seconds=2 * 60 * 60),
                    limits_to_root=LimitsReq(),
                    limits_to_leaves=LimitsReq(),
                    conversion_efficiency=None,
                    prive_per_kwh=None
                )
            ],
            schedule_export=[],
        )
    def run_additional_assertions(self, limits: ExternalLimits):
        assert_time_delta(
            limits.schedule_import[0].timestamp,
            limits.schedule_import[1].timestamp,
            datetime.timedelta(hours=2)
        )
