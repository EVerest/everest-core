#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: andreas.heinrich@pionix.de
This scripts checks whether the multiversion root directory contains latest/
"""


import argparse
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description='Checks whether the multiversion root directory contains latest/')
    parser.add_argument(
        '--multiversion-root-directory',
        type=Path,
        dest='multiversion_root_directory',
        action='store',
        required=True,
        help='Path to the multiversion root directory'
    )
    args = parser.parse_args()

    if not args.multiversion_root_directory.is_absolute():
        raise ValueError("Multiversion root directory must be absolute")

    if (args.multiversion_root_directory / "latest").is_dir():
        print(f"✅ latest/ exists at {args.multiversion_root_directory / 'latest'}")
    else:
        print(
            f"❌ latest/ does not exist at {args.multiversion_root_directory / 'latest'}"
            "  - make sure that you have a pre existing latest/ or mark the build as release"
        )
        exit(1)


if __name__ == "__main__":
    main()
