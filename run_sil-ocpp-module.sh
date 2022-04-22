##
## SPDX-License-Identifier: Apache-2.0
## Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
##
LD_LIBRARY_PATH=`pwd`/_deps/everest-framework-build/lib \
dist/modules/OCPP/OCPP \
--log_conf ../config/logging.ini \
--main_dir dist \
--conf ../config/config-sil-ocpp.json \
--module charge_point
