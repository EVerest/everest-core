# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import os

def pytest_addoption(parser):
    parser.addoption("--path", action="store", default="~/checkout/everest-workspace/everest-core",
                     help="everest-core path; default = '~/checkout/everest-workspace/everest-core'")


def pytest_sessionfinish(session, exitstatus):
    os._exit(exitstatus)
