# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import pytest
import pytest_asyncio

import grpc_servicer.control_service_servicer as control_service_servicer_module
import grpc_servicer.cs_lpc_control_servicer as cs_lpc_control_servicer_module
import grpc_server.control_service_server as control_service_server_module
import grpc_server.cs_lpc_control_server as cs_lpc_control_server_module

@pytest.fixture
def control_service_servicer():
    """
    Provides a ControlServiceServicer instance for testing.
    """
    return control_service_servicer_module.ControlServiceServicer()

@pytest_asyncio.fixture
async def control_service_server(request, control_service_servicer):
    """
    Provides an asyncio ControlServiceServer instance for testing that is automatically
    started and stopped.
    """
    rpc_port = request.node.get_closest_marker("eebus_rpc_port")
    if rpc_port is None or len(rpc_port.args) == 0:
        pytest.fail("No rpc port provided")
    rpc_port = rpc_port.args[0]
    server = control_service_server_module.ControlServiceServer(control_service_servicer, rpc_port)
    await server.start()
    yield server
    await server.stop()

@pytest.fixture
def cs_lpc_control_servicer():
    """
    Provides a CsLpcControlServicer instance for testing.
    """
    return cs_lpc_control_servicer_module.CsLpcControlServicer()

@pytest_asyncio.fixture
async def cs_lpc_control_server(cs_lpc_control_servicer):
    """
    Provides an asyncio CsLpcControlServer instance for testing that is automatically
    started and stopped.
    """
    server = cs_lpc_control_server_module.CsLpcControlServer(cs_lpc_control_servicer)
    await server.start()
    yield server
    await server.stop()
