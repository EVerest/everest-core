# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import os, pathlib

from helpers.types import ExternalLimits

from helpers.import_helpers import insert_dir_to_sys_path
insert_dir_to_sys_path(pathlib.Path(os.path.dirname(os.path.realpath(__file__))) / "../grpc_libs/generated")
import common_types.types_pb2 as common_types_pb2

class TestData:
    """
    Abstract class for test data.

    - get_load_limit: Should return the load limit that is sent to the EEBUS module
    - get_expected_external_limits: Should return the expected external limits
        that are sent to the probe module by the EEBUS module
    - run_additional_assertions: Is called in test case to run additional assertions
        on the limits that are sent to the probe module by the EEBUS module
    """
    def get_load_limit(self) -> common_types_pb2.LoadLimit:
        raise NotImplementedError("get_load_limit is not implemented")
    def get_expected_external_limits(self) -> ExternalLimits:
        raise NotImplementedError("get_expected_external_limits is not implemented")
    def run_additional_assertions(self, limits: ExternalLimits, expected_limits: ExternalLimits):
        raise NotImplementedError("run_additional_assertions is not implemented")
