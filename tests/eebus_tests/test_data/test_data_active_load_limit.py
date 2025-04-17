# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import os, pathlib
import datetime

from .test_data import TestData
from helpers.types import ExternalLimits, ScheduleReqEntry, LimitsReq

from helpers.import_helpers import insert_dir_to_sys_path
insert_dir_to_sys_path(pathlib.Path(os.path.dirname(os.path.realpath(__file__))) / "../grpc_libs/generated")
import common_types.types_pb2 as common_types_pb2


class TestDataActiveLoadLimit(TestData):
    """
    In this scenario, the load limit is active and has no duration.

    This means an external limits with one entry is expected:
    - The entry to activate the limit
    """
    def get_load_limit(self) -> common_types_pb2.LoadLimit:
        return common_types_pb2.LoadLimit(
            duration_nanoseconds=0,
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
                        total_power_W=4200.0,
                    ),
                    limits_to_leaves=LimitsReq(
                        total_power_W=4200.0,
                    ),
                    conversion_efficiency=None,
                    prive_per_kwh=None
                )
            ]
        )
    def run_additional_assertions(self, limits: ExternalLimits):
        assert True
