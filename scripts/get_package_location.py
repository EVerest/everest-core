#!/usr/bin/env -S python3 -tt
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: kai-uwe.hermann@pionix.de
"""

import argparse
from importlib.metadata import Distribution, PackageNotFoundError
import json


__version__ = '0.1.0'


class EVerestParsingException(SystemExit):
    pass


def main():
    parser = argparse.ArgumentParser(
        description='EVerest get package location')
    parser.add_argument('--version', action='version',
                        version=f'%(prog)s {__version__}')
    parser.add_argument('--package-name', type=str,
                        help='Name of the package that the location should be retrieved from', default=None)
    args = parser.parse_args()

    if not 'package_name' in args or not args.package_name:
        raise EVerestParsingException('Please provide a valid package name')

    direct_url = ""
    try:
        direct_url = Distribution.from_name(
            args.package_name).read_text('direct_url.json')
    except PackageNotFoundError as e:
        raise EVerestParsingException(e)
    url = json.loads(direct_url).get('url', None)

    if url and url.startswith('file://'):
        url = url.replace('file://', '')
        print(f'{url}')
    else:
        raise EVerestParsingException()


if __name__ == '__main__':
    try:
        main()
    except EVerestParsingException as e:
        raise SystemExit(e)
