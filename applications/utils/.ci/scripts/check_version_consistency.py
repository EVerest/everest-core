#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
authors:
    - andreas.heinrich@pionix.de
    - kai-uwe.hermann@pionix.de
Check if versions in ev-dev-tools, everest-testing and CMake are consistent
"""


import argparse
import re
import sys

from pathlib import Path


cmake_project_version_search = r'(project\(everest-utils\n)+(\s+VERSION\s+)(\d+.\d+.\d+)'
python_package_version_search = r'(__version__\s+=\s+\')+(\d+.\d+.\d+)(\')+'


def get_cmake_project_version(cmakelists_txt_path: Path) -> str:
    with open(cmakelists_txt_path, 'r') as f:
        cmakelists_txt = f.read()

        version_search = re.search(cmake_project_version_search, cmakelists_txt)
        if version_search:
            return version_search.group(3)

    raise Exception('Could not get CMake project version')


def set_cmake_project_version(cmakelists_txt_path: Path, version: str) -> str:
    with open(cmakelists_txt_path, 'r+') as f:
        cmakelists_txt = f.read()
        f.seek(0)

        def replace(match):
            return f'{match.group(1)}{match.group(2)}{version}'
        version_search = re.sub(cmake_project_version_search, replace, cmakelists_txt)
        f.write(version_search)


def get_python_package_version(init_py_path: Path) -> str:
    with open(init_py_path, 'r') as f:
        init_py = f.read()

        version_search = re.search(python_package_version_search, init_py)
        if version_search:
            return version_search.group(2)

    raise Exception(f'Could not get Python package version from {init_py_path}')


def set_python_package_version(init_py_path: Path, version: str) -> str:
    with open(init_py_path, 'r+') as f:
        init_py = f.read()
        f.seek(0)

        def replace(match):
            return f'{match.group(1)}{version}{match.group(3)}'
        version_search = re.sub(python_package_version_search, replace, init_py)
        f.write(version_search)


def main() -> int:
    parser = argparse.ArgumentParser(
        description='check for consistent versions in everest-utils')

    parser.add_argument('--input',
                        dest='in_path',
                        help='Path to everest-utils sourcecode',
                        required=True)
    parser.add_argument('--update-version',
                        dest='update_version',
                        help='New version number to set in all relevant parts of the project, format: x.y.z where x, y and z are positive integers')
    parser.add_argument('--fix', action='store_true', default=False,
                        help='Fix version mismatches by applying the version defined in the toplevel CMakeLists.txt')

    args = parser.parse_args()

    in_path = Path(args.in_path).expanduser().resolve()

    cmakelists_txt_path = in_path / 'CMakeLists.txt'
    ev_cli_init_py_path = in_path / 'ev-dev-tools' / 'src' / 'ev_cli' / '__init__.py'
    everest_testing_init_py_path = in_path / 'everest-testing' / 'src' / 'everest' / 'testing' / '__init__.py'

    update_version = False

    if args.update_version:
        if not re.match(r'(\d+.\d+.\d+)', args.update_version):
            print(
                f'Provided version `{args.update_version}` has to be in format x.y.z where x, y and z are positive integers')
            return 1
        new_version = args.update_version
        update_version = True
    elif args.fix:
        new_version = get_cmake_project_version(cmakelists_txt_path)
        update_version = True

    if update_version:
        print(f'Updating version to: {new_version}')
        set_cmake_project_version(cmakelists_txt_path, new_version)
        set_python_package_version(ev_cli_init_py_path, new_version)
        set_python_package_version(everest_testing_init_py_path, new_version)

    everest_utils_version = get_cmake_project_version(cmakelists_txt_path)
    print(f'everest-utils version: {everest_utils_version}')

    ev_cli_python_version = get_python_package_version(ev_cli_init_py_path)
    print(f'ev-cli version: {ev_cli_python_version}')
    everest_testing_python_version = get_python_package_version(everest_testing_init_py_path)
    print(f'everest-testing version: {everest_testing_python_version}')

    if all(value == everest_utils_version for value in [ev_cli_python_version, everest_testing_python_version]):
        return 0
    print('Version mismatch, you can fix this by calling this script with "--fix" which will apply the "everest-utils version" defined in the toplevel CMakeLists.txt')
    print('Alternatively you can apply a new version by calling this script with "--update-version x.y.z" where x, y and z are positive integers')
    return 1


if __name__ == '__main__':
    return_value = 1
    try:
        return_value = main()
    except Exception as e:
        print(f'Error: {e}')
        return_value = 1
    sys.exit(return_value)
