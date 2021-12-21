#!/bin/bash
##
## SPDX-License-Identifier: Apache-2.0
## Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
##
LD_LIBRARY_PATH=`pwd`/_deps/everest-framework-build/lib gdb --args dist/bin/main --schemasdir dist/schemas --conf ../config/config-hil.json --modulesdir dist/modules --classesdir dist/interfaces --module $1  --logconf ../config/logging.ini
