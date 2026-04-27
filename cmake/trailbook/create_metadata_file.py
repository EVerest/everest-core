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
import json
from pathlib import Path
import yaml


def main():
    parser = argparse.ArgumentParser(description='Creates metadata yaml and json files')
    
    parser.add_argument(
        '--multiversion-root-directory',
        type=Path,
        dest='multiversion_root_dir',
        action='store',
        required=True,
        help='Path to the root directory of the multiversion documentation'
    )
    parser.add_argument(
        '--yaml-output-path',
        type=Path,
        dest='yaml_output_path',
        action='store',
        required=True,
        default=None,
        help='Path where the trailbook_metadata.yaml file will be created'
    )
    parser.add_argument(
        '--json-output-path',
        type=Path,
        dest='json_output_path',
        action='store',
        required=True,
        default=None,
        help='Path where the trailbook_metadata.json file will be created'
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

    if args.yaml_output_path is None or args.json_output_path is None:
        parser.error("at least one of --yaml-output-path or --json-output-path is required")

    if not args.multiversion_root_dir.is_absolute():
        raise ValueError("Multiversion root directory must be absolute")
    if not args.multiversion_root_dir.is_dir():
        print(f"\033[33mWarning: {args.multiversion_root_dir} does not exist or is not a directory, it is treated as an empty multiversion root dir\033[0m")
    if not args.json_output_path.is_absolute():
        raise ValueError("JSON output path must be absolute")
    if not args.yaml_output_path.is_absolute():
        raise ValueError("YAML output path must be absolute")
    if args.json_output_path.exists():
        raise FileExistsError("JSON output path already exists")
    if args.yaml_output_path.exists():
        raise FileExistsError("YAML output path already exists")

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
    if args.yaml_output_path is not None:
        with args.yaml_output_path.open('w') as f:
            yaml.dump(data, f, default_flow_style=False)

    if args.json_output_path is not None:
        with args.json_output_path.open('w') as f:
            json.dump(data, f, indent=2)


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}")
        exit(1)
