# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest_asyncio
import logging
from prepared_eebus_module_test import PreparedEEBUSModuleTest

from everest.testing.core_utils.fixtures import everest_core, test_controller
from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.controller.everest_test_controller import EverestTestController

from grpc_servicer.control_service_servicer import ControlServiceServicer
from grpc_server.control_service_server import ControlServiceServer
from grpc_servicer.cs_lpc_control_servicer import CsLpcControlServicer
from grpc_server.cs_lpc_control_server import CsLpcControlServer

from fixtures.grpc_testing_server import control_service_server, control_service_servicer, cs_lpc_control_server, cs_lpc_control_servicer

@pytest_asyncio.fixture
async def eebus_module_test(
        everest_core: EverestCore,
        test_controller: EverestTestController,
        control_service_server: ControlServiceServer,
        control_service_servicer: ControlServiceServicer,
        cs_lpc_control_server: CsLpcControlServer,
        cs_lpc_control_servicer: CsLpcControlServicer,
):
    """
    Provides a PreparedEEBUSModuleTest instance for testing.

    It is set up and torn down automatically by pytest.
    """
    test = PreparedEEBUSModuleTest(
        everest_core=everest_core,
        test_controller=test_controller,
        control_service_server=control_service_server,
        control_service_servicer=control_service_servicer,
        cs_lpc_control_server=cs_lpc_control_server,
        cs_lpc_control_servicer=cs_lpc_control_servicer
    )
    logging.info("Setting up EEBUSModuleTest")
    await test.setup()
    yield test
    logging.info("Tearing down EEBUSModuleTest")
    test.tear_down()