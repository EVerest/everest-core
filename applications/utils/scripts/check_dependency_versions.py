#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: kai-uwe.hermann@pionix.de
Parse snapshot.yaml files check if dependency versions are up2date by checking for newer git tags
"""
import argparse
import os
import re
import yaml
import subprocess
from packaging.version import Version

def get_remote_tags(remote_url: str) -> list:
    """Return the remote tags of the repo at path, or an empty list."""
    remote_tags = []
    try:
        result = subprocess.run(["git", "-c", "versionsort.suffix=-", "ls-remote", "--tags", "--sort=-v:refname", "--refs", "--quiet", remote_url],
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=True)
        result_list = result.stdout.decode("utf-8").split("\n")
        for entry in result_list:
            ref_and_tag = entry.split("\t")
            if len(ref_and_tag) > 1:
                remote_tags.append(ref_and_tag[1].replace("refs/tags/", ""))
    except subprocess.CalledProcessError:
        return remote_tags

    return remote_tags


def parse_curl_version(curl_version):
    parsed_curl_version = curl_version.removeprefix("tiny-curl-")
    parsed_curl_version = parsed_curl_version.removeprefix("curl-")
    parsed_curl_version = parsed_curl_version.replace("_", ".")
    return parsed_curl_version


def main():
    parser = argparse.ArgumentParser(
        description='get updated dependency versions from snapshot.yaml')

    parser.add_argument('--input',
                        dest='in_snapshot',
                        help='Path to the snapshot.yaml file')

    args = parser.parse_args()

    in_snapshot = os.path.realpath(os.path.expanduser(args.in_snapshot))
    
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

    branch_re = re.compile(r'branch=([^;|^"|\s]*)')

    for key, entry in snapshot.items():
        branch = entry['branch']
        git_rev = entry['git_rev']
        git_tag = entry['git_tag']
        version = git_tag.removeprefix('wip-release-')
        version = version.removeprefix('v')
        version_without_rc, _vsep, _rc = version.partition('-rc')
        # todo: maybe even strip something like "wip-release-" from git_tag to keep versions sane?
        # or just do not modify the version if this cannot be parsed properly?
        # print(f"version: {version} without-rc: {version_without_rc}")
        git_url = entry['git']
        remote_tags = get_remote_tags(git_url)
        tags = []
        for original_remote_tag in remote_tags:
            remote_tag = original_remote_tag.removeprefix("v")
            if key == "libcurl":
                # special handling for curl
                remote_tag = parse_curl_version(remote_tag)
            if key == "pugixml":
                if remote_tag == "latest":
                    continue

            tags.append((original_remote_tag, remote_tag))

        remote_versions = []
        version_mapping = {}
        for (original_remote_tag, remote_tag) in tags:
            try:
                ver = Version(remote_tag)
                version_mapping[ver] = original_remote_tag
                remote_versions.append(ver)
            except Exception as e:
                pass

        remote_versions.sort(reverse=True)

        try:
            remote_version = remote_versions[0]
            parsed_version = ""
            if key == "libcurl":
                parsed_version = Version(parse_curl_version(version))
            else:
                parsed_version = Version(version)
            if parsed_version != remote_version:
                print(f"  {key}: remote version is different from version in snapshot! Check if you want to update.")
                print(f"    snapshot version: {version}")
                print(f"    remote version:   {remote_version}, original tag: {version_mapping[remote_version]}")
        except Exception as e:
            print(f"  {key}: cannot parse remote version, please check manually!")
            print(f"    snapshot version: {version}")
            print(f"    thrown exception: {e}")


if __name__ == '__main__':
    main()
