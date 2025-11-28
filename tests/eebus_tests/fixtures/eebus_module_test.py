# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest_asyncio
import logging
import asyncio
import threading

from everest.testing.core_utils.everest_core import EverestCore
from everest.testing.core_utils.probe_module import ProbeModule
from everest.testing.core_utils.controller.everest_test_controller import EverestTestController

from grpc_servicer.control_service_servicer import ControlServiceServicer
from grpc_server.control_service_server import ControlServiceServer
from grpc_servicer.cs_lpc_control_servicer import CsLpcControlServicer
from grpc_server.cs_lpc_control_server import CsLpcControlServer

from .grpc_testing_server import control_service_server, control_service_servicer, cs_lpc_control_server, cs_lpc_control_servicer
from helpers.conversions import convert_external_limits


class EebusTestProbeModule(ProbeModule):
    """
    This class extends the ProbeModule to add a command for receiving
    external limits from the EEBUS module.
    """

    def __init__(self, runtime_session):
        super().__init__(runtime_session)

        super().implement_command(
            "eebus_energy_sink",
            "set_external_limits",
            lambda arg: self._set_external_limits(arg),
        )
        self.external_limits_queue = asyncio.Queue(maxsize=1)

    def _set_external_limits(self, limits):
        logging.info(f"Set external limits: {limits}")
        limits = convert_external_limits(limits)
        self.external_limits_queue.put_nowait(limits)


@pytest_asyncio.fixture
async def eebus_test_env(everest_core: EverestCore, test_controller: EverestTestController, control_service_server: ControlServiceServer, cs_lpc_control_server: CsLpcControlServer, control_service_servicer: ControlServiceServicer, cs_lpc_control_servicer: CsLpcControlServicer):
    """
    This fixture provides the basic test environment for the EEBUS module.
    It starts EVerest and the mock gRPC servers.
    """
    # Start everest in a thread so it doesn't block the event loop
    everest_thread = threading.Thread(target=test_controller.start)
    everest_thread.start()

    # The module will start initializing. The test function is now responsible for handling the handshake.

    test_env = {
        "everest_core": everest_core,
        "control_service_servicer": control_service_servicer,
        "cs_lpc_control_servicer": cs_lpc_control_servicer,
        "cs_lpc_control_server": cs_lpc_control_server,
    }

    yield test_env

    # Teardown
    logging.info("Tearing down EEBUSModuleTest fixture")

    # Stop the servicers and servers
    control_service_servicer.stop()
    await control_service_server.stop()
    await cs_lpc_control_server.stop()

    # Stop the EVerest process first
    test_controller.stop()
