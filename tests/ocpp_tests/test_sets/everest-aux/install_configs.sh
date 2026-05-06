#!/usr/bin/env bash

usage() {
    echo "Usage: $0 <EVerest-installation-directory>"
    exit 1
}

if [ $# -ne 1 ] ; then
    usage
else
    EVEREST_OCPP_CONFIGS_PATH="$1/share/everest/modules/OCPP"
    mkdir -p "$EVEREST_OCPP_CONFIGS_PATH"

    cp config/libocpp-config-* "$EVEREST_OCPP_CONFIGS_PATH"

    EVEREST_OCPP201_CC_CUSTOM_PATH="$1/share/everest/modules/OCPP201/component_config/custom"
    mkdir -p "$EVEREST_OCPP201_CC_CUSTOM_PATH"
    cp component_config/custom/DCDERCtrlr_1.json "$EVEREST_OCPP201_CC_CUSTOM_PATH/"
fi
