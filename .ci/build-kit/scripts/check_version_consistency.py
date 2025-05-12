#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: kai-uwe.hermann@pionix.de
Check if versions in everestjs, everestpy, everestrs and CMake are consistent
"""
import argparse
import json
import re
import sys

from pathlib import Path


cmake_project_version_search = r'(project\(everest-framework\n)+(\s+VERSION\s+)(\d+.\d+.\d+)'
python_package_version_search = r'(__version__\s+=\s+\')+(\d+.\d+.\d+)(\')+'
python_cpp_version_search = r'(m\.attr\(\"__version__\"\)\s+=\s+\")+(\d+.\d+.\d+)(\";)+'
everestrs_version_search = r'(\[package\]\nname = \"everest[a-zA-Z-]+\"\nversion = \")+(\d+.\d+.\d+)(\")+'


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


def get_js_package_version(package_json_path: Path) -> str:
    with open(package_json_path, 'r') as f:
        package_json = json.load(f)

        return package_json['version']

    raise Exception('Could not get JavaScript package version')


def set_js_package_version(package_json_path: Path, version: str) -> str:
    with open(package_json_path, 'r+') as f:
        package_json = json.load(f)
        package_json['version'] = version
        f.seek(0)
        json.dump(package_json, f, indent=2)


def get_python_package_version(init_py_path: Path) -> str:
    with open(init_py_path, 'r') as f:
        init_py = f.read()

        version_search = re.search(python_package_version_search, init_py)
        if version_search:
            return version_search.group(2)

    raise Exception('Could not get Python package version')


def set_python_package_version(init_py_path: Path, version: str) -> str:
    with open(init_py_path, 'r+') as f:
        init_py = f.read()
        f.seek(0)

        def replace(match):
            return f'{match.group(1)}{version}{match.group(3)}'
        version_search = re.sub(python_package_version_search, replace, init_py)
        f.write(version_search)


def get_python_cpp_version(everestpy_cpp_path: Path) -> str:
    with open(everestpy_cpp_path, 'r') as f:
        everestpy_cpp = f.read()

        version_search = re.search(python_cpp_version_search, everestpy_cpp)
        if version_search:
            return version_search.group(2)

    raise Exception('Could not get Python C++ code version')


def set_python_cpp_version(everestpy_cpp_path: Path, version: str) -> str:
    with open(everestpy_cpp_path, 'r+') as f:
        everestpy_cpp = f.read()
        f.seek(0)

        def replace(match):
            return f'{match.group(1)}{version}{match.group(3)}'
        version_search = re.sub(python_cpp_version_search, replace, everestpy_cpp)
        f.write(version_search)


def get_cargo_toml_version(cargo_toml_path: Path) -> str:
    with open(cargo_toml_path, 'r') as f:
        cargo_toml = f.read()

        version_search = re.search(everestrs_version_search, cargo_toml)
        if version_search:
            return version_search.group(2)

    raise Exception('Could not get Cargo.toml version')


def set_cargo_toml_version(cargo_toml_path: Path, version: str) -> str:
    with open(cargo_toml_path, 'r+') as f:
        cargo_toml = f.read()
        f.seek(0)

        def replace(match):
            return f'{match.group(1)}{version}{match.group(3)}'
        version_search = re.sub(everestrs_version_search, replace, cargo_toml)
        f.write(version_search)


def main() -> int:
    parser = argparse.ArgumentParser(
        description='check for consistent versions in everest-framework')

    parser.add_argument('--input',
                        dest='in_path',
                        help='Path to everest-framework sourcecode',
                        required=True)
    parser.add_argument('--update-version',
                        dest='update_version',
                        help='New version number to set in all relevant parts of the project, format: x.y.z where x, y and z are positive integers')
    parser.add_argument('--fix', action='store_true', default=False,
                        help='Fix version mismatches by applying the version defined in the toplevel CMakeLists.txt')

    args = parser.parse_args()

    in_path = Path(args.in_path).expanduser().resolve()

    cmakelists_txt_path = in_path / 'CMakeLists.txt'
    package_json_path = in_path / 'everestjs' / 'package.json'
    init_py_path = in_path / 'everestpy' / 'src' / 'everest' / 'framework' / '__init__.py'
    everestpy_cpp_path = in_path / 'everestpy' / 'src' / 'everest' / 'everestpy.cpp'
    everestrs_cargo_toml_path = in_path / 'everestrs' / 'everestrs' / 'Cargo.toml'
    everestrs_build_cargo_toml_path = in_path / 'everestrs' / 'everestrs-build' / 'Cargo.toml'

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
        set_js_package_version(package_json_path, new_version)
        set_python_package_version(init_py_path, new_version)
        set_python_cpp_version(everestpy_cpp_path, new_version)
        set_cargo_toml_version(everestrs_cargo_toml_path, new_version)
        set_cargo_toml_version(everestrs_build_cargo_toml_path, new_version)

    everest_framework_version = get_cmake_project_version(cmakelists_txt_path)
    print(f'everest-framework version: {everest_framework_version}')

    everestjs_version = get_js_package_version(package_json_path)
    print(f'everestjs version: {everestjs_version}')

    everestpy_python_version = get_python_package_version(init_py_path)
    print(f'everestpy python version: {everestpy_python_version}')

    everestpy_cpp_version = get_python_cpp_version(everestpy_cpp_path)
    print(f'everestpy C++ version: {everestpy_cpp_version}')

    everestrs_version = get_cargo_toml_version(everestrs_cargo_toml_path)
    print(f'everestrs version: {everestrs_version}')

    everestrs_build_version = get_cargo_toml_version(everestrs_build_cargo_toml_path)
    print(f'everestrs-build version: {everestrs_build_version}')

    if all(value == everest_framework_version for value in [everestjs_version, everestpy_python_version, everestpy_cpp_version, everestrs_version, everestrs_build_version]):
        return 0
    print('Version mismatch, you can fix this by calling this script with "--fix" which will apply the "everest-framework version" defined in the toplevel CMakeLists.txt')
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
