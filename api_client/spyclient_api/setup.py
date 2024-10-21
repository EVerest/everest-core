# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

from setuptools import setup, find_packages

setup(
    name = "spyclient_api",
    version = "0.0.1",
    author = "Florin Mihut",
    author_email = "florin.mihut@pionix.de",
    url = "www.pionix.de",
    description = "A spy asyncapi python mqtt client",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    python_requires = ">=3.8"
)
