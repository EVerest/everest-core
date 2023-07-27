# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

def pytest_addoption(parser):
    parser.addoption("--everest-prefix", action="store", default="~/checkout/everest-workspace/everest-core",
                     help="everest-core path; default = '~/checkout/everest-workspace/everest-core'")
    parser.addoption("--libocpp", action="store", default="~/checkout/everest-workspace/libocpp",
                     help="libocpp path; default = '~/checkout/everest-workspace/libocpp'")

