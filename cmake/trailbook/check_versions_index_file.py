#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: andreas.heinrich@pionix.de
This script checks whether the versions_index.html file exists at the specified location.
"""


import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description='Checks whether the versions_index.html file exists in the multiversion root directory')
    parser.add_argument(
        '--versions-index-file',
        type=Path,
        dest='versions_index_file',
        action='store',
        required=True,
        help='Path to the versions_index.html file'
    )
    args = parser.parse_args()

    if not args.versions_index_file.is_absolute():
        raise ValueError("Versions index file must be absolute")

    if args.versions_index_file.is_file():
        print(f"✅ versions_index.html exists at {args.versions_index_file}")
    else:
        print(f"❌ versions_index.html does not exist at {args.versions_index_file}")
        print("   Please make sure that your sphinx stem includes the generation of some versions_index.html file.")
        exit(1)


if __name__ == "__main__":
    main()
