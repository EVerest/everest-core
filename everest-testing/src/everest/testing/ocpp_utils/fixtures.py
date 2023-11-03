# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
import getpass
import os
import shutil
import socket
import ssl
import sys
import tempfile
from pathlib import Path
from threading import Thread

import pytest
import pytest_asyncio
from pyftpdlib import servers
from pyftpdlib.authorizers import DummyAuthorizer
from pyftpdlib.handlers import FTPHandler

from everest.testing.core_utils.common import OCPPVersion
from everest.testing.core_utils.configuration.everest_environment_setup import EverestEnvironmentOCPPConfiguration
from everest.testing.core_utils.controller.everest_test_controller import EverestTestController
from everest.testing.ocpp_utils.central_system import CentralSystem, inject_csms_v201_mock, inject_csms_v16_mock
from everest.testing.ocpp_utils.charge_point_utils import TestUtility, OcppTestConfiguration

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), ".")))


@pytest.fixture
def ocpp_version(request) -> OCPPVersion:
    ocpp_version = request.node.get_closest_marker("ocpp_version")
    if ocpp_version:
        return OCPPVersion(request.node.get_closest_marker("ocpp_version").args[0])
    else:
        return OCPPVersion("ocpp1.6")


@pytest.fixture
def ocpp_config(request, central_system: CentralSystem, test_config: OcppTestConfiguration, ocpp_version: OCPPVersion):
    ocpp_config_marker = request.node.get_closest_marker("ocpp_config")

    return EverestEnvironmentOCPPConfiguration(
        central_system_port=central_system.port,
        central_system_host="127.0.0.1",
        ocpp_version=ocpp_version,
        libocpp_path=Path(request.config.getoption("--libocpp")),
        template_ocpp_config=Path(ocpp_config_marker.args[0]) if ocpp_config_marker else None
    )


@pytest_asyncio.fixture
async def central_system(request, ocpp_version: OCPPVersion, test_config):
    """Fixture for CentralSystem. Can be started as TLS or
        plain websocket depending on the request parameter.
    """
    if (hasattr(request, 'param')):
        ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        ssl_context.load_cert_chain(test_config.certificate_info.csms_cert,
                                    test_config.certificate_info.csms_key,
                                    test_config.certificate_info.csms_passphrase)
    else:
        ssl_context = None
    cs = CentralSystem(test_config.charge_point_info.charge_point_id,
                       ocpp_version=ocpp_version)

    if request.node.get_closest_marker('inject_csms_mock'):
        if ocpp_version == OCPPVersion.ocpp201:
            mock = inject_csms_v201_mock(cs)
        else:
            mock = inject_csms_v16_mock(cs)
        cs.mock = mock

    async with cs.start(ssl_context):
        yield cs


@pytest_asyncio.fixture
async def charge_point(central_system: CentralSystem, test_controller: EverestTestController):
    """Fixture for ChargePoint16. Requires central_system_v201 and test_controller. Starts test_controller immediately
    """
    test_controller.start()
    cp = await central_system.wait_for_chargepoint()
    yield cp
    await cp.stop()


@pytest.fixture
def test_utility():
    """Fixture for test case meta data
    """
    return TestUtility()


@pytest.fixture
def test_config():
    return OcppTestConfiguration()


class FtpThread(Thread):
    def set_port(self, port):
        self.port = port


@pytest.fixture
def ftp_server(test_config: OcppTestConfiguration):
    """This fixture creates a temporary directory and starts
    a local ftp server connected to that directory. The temporary
    directory is deleted afterwards
    """

    d = tempfile.mkdtemp(prefix='tmp_ftp')
    address = ("127.0.0.1", 0)
    ftp_socket = socket.socket()
    ftp_socket.bind(address)
    port = ftp_socket.getsockname()[1]

    def _ftp_server(test_config: OcppTestConfiguration, ftp_socket):
        shutil.copyfile(test_config.firmware_info.update_file, os.path.join(
            d, "firmware_update.pnx"))
        shutil.copyfile(test_config.firmware_info.update_file_signature,
                        os.path.join(d, "firmware_update.pnx.base64"))

        authorizer = DummyAuthorizer()
        authorizer.add_user(getpass.getuser(), "12345", d, perm="elradfmwMT")

        handler = FTPHandler
        handler.authorizer = authorizer

        server = servers.FTPServer(ftp_socket, handler)

        server.serve_forever()

    ftp_thread = FtpThread(target=_ftp_server, args=[test_config, ftp_socket])
    ftp_thread.daemon = True
    ftp_thread.set_port(port)
    ftp_thread.start()

    yield ftp_thread

    shutil.rmtree(d)


@pytest_asyncio.fixture
async def central_system_v16(central_system):
    """ Note: This is only for backwards compatibility; use central_system directly! """
    yield central_system


@pytest_asyncio.fixture
async def central_system_v201(central_system):
    """ Note: This is only for backwards compatibility; use central_system directly! """
    yield central_system


@pytest_asyncio.fixture
async def charge_point_v16(charge_point):
    """ Note: This is only for backwards compatibility; use charge_point directly! """
    yield charge_point


@pytest_asyncio.fixture
async def charge_point_v201(charge_point):
    """ Note: This is only for backwards compatibility; use charge_point directly! """
    yield charge_point


@pytest_asyncio.fixture
async def central_system_v16_standalone(request, central_system: CentralSystem, test_controller: EverestTestController):
    """ Note: This is only for backwards compatibility; use central_system + test_controller directly!

    Fixture for standalone central system. Requires central_system_v16 and test_controller. Starts test_controller immediately
    """
    test_controller.start()
    yield central_system
    test_controller.stop()
