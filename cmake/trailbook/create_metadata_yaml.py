#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: andreas.heinrich@pionix.de
This script creates a trailbook_metadata.yaml file 
based on the versions found in the multiversion root directory.
"""


import argparse
from pathlib import Path
import yaml


def main():
    parser = argparse.ArgumentParser(description='Creates a trailbook_metadata.yaml file')
    
    parser.add_argument(
        '--multiversion-root-directory',
        type=Path,
        dest='multiversion_root_dir',
        action='store',
        required=True,
        help='Path to the root directory of the multiversion documentation'
    )
    parser.add_argument(
        '--output-path',
        type=Path,
        dest='output_path',
        action='store',
        required=True,
        help='Path where the trailbook_metadata.yaml file will be created'
    )
    parser.add_argument(
        '--additional-version',
        type=str,
        dest='additional_versions',
        action='append',
        default=[],
        help='Additional version to include in the metadata (can be used multiple times)'
    )
    args = parser.parse_args()

    if not args.multiversion_root_dir.is_absolute():
        raise ValueError("Multiversion root directory must be absolute")
    if not args.multiversion_root_dir.is_dir():
        print(f"\033[33mWarning: {args.multiversion_root_dir} does not exist or is not a directory, it is treated as an empty multiversion root dir\033[0m")
    if not args.output_path.is_absolute():
        raise ValueError("Output path must be absolute")
    if args.output_path.exists():
        raise FileExistsError("Output path already exists")

    versions_list = []
    if args.multiversion_root_dir.is_dir():
        for instance_dir in args.multiversion_root_dir.iterdir():
            if not instance_dir.is_dir():
                continue
            if not (instance_dir / 'index.html').is_file():
                continue
            versions_list.append(instance_dir.name)
    versions_list.extend(args.additional_versions)
    versions_list = list(set(versions_list))
    if len(versions_list) == 0:
        raise ValueError("No versions found in the specified multiversion root directory")
    versions_list.sort()

    # create yaml content
    data = {
        'versions': versions_list
    }
    # render yaml content
    with args.output_path.open('w') as f:
        yaml.dump(data, f, default_flow_style=False)


if __name__ == "__main__":
    main()
