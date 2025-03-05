# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest

import os,sys
import pathlib

def insert_dir_to_sys_path(directory: pathlib.Path):
    """
    This helper allows to insert a directory to the sys.path.
    """
    if not os.path.exists(directory):
        raise FileNotFoundError(f"Directory {directory} does not exist")
    directory = directory.resolve().as_posix()
    if directory in sys.path:
        return
    sys.path.insert(0, directory)
