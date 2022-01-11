##
## SPDX-License-Identifier: Apache-2.0
## Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
##
LD_LIBRARY_PATH=`pwd`/_deps/everest-framework-build/lib \
dist/bin/main \
--logconf ../config/logging.ini \
--maindir dist \
--schemasdir dist/schemas \
--conf ../config/config-sil-ocpp.json \
--modulesdir dist/modules \
--classesdir dist/interfaces \
--module charge_point
