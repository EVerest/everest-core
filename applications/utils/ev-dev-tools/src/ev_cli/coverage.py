# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: maaike@pionix.de
With this script, orphaned object (.o) and gcovr (gcno) files can be removed. This is especially necessary when branches
are switched and there is an orphaned object / gcno file. gcovr will then give an error. When gcno and object files
of the orphaned class are removed, gcovr will run fine.
This script will also remove all gcda files from the given build directory.
"""

import os
import glob
from pathlib import Path, PurePosixPath
import argparse
import shutil
import subprocess
import re
from dataclasses import dataclass
# from ev_coverage import __version__


@dataclass
class RemovedFiles:
    """Keeps track of the amount of deleted files of different categories."""
    gcda: int
    orphaned_object: int


def log_wrapper(message: str, silent: bool):
    if not silent:
        print(f'{message}')


def remove_wrapper(filepath: Path, dry_run=True, silent=False):
    if dry_run:
        log_wrapper(f'(dry-run) did not remove: {filepath}', silent)
        return
    os.remove(filepath)


def remove_all_gcda_files(build_dir: Path, dry_run: bool, silent: bool) -> int:
    log_wrapper('Removing all gcda files from build directory', silent)

    return len([remove_wrapper(f, dry_run, silent) for f in build_dir.rglob('*.gcda')])


def remove_orphaned_object_files(build_dir: Path, use_dwarfdump: bool, dry_run: bool, silent: bool) -> int:
    log_wrapper('Removing orphaned object files and gcno files from build directory', silent)
    # Find all object files (*.o) in the build directory
    object_files = glob.glob(f'{build_dir.as_posix()}/**/*.o', recursive=True)
    removed_files = 0

    if not use_dwarfdump:
        log_wrapper(
            'Using GDB to check for object files. This is way slower than using dwarfdump. You can install dwarfdump '
            '(llvm-dwarfdump) to speed up the process', False)

    for obj_file in object_files:
        cpp_file_exists = False

        full_path = PurePosixPath(obj_file)

        # Get the base name of the object file (remove the path)
        # obj_basename = os.path.basename(obj_file)
        obj_basename = full_path.name
        obj_dir = full_path.parent

        # Convert the object file base name to the corresponding cpp file name
        cpp_basename = obj_basename[:-2] \
            if obj_basename.endswith('cpp.o') or obj_basename.endswith('c.o') or obj_basename.endswith('cc.o') \
            else obj_basename.replace('.o', '.cpp')

        if use_dwarfdump:
            # Get the source file belonging to the object file
            command_dwarfdump = f'llvm-dwarfdump --show-sources {obj_file} | grep {cpp_basename}'
            result = subprocess.run([command_dwarfdump], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
                                    shell=True).stdout.decode('utf-8')

            # This will return some information, which sometimes is just the file and sometimes a list of files,
            # one per line. The latter should be split.
            paths = re.split('\n', result)
        else:
            # Get the source file belonging to the object file
            command = f'/usr/bin/gdb -q -ex "set height 0" -ex "info sources" -ex quit {obj_file} | grep {cpp_basename}'

            result = subprocess.run([command], stdout=subprocess.PIPE, shell=True).stdout.decode('utf-8')

            # This will return some information, including a comma separated list of files on one line. So this should
            # be split.
            paths = re.split('\n|,|:', result)
            # Loop over all strings read from the gdb command, which includes paths. If the string ends with the name
            # of the file, then it should be the source

        for path in paths:
            path = path.strip()
            if path.endswith(cpp_basename) and os.path.isfile(path):
                # Found path, do not remove!
                cpp_file_exists = True
                break

        # If no corresponding .cpp file was found, remove the orphaned .o file
        if not cpp_file_exists:
            log_wrapper(f'Removing orphaned object file: {obj_file}', silent)
            for p in Path(obj_dir).glob(obj_basename + "*"):
                # for p in pathlib.Path(build_dir).rglob(relative_path_cpp + '*'):
                if os.path.isfile(p):
                    log_wrapper(f'Removing other orphaned file: {p}', silent)
                    remove_wrapper(p, dry_run, silent)
                    removed_files += 1
            # If obj file is not removed in the previous loop, remove it now
            if os.path.isfile(obj_file):
                log_wrapper(f'Removing object file: {obj_file}', silent)
                remove_wrapper(obj_file, dry_run, silent)
                removed_files += 1
    return removed_files


def remove_unnecessary_files(build_dir: Path, dry_run: bool, summary: bool, silent: bool) -> bool:
    use_dwarfdump = shutil.which('llvm-dwarfdump')
    gdb_exists = shutil.which('gdb')

    if not gdb_exists and not use_dwarfdump:
        log_wrapper('GDB and dwarfdump do not exist. Install one of them to be able to run this script', False)
        return False

    removed_files = RemovedFiles(0, 0)
    removed_files.gcda = remove_all_gcda_files(build_dir=build_dir, dry_run=dry_run, silent=silent)
    removed_files.orphaned_object = remove_orphaned_object_files(
        build_dir=build_dir, use_dwarfdump=use_dwarfdump, dry_run=dry_run, silent=silent)
    if summary:
        prefix = '(dry-run) Would have removed' if dry_run else 'Removed'
        print(f'{prefix} {removed_files.gcda} .gcda files and {removed_files.orphaned_object} orphaned object files')

    if not use_dwarfdump:
        log_wrapper(
            '\n === Did that take a longggg time? Install dwarfdump (llvm0-dwarfdump) to get quicker results '
            'next time! === \n', False)
