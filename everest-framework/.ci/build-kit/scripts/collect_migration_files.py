#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: kai-uwe.hermann@pionix.de
Collects database migration files
FIXME: move this to everest-sqlite once it has bazel build support
"""
import argparse
import re
import sys

from pathlib import Path


SUCCESS = 0
FAILURE = 1


def main() -> int:
    parser = argparse.ArgumentParser(
        description='collect database migration files')

    parser.add_argument('--location',
                        dest='location',
                        help='Path to migration files')
    parser.add_argument('--migration-files',
                        dest='migration_files',
                        nargs='*')
    parser.add_argument('--output',
                        dest='output',
                        help='Path to compile_time_settings.hpp to append defines to')

    args = parser.parse_args()

    if not args.location and not args.migration_files:
        print(f'Either --location or --migration-files has to be provided')
        return FAILURE

    migration_file_list = []

    if args.location:
        location = Path(args.location).expanduser().resolve()
        migration_file_list = list(location.glob('*.sql'))
    elif args.migration_files:
        for migration_file in args.migration_files:
            migration_file_list.append(Path(migration_file))

    migration_file_list.sort()

    current_migration_file_id = 1
    next_migration_file_type = 'up'
    for migration_file in migration_file_list:
        file_name = migration_file.name
        if not re.match(r'^([0-9]+)_(up|down)(|-.+)\.sql$', file_name):
            print(f'Migration filename does not match specification: {file_name}')
            return FAILURE
        next_id = f'^{current_migration_file_id}_'
        if not re.match(next_id, file_name):
            print(f'Skipped migration file ID, expected {current_migration_file_id}_*.sql, but got {file_name}')
            return FAILURE
        next_id = f'{next_id}{next_migration_file_type}'
        if not re.match(next_id, file_name):
            print(f'Missing {next_migration_file_type} migration file: {file_name}')
            return FAILURE

        if next_migration_file_type == 'up':
            current_migration_file_id += 1
            next_migration_file_type = 'down'
        elif next_migration_file_type == 'down' :
            next_migration_file_type = 'up'
        else:
            print(f'Unknown next migration file type: {next_migration_file_type}')
            return FAILURE
    
    if next_migration_file_type == 'up':
        print(f'Down migration file {current_migration_file_id}_*.sql is missing up migration file')
        return FAILURE

    # Since we always add on the up file we need to subtract one here
    current_migration_file_id -= 1
    if args.output:
        output = Path(args.output).expanduser().resolve()
        with open(output, mode='a+') as bazel_out_file:
            bazel_out_file.write(f'#define TARGET_MIGRATION_FILE_VERSION {current_migration_file_id}')
    return SUCCESS


if __name__ == '__main__':
    return_value = FAILURE
    try:
        return_value = main()
    except Exception as e:
        print(f'Error: {e}')
        return_value = FAILURE
    sys.exit(return_value)
