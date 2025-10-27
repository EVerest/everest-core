#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: andreas.heinrich@pionix.de
This script provides command to manage a list of file paths
It can be used for custom cmake commands to track created files and directories
and later remove or move them.
"""


import argparse
from pathlib import Path
import yaml


def create_filelist(args):
    if not args.root_dir.exists():
        raise ValueError("Root directory does not exist")
    if not args.root_dir.is_dir():
        raise ValueError("Root directory must be a directory")

    if args.data_file.exists():
        raise FileExistsError("Data file already exists")

    file_paths = []
    directory_paths = []
    for item in args.root_dir.rglob('*'):        
        relative_path = item.relative_to(args.root_dir)
        if item.is_dir():
            directory_paths.append(str(relative_path))
        elif item.is_file():
            file_paths.append(str(relative_path))
        else:
            raise ValueError(f"Unknown file type: {item}")
    
    data = {
        'files': file_paths,
        'directories': directory_paths
    }

    args.data_file.parent.mkdir(parents=True, exist_ok=True)
    with args.data_file.open('w') as f:
        yaml.dump(data, f)
    exit(0)


def remove_filelist(args):
    if not args.data_file.exists():
        exit(0)
    if not args.data_file.is_file():
        raise ValueError("Data file path is not a file")
    
    with args.data_file.open('r') as f:
        data = yaml.safe_load(f)
    
    for file_path in data.get('files', []):
        full_path = args.root_dir / file_path
        if not full_path.exists():
            raise FileNotFoundError(f"File does not exist: {full_path}")
        if not full_path.is_file():
            raise ValueError(f"Path is not a file: {full_path}")
        full_path.unlink()
    
    for dir_path in data.get('directories', []):
        full_path = args.root_dir / dir_path
        if not full_path.exists():
            raise FileNotFoundError(f"Directory does not exist: {full_path}")
        if not full_path.is_dir():
            raise ValueError(f"Path is not a directory: {full_path}")

        if len(list(full_path.iterdir())) > 0:
            continue

        full_path.rmdir()

    args.data_file.unlink()

    exit(0)


def move_filelist(args):
    if not args.root_dir.exists():
        raise ValueError("Root directory does not exist")
    if not args.root_dir.is_dir():
        raise ValueError("Root directory must be a directory")

    if not args.data_file.exists():
        raise FileNotFoundError("Data file does not exist")
    if not args.data_file.is_file():
        raise ValueError("Data file path is not a file")
    
    if not args.target_root_dir.is_absolute():
        raise ValueError("Target root directory must be absolute")
    if args.target_root_dir.exists():
        if not args.target_root_dir.is_dir():
            raise ValueError("Target root directory must be a directory")
    
    with args.data_file.open('r') as f:
        data = yaml.safe_load(f)

    for file_path in data.get('files', []):
        source_file = args.root_dir / file_path
        target_file = args.target_root_dir / file_path
        target_file.parent.mkdir(parents=True, exist_ok=True)
        source_file.rename(target_file)

    for dir_path in data.get('directories', []):
        source_dir = args.root_dir / dir_path
        target_dir = args.target_root_dir / dir_path
        if not target_dir.exists():
            source_dir.rename(target_dir)
    exit(0)


def main():
    parser = argparse.ArgumentParser(description='This script provides command to manage a list of file paths')
    
    subparsers = parser.add_subparsers()

    create_parser = subparsers.add_parser(
        "create",
        description="Creates the file with a list of all paths in it",
        add_help=True,
    )
    create_parser.add_argument(
        '--data-file',
        type=Path,
        dest='data_file',
        action='store',
        required=True,
        help='File to read/write from/to filelist'
    )
    create_parser.add_argument(
        '--root-directory',
        type=Path,
        dest='root_dir',
        action='store',
        required=True,
        help='Path to the directory to list'
    )
    create_parser.set_defaults(
        action_handler=create_filelist
    )

    remove_parser = subparsers.add_parser(
        "remove",
        description="Removes all files and directories listed in the filelist",
        add_help=True,
    )
    remove_parser.add_argument(
        '--data-file',
        type=Path,
        dest='data_file',
        action='store',
        required=True,
        help='File to read/write from/to filelist'
    )
    remove_parser.add_argument(
        '--root-directory',
        type=Path,
        dest='root_dir',
        action='store',
        required=True,
        help='Path to the directory to list'
    )
    remove_parser.set_defaults(
        action_handler=remove_filelist
    )
    
    move_parser = subparsers.add_parser(
        "move",
        description="Moves all files and directories listed in the filelist to a new root directory",
        add_help=True,
    )
    move_parser.add_argument(
        '--data-file',
        type=Path,
        dest='data_file',
        action='store',
        required=True,
        help='File to read/write from/to filelist'
    )
    move_parser.add_argument(
        '--root-directory',
        type=Path,
        dest='root_dir',
        action='store',
        required=True,
        help='Path to the directory to list'
    )
    move_parser.add_argument(
        '--target-root-directory',
        type=Path,
        dest='target_root_dir',
        action='store',
        required=True,
        help='Path to the target root directory to move files to'
    )
    move_parser.set_defaults(
        action_handler=move_filelist
    )
    
    args = parser.parse_args()

    if not args.root_dir.is_absolute():
        raise ValueError("Root directory must be absolute")

    if not args.data_file.is_absolute():
        raise ValueError("Data file path must be absolute")
    
    if 'action_handler' not in args:
        raise ValueError("No action specified")

    args.action_handler(args)
    exit(0)


if __name__ == "__main__":
    main()
