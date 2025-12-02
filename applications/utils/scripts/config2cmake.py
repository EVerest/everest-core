#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: kai-uwe.hermann@pionix.de
Parse a EVerest config and return a CMake command line to build only those modules
"""
import argparse
import yaml
import sys

from pathlib import Path


def get_modules(config_yaml_path: Path):
    try:
        config = yaml.safe_load(config_yaml_path.read_text())
        module_names = set()
        for _key, value in config['active_modules'].items():
            module_names.add(value['module'])
        modules = ';'.join(sorted(module_names))
        return f'-DEVEREST_INCLUDE_MODULES="{modules}"'
    except yaml.YAMLError as err:
        raise Exception(f'Could not parse config file {config_yaml_path}') from err


def main() -> int:
    parser = argparse.ArgumentParser(
        description='parse EVerest configs and extract modules')

    parser.add_argument('config',
                        help='Path to EVerest config',
                        nargs=1)
    parser.add_argument('--full',
                        action='store_true',
                        default=False,
                        help='Set this flag if you want a full cmake command line (with "build" as default build-dir)')

    args = parser.parse_args()

    config_path = Path(args.config[0]).expanduser().resolve()

    try:
        modules = get_modules(config_path)
        if args.full:
            print(f'cmake -S . -B build {modules}')
        else:
            print(modules)
    except Exception as err:
        print(f'Could not generate CMake command line: {err}')
        return 1

    return 0


if __name__ == '__main__':
    return_value = 1
    try:
        return_value = main()
    except Exception as e:
        print(f'Error: {e}')
        return_value = 1
    sys.exit(return_value)
