#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: kai-uwe.hermann@pionix.de
Parse snapshot.yaml files and modify corresponding .bb recipes
"""
import argparse
import os
import re
import yaml
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(
        description='modify .bb files based on a snapshot')

    parser.add_argument('--input',
                        dest='in_snapshot',
                        help='Path to the snapshot.yaml file')
    parser.add_argument('--out',
                        dest='out_dir',
                        help='Relative path to meta-everest')

    args = parser.parse_args()

    in_snapshot = os.path.realpath(os.path.expanduser(args.in_snapshot))
    out_dir = Path(os.path.realpath(os.path.expanduser(args.out_dir)))
    
    snapshot = None
    with open(in_snapshot, encoding='utf-8') as snapshot_file:
        try:
            snapshot = yaml.safe_load(snapshot_file)
        except yaml.YAMLError as e:
            print(f"Error parsing yaml of {in_snapshot}: {e}")
            return

    if not snapshot:
        print(f"snapshot empty?")
        return

    output = {}

    print(f"got snapshot: {snapshot}")

    recipes_core = out_dir / 'recipes-core'
    if not recipes_core.exists():
        print(f"cannot find recipes-core in {recipes_core}")
        return

    # TODO: provide this via a file as a command line parameter
    mapping = {'Josev': 'josev/python3-iso15118',
               'everest-utils': 'everest-devtools/evcli',
               'everest-core': 'everest/everest-core',
               'everest-framework': 'everest/everest-framework',
               'everest-sqlite': 'everest/everest-sqlite',
               'libcbv2g': 'everest/libcbv2g',
               'libevse-security': 'everest/libevse-security',
               'libfsm': 'everest/libfsm',
               'libiso15118': 'everest/libiso15118',
               'liblog': 'everest/liblog',
               'libnfc-nci': 'everest/libnfc-nci',
               'libocpp': 'everest/libocpp',
               'libslac': 'everest/libslac',
               'libtimer': 'everest/libtimer'}

    branch_re = re.compile(r'branch=([^;|^"|\s]*)')

    for key, entry in snapshot.items():
        print(f"key: {key}")
        branch = entry['branch']
        git_rev = entry['git_rev']
        git_tag = entry['git_tag']
        version = git_tag.removeprefix('wip-release-')
        version = version.removeprefix('v')
        version_without_rc, _vsep, _rc = version.partition('-')
        # todo: maybe even strip something like "wip-release-" from git_tag to keep versions sane?
        # or just do not modify the version if this cannot be parsed properly?
        print(f"version: {version} without-rc: {version_without_rc}")
        if key in mapping:
            file_glob = f'{mapping[key]}_*.bb'
            files = list(recipes_core.glob(file_glob))
            if len(files) == 1:
                bb_path = files[0]
                
                print(f"found a matching file: {bb_path}")
                with open(bb_path) as bb_file:
                    bb_content = bb_file.read().splitlines()
                    bb_content_modified = []
                    for index, line in enumerate(bb_content):
                        if line.startswith("SRC_URI"):
                            def replace(match):
                                return f'branch={branch}'
                            line = branch_re.sub(replace, line)
                        elif line.startswith("SRCREV"):
                            line = f"SRCREV = \"{git_rev}\""
                        
                        bb_content_modified.append(line)
                    # create new filename
                    prefix, sep, postfix = bb_path.name.partition('_')
                    new_filename = f'{prefix}_{version_without_rc}.bb'
                    new_bb_path = bb_path.parent / new_filename
                    if bb_path != new_bb_path:
                        print(f"filename is different, so remove the old one")
                        bb_path.unlink(missing_ok=True)

                    print(f"writing bb file: {new_bb_path}")
                    with open(new_bb_path, mode='w+') as new_bb_file:
                        new_bb_file.write('\n'.join(bb_content_modified))
                        new_bb_file.write('\n')


if __name__ == '__main__':
    main()
