# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
import logging
import sys
import time
from pathlib import Path

import pytest

sys.path.append(str(Path(__file__).parent / "../utils"))

from lem_dcbm_api_mock.main import app as lem_api_mock
import uvicorn
from multiprocessing import Process


def pytest_addoption(parser):
    parser.addoption("--everest-prefix", action="store", default="../build/dist",
                     help="everest prefix path; default = '../build/dist'")
    parser.addoption("--lem-dcbm-host", action="store",
                     help="Address of LEM DCBM 400/600")
    parser.addoption("--lem-dcbm-port", action="store",
                     help="Port of LEM DCBM 400/600")


@pytest.fixture(scope="module")
def lem_dcbm_mock():
    # Start the server in a subprocess
    server = Process(target=uvicorn.run,
                     args=(lem_api_mock,),
                     kwargs={
                         "host": "0.0.0.0",
                         "port": 8000,
                         "log_level": "info"},
                     daemon=True)
    try:
        server.start()
        time.sleep(0.1)  # Allow some time for the server to start
        assert server.is_alive()
        logging.info("started up lem dcbm api mock server")
        yield  # This is where the testing happens

        # After the tests, terminate the server
    finally:
        server.terminate()
