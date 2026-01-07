#!/bin/bash
##
## SPDX-License-Identifier: Apache-2.0
## Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
##
echo "generating bash-completion file"
SRC_DIR="$(dirname "${BASH_SOURCE[0]}")/src"
echo "Using module found in ${SRC_DIR}"
cd "${SRC_DIR}"
BASH_COMPLETION_FILE_DIR="$(pwd)"
BASH_COMPLETION_FILE="${BASH_COMPLETION_FILE_DIR}/edm_tool/edm-completion.bash"
shtab --shell=bash -u edm_tool.get_parser --prog edm > "${BASH_COMPLETION_FILE}"