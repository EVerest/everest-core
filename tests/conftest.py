# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

import os

def pytest_addoption(parser):
    parser.addoption("--everest-prefix", action="store", default="../build/dist",
                     help="everest prefix path; default = '../build/dist'")
