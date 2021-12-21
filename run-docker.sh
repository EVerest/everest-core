#!/bin/bash
##
## SPDX-License-Identifier: Apache-2.0
## Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
##
./dist/bin/charge_point \
    --maindir ./dist/ocpp \
    --conf config-docker.json
