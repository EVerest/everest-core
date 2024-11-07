#!/bin/bash

usage() {
    echo "Usage: $0 <everest-core-directory>"
    exit 1
}

if [ $# -ne 1 ] ; then
    usage
else
    EVEREST_OCPP_CONFIGS_PATH="$1"/build/dist/share/everest/modules/OCPP
    mkdir -p $EVEREST_OCPP_CONFIGS_PATH

    cp config/libocpp-config-* $EVEREST_OCPP_CONFIGS_PATH
fi
