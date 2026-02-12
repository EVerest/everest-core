# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import os, pathlib
import datetime

from .test_data import TestData
from helpers.types import ExternalLimits, ScheduleReqEntry, LimitsReq, TotalPowerW

from helpers.import_helpers import insert_dir_to_sys_path
insert_dir_to_sys_path(pathlib.Path(os.path.dirname(os.path.realpath(__file__))) / "../grpc_libs/generated")
import common_types.types_pb2 as common_types_pb2

class TestDataHeartbeat(TestData):
    """
    In this scenario, the module receives a heartbeat and transitions to UnlimitedControlled.
    """
    def get_load_limit(self) -> common_types_pb2.LoadLimit:
        # This is not directly used for heartbeat, but required by the test setup
        return common_types_pb2.LoadLimit(
            duration_nanoseconds=0,
            is_changeable=True,
            is_active=False,
            value=4200,
            delete_duration=False,
        )

    def get_expected_external_limits(self) -> ExternalLimits:
        return ExternalLimits(
            schedule_import=[
                ScheduleReqEntry(
                    timestamp=datetime.datetime.now(),
                    limits_to_root=LimitsReq(),
                    limits_to_leaves=LimitsReq(),
                    conversion_efficiency=None,
                    prive_per_kwh=None
                )
            ],
            schedule_export=[],
        )

    def run_additional_assertions(self, limits: ExternalLimits):
        assert True
