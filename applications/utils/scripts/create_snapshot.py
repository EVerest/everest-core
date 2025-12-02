#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: kai-uwe.hermann@pionix.de
Use edm to create a snapshot of the current directory without polluting the current working dir
"""

import argparse
import yaml
import subprocess
from pathlib import Path
import shutil
import string


def get_tags(path: Path) -> list:
        """Return a list of tags the HEAD points to of the repo at path, or an empty list."""
        tags = []
        try:
            result = subprocess.run(["git", "-C", path, "tag", "--points-at", "HEAD"],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
            tags = result.stdout.decode("utf-8").splitlines()
        except subprocess.CalledProcessError:
            return tag

        return tags


def main():
    parser = argparse.ArgumentParser(
        description='create an isolated snapshot with edm')

    parser.add_argument('--working-dir', '-wd', type=str,
                        help='Working directory containing the EVerest workspace (default: .)', default=str(Path.cwd()))
    parser.add_argument('--temp-dir', '-td', type=str,
                        help='Temporary directory for creating the snapshot in (default: working-dir/tmp-for-snapshot)', default=None)
    parser.add_argument('--version', type=str,
                        help='dependency version to override, format is: dependency1:version,dependency2:version2', default=None)
    parser.add_argument('--git-version', action='store_true', help='Use "git" as version when encountering a git hash')
    parser.add_argument('--allow-relative-to-working-dir', action='store_true', help='Allow temporary directory to be relative to working dir (dangerous!)')
    parser.add_argument('--post-process', action='store_true', help='Postprocess existing snapshot')
    parser.add_argument('--include-external-deps', action='store_true', help='Include external dependencies in snapshot')

    args = parser.parse_args()

    working_dir = Path(args.working_dir).expanduser().resolve()

    tmp_dir = working_dir / 'tmp-for-snapshot'

    if args.temp_dir:
        tmp_dir = Path(args.temp_dir).expanduser().resolve()

    if working_dir == tmp_dir:
        print(f'Temporary directory cannot be equal to working directory: {tmp_dir}')
        return 1

    if tmp_dir.is_relative_to(working_dir) and tmp_dir.parent != working_dir and not args.allow_relative_to_working_dir:
        print(f'Temporary directory cannot be relative to working directory: {tmp_dir}')
        return 1

    if not args.post_process and tmp_dir.exists():
        print(f'Temporary directory dir already exists, deleting it: {tmp_dir}')
        shutil.rmtree(tmp_dir, ignore_errors=True)
    if not args.post_process:
        tmp_dir.mkdir()

        subdirs = list(working_dir.glob('*/'))
        for subdir in subdirs:
            subdir_path = Path(subdir)
            if not subdir_path.is_dir():
                print(f'{subdir_path} is not a dir, ignoring')
                continue
            if subdir_path == tmp_dir:
                print(f'{subdir_path} is tmp dir, ignoring')
                continue
            print(f'Copying {subdir_path} to {tmp_dir}')
            destdir = tmp_dir / subdir_path.name

            shutil.copytree(subdir_path, destdir, ignore=shutil.ignore_patterns('build*'))

        print('Running edm snaphot --recursive')
        cmd_line = ['edm']
        if args.include_external_deps:
            cmd_line.append('--external-in-config')
        cmd_line.extend(['snapshot', '--recursive'])
        with subprocess.Popen(cmd_line, stderr=subprocess.PIPE, cwd=tmp_dir) as edm:
            for line in edm.stderr:
                print(line.decode('utf-8'), end='')
    in_snapshot = tmp_dir / 'snapshot.yaml'
    snapshot = None
    with open(in_snapshot, mode='r', encoding='utf-8') as snapshot_file:
        try:
            snapshot = yaml.safe_load(snapshot_file)
        except yaml.YAMLError as e:
            print(f'Error parsing yaml of {in_snapshot}: {e}')
    if snapshot:
        # check if git_tag is a 40 character hex string and assume it is a git_rev
        if args.git_version:
            for dependency, entry in snapshot.items():
                git_tag = ''
                if 'git_tag' in entry:
                    git_tag = entry['git_tag']
                elif 'git_rev' in entry:
                    git_tag = entry['git_rev']
                if len(git_tag) == 40 and all(character in string.hexdigits for character in git_tag):
                    snapshot[dependency]['git_tag'] = 'git'
        if args.version:
            versions = args.version.split(',')
            for dep_versions in versions:
                dependency, version = dep_versions.split(':')
                if dependency in snapshot:
                    print(f'Overriding {dependency} version {snapshot[dependency]['git_tag']} to {version}')
                    snapshot[dependency]['git_tag'] = version
        for dependency, entry in snapshot.items():
            git_tag = ''
            if 'git_tag' in entry:
                git_tag = entry['git_tag']
            if git_tag == 'latest':
                print(f'{dependency} has tag "latest", check if there is a version tag as well')
                dependency_path = tmp_dir / dependency
                tags = get_tags(dependency_path)
                tags.remove('latest')
                if len(tags) == 1:
                    print('  Fixing "latest" tag in snapshot')
                    snapshot[dependency]['git_tag'] = tags[0]
                else:
                    print(f'  List of tags with "latest" removed: "{tags}" is not directly usable...')

        with open(in_snapshot, mode='w', encoding='utf-8') as snapshot_file:
            yaml.safe_dump(snapshot, snapshot_file, indent=2, sort_keys=False, width=120)
    print('Done')


if __name__ == '__main__':
    main()
