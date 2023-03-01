#!/bin/bash

usage() {
    echo "Usage: $0 [--repo <string>] [--branch <string>] [--conf <string>] [--ocpp-conf <string>] [--name <string>]" 1>&2
    echo -e "\t--repo: Git repository - Optional, defaults to: https://github.com/EVerest/everest-core.git"
    echo -e "\t--branch: Git branch or tag name - Optional, defaults to: main"
    echo -e "\t--conf: Path to EVerest config file - Required"
    echo -e "\t--ocpp-conf: Path to EVerest OCPP config file - Optional, no default"
    echo -e "\t--name: Name of the docker image - Optional, defaults to: everest-core"
    exit 1
}

while [ ! -z "$1" ]; do
    if [ "$1" == "--repo" ]; then
        repo="${2}"
        shift 2
    elif [ "$1" == "--conf" ]; then
        conf="${2}"
        shift 2
    elif [ "$1" == "--ocpp-conf" ]; then
        ocpp_conf="${2}"
        shift 2
    elif [ "$1" == "--name" ]; then
        name="${2}"
        shift 2
    elif [ "$1" == "--branch" ]; then
        branch="${2}"
        shift 2
    else
        usage
        break
    fi
done

if [ -z "${repo}" ]; then
    repo="https://github.com/EVerest/everest-core.git"
fi

if [ -z "${conf}" ]; then
    usage
else
    cp "${conf}" user_config.yaml
    conf="user_config.yaml"
fi

if [ -z "${ocpp_conf}" ]; then
    touch ocpp_user_config.json
    ocpp_conf="ocpp_user_config.json"
    echo "{}" > $ocpp_conf
else
    cp "${ocpp_conf}" ocpp_user_config.json
    ocpp_conf="ocpp_user_config.json"
fi

if [ -z "${name}" ]; then
    name="everest-core"
fi

if [ -z "${branch}" ]; then
    branch="main"
fi


NOW=$(date +"%Y-%m-%d-%H-%M-%S")
DOCKER_BUILDKIT=1 docker build \
    --build-arg BUILD_DATE="${NOW}" \
    --build-arg REPO="${repo}" \
    --build-arg EVEREST_CONFIG="${conf}" \
    --build-arg OCPP_CONFIG="${ocpp_conf}" \
    --build-arg BRANCH="${branch}" \
    -t "${name}" --ssh default .
docker save "${name}":latest | gzip >"$name-${NOW}.tar.gz"
