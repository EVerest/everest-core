#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: andreas.heinrich@pionix.de
This script checks whether a directory exists or not and returns zero based on the flags provided.
"""


import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description='Checks whether a directory exists or not and returns zero based on the flags provided')
    parser.add_argument(
        '--directory',
        type=Path,
        dest='directory',
        action='store',
        required=True,
        help='Directory to check for existence'
    )
    parser.add_argument(
        '--return-zero-if-exists',
        action='store_true',
        help='Return zero if the directory exists',
        dest='return_zero_if_exists',
    )
    parser.add_argument(
        '--return-zero-if-not-exists',
        action='store_true',
        help='Return zero if the directory does not exist',
        dest='return_zero_if_not_exists',
    )
    args = parser.parse_args()

    if not args.directory.is_absolute():
        raise ValueError("Directory path must be absolute")
    if args.return_zero_if_exists and args.return_zero_if_not_exists:
        raise ValueError("Cannot use both --return-zero-if-exists and --return-zero-if-not-exists at the same time")

    if args.return_zero_if_exists:
        if not args.directory.exists():
            print(f"❌ Directory does not exist at {args.directory}")
            exit(1)
        if not args.directory.is_dir():
            print(f"❌ Path exists but is not a directory at {args.directory}")
            exit(2)
        print(f"✅ Directory exists at {args.directory}")
        exit(0)
    elif args.return_zero_if_not_exists:
        if args.directory.is_dir():
            print(f"❌ Directory exists at {args.directory}")
            exit(1)
        if args.directory.exists():
            print(f"❌ Path exists but is not a directory at {args.directory}")
            exit(2)
        print(f"✅ Directory does not exist at {args.directory}")
        exit(0)
    else:
        raise ValueError("Either --return-zero-if-exists or --return-zero-if-not-exists must be specified")


if __name__ == "__main__":
    main()
