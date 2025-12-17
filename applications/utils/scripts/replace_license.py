#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: kai-uwe.hermann@pionix.de
Replace licenses with Apache 2.0
"""

import argparse
from datetime import date
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(
        description='replaces licenses with Apache 2.0')

    parser.add_argument('--working-dir', '-wd', type=str,
                        help='Working directory (default: .)', default=str(Path.cwd()))
    parser.add_argument('--no-year', action='store_true', help='Do not include years in license header')

    args = parser.parse_args()

    working_dir = Path(args.working_dir).expanduser().resolve()

    files = [file for file in working_dir.rglob('*') if file.suffix in ['.cpp', '.hpp']]

    year = ''
    if not args.no_year:
        year = f'2020 - {date.today().year} '

    license_text = f"""// SPDX-License-Identifier: Apache-2.0
// Copyright {year}Pionix GmbH and Contributors to EVerest
"""

    success = 0
    failure = 0
    count = len(files)

    for file in files:
        content = file.read_text()
        if content.startswith('/*'):
            needle = '*/\n'
            end = content.find(needle) + len(needle)
            new_content = license_text + content[end:]
            file.write_text(new_content)
            print(f'Modified {file} with new license header')
            success += 1
        else:
            content_lines = content.splitlines()
            end = 0
            for line in content_lines:
                if line.startswith('//'):
                    end += 1
                else:
                    break
            new_content = license_text + '\n'.join(content_lines[end:]) + '\n'
            file.write_text(new_content)
            success += 1
    
    if success != count:
        print('ERROR during license replacement')
    else:
        print('Everything went well')

    manifest_files = [file for file in working_dir.rglob('*') if file.name == 'manifest.yaml']
    for file in manifest_files:
        manifest = file.read_text()
        needle = 'license:'
        start = manifest.find(needle)
        end = manifest.find('\n', start+len(needle))
        new_manifest = manifest[:start] + 'license: https://opensource.org/licenses/Apache-2.0' + manifest[end:]
        file.write_text(new_manifest)


if __name__ == '__main__':
    main()
