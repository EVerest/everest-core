#!/usr/bin/env -S python3 -tt
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#

import sys
import yaml
from pathlib import Path

def main():
  input = Path(sys.argv[1])

  config_as_dict = yaml.safe_load(input.read_text())

  new_config = {
    "active_modules": {},
    "x-module-layout": {}
  }

  for mod, mod_def in config_as_dict.items():
    if "x-view-model" in mod_def:
      new_config["x-module-layout"][mod] = mod_def["x-view-model"]
      del mod_def["x-view-model"]

    new_config["active_modules"][mod]  = mod_def

  yaml.safe_dump(new_config, sys.stdout, indent=2, sort_keys=False, width=120)

if __name__ == '__main__':
  main()
