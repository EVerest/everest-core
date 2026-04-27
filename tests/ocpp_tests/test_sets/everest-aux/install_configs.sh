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

    # OCPP 2.x per-EVSE custom device-model component configs. libocpp's
    # LIBOCPP_INSTALL_CUSTOM_COMPONENT_CONFIG defaults to OFF, so these are
    # not installed by `ninja install` — tests that need them (e.g. R04 DER
    # Control) must stage them here. Only stage the DER controller config;
    # EVSE_*/Connector_* custom files are not needed for the test harness.
    EVEREST_OCPP201_CUSTOM_PATH="$1/share/everest/modules/OCPP201/component_config/custom"
    mkdir -p "$EVEREST_OCPP201_CUSTOM_PATH"
    cp component_config/custom/DCDERCtrlr_1.json "$EVEREST_OCPP201_CUSTOM_PATH"
fi
