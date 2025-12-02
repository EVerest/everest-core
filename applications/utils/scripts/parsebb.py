#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: kai-uwe.hermann@pionix.de
Parse .bb files and extract git information as json
"""
import argparse
import json
import os
import re


def main():
    parser = argparse.ArgumentParser(
        description='extracts git url and rev from .bb files')

    parser.add_argument('--input',
                        dest='in_bb',
                        help='Path to the .bb file')
    parser.add_argument('--file',
                        dest='in_file',
                        help='Relative path to the repo')

    args = parser.parse_args()

    in_bb = os.path.realpath(os.path.expanduser(args.in_bb))
    in_rel_path = args.in_file

    bb = None
    try:
        bb_file = open(in_bb, 'r')
    except Exception:
        return

    output = {}

    with bb_file as f:
        bb = f.read()
        bbvars = re.findall(r'^.+=[^=]+\n', bb, flags=re.MULTILINE)
        for variable in bbvars:
            variable_search = re.search(r'([a-zA-Z_]*)\s?=\s?\"([^\s]*)', variable)
            if variable_search:
                name = variable_search.group(1)
                value = variable_search.group(2)
                if name == 'SRC_URI':
                    src_uri_search = re.search(r'git:\/\/([^;]*);branch=([^;]*)', value)
                    if src_uri_search:
                        repo = f'https://{src_uri_search.group(1)}'
                        branch = src_uri_search.group(2)
                        output['repo'] = repo
                        output['branch'] = branch
                if name == 'SRCREV':
                    src_rev_search = re.search(r'([^;]*)\"', value)
                    if src_rev_search:
                        rev = src_rev_search.group(1)
                        output['rev'] = rev
    output['url'] = output['repo'].replace('.git', '/').replace('github.com/', 'raw.githubusercontent.com/')
    output['url'] += output['rev'] + '/' + in_rel_path
    print(f'{json.dumps(output)}')


if __name__ == '__main__':
    main()
