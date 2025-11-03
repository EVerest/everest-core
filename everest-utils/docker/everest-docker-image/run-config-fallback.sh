#!/bin/bash

#!/bin/bash

usage() {
    echo "Usage: $0 [--conf <string>] [--ocpp-conf <string>] [--init]" 1>&2
    echo -e "\t--conf: Path to EVerest config file - Optional, defaults to config-fallbackyaml"
    echo -e "\t--ocpp-conf: Path to EVerest OCPP config file - Optional, defaults to ocpp-config.json"
    echo -e "\t--init: If the OCPP 2.0.1 device model should be re-initialized with the given OCPP config - Optional"
    exit 1
}

export EVEREST_CONFIG=config-fallbackyaml.yaml
export OCPP_CONFIG=ocpp-config.json
export EVEREST_COMMAND="sh -c '/opt/everest/bin/manager --conf /opt/everest/config/config.yaml'"

while [ ! -z "$1" ]; do
    if [ "$1" == "--conf" ]; then
        export EVEREST_CONFIG="${2}"
        shift 2
    elif [ "$1" == "--ocpp-conf" ]; then
        export OCPP_CONFIG="${2}"
        shift 2
    elif [ "$1" == "--init" ]; then
        export EVEREST_COMMAND="sh -c '/opt/everest/initialize.sh'"
        shift 1
    else
        usage
        break
    fi
done

docker compose up
