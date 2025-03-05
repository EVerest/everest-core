# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

def pytest_addoption(parser):
    parser.addoption("--everest-prefix", action="store", default="../build/dist",
                     help="everest prefix path; default = '../build/dist'")
