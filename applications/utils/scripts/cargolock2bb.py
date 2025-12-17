#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: kai-uwe.hermann@pionix.de
Convert Cargo.lock files (that can also be provided as an URL) into a .bb file
"""
import argparse
import os
import requests
import toml


def read_cargo_lock_file(cargo_lock_path):
    try:
        cargo_lock_file = open(cargo_lock_path, 'r')
    except Exception:
        return None

    with cargo_lock_file:
        return toml.load(cargo_lock_file)


def read_cargo_lock_from_url(cargo_lock_url):
    try:
        req = requests.get(cargo_lock_url, timeout=5)
        return toml.loads(req.text)
    except Exception:
        return None


def print_bb_output(cargo_lock, skip_packages):
    if not cargo_lock or 'package' not in cargo_lock:
        print('# No packages in Cargo.lock file')
        return

    print(f'SRC_URI += " \\')
    for package in cargo_lock['package']:
        if package['name'] in skip_packages:
            continue
        print(f'           crate://crates.io/{package["name"]}/{package["version"]} \\')
    print('           "')


def main():
    parser = argparse.ArgumentParser(description='converts Cargo.lock files to bitbake recipe output')

    parser.add_argument('--input',
                        dest='in_cargo_lock',
                        default='Cargo.lock',
                        help='Path to the Cargo.lock file')
    parser.add_argument('--url',
                        dest='url_cargo_lock',
                        default=None,
                        help='URL to the Cargo.lock file')
    parser.add_argument('--skip',
                        dest='in_skip',
                        default='',
                        type=str,
                        help='List of packages to skip, using \',\' as delimiter.')

    args = parser.parse_args()

    in_cargo_lock = os.path.realpath(os.path.expanduser(args.in_cargo_lock))
    in_cargo_lock_url = args.url_cargo_lock
    in_skip_packages = [entry for entry in args.in_skip.split(',')]

    if in_cargo_lock_url:
        cargo_lock = read_cargo_lock_from_url(in_cargo_lock_url)
    else:
        cargo_lock = read_cargo_lock_file(in_cargo_lock)

    print_bb_output(cargo_lock, in_skip_packages)


if __name__ == '__main__':
    main()
