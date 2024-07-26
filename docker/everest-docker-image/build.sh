#!/bin/bash

usage() {
    echo "Usage: $0 [--repo <string>] [--branch <string>] [--conf <string>] [--ocpp-conf <string>] [--name <string>]" 1>&2
    echo -e "\t--repo: Git repository - Optional, defaults to: https://github.com/EVerest/everest-core.git"
    echo -e "\t--branch: Git branch or tag name - Optional, defaults to: main"
    echo -e "\t--conf: Path to EVerest config file - Required"
    echo -e "\t--ocpp-conf: Path to EVerest OCPP config file - Optional, no default"
    echo -e "\t--name: Name of the docker image - Optional, defaults to: everest-core"
    echo -e "\t--build-date: Build date of the docker image, is reflected in its name and can have an effect on caching - Optional, defaults to the current datetime"
    echo -e "\t--no-ssh: Do not append \"--ssh default\" to docker build - Optional"
    echo -e "\t--container-runtime: Set container runtime (e.g. docker or podman) for build - Optional"
    echo -e "\t--additional-cmake-parameters: Set additional cmake parameters for build - Optional, default \"-DEVEREST_BUILD_ALL_MODULES=ON\""
    exit 1
}

ssh_param="--ssh=default"
container_runtime="docker"
additional_cmake_parameters="-DEVEREST_BUILD_ALL_MODULES=ON"

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
    elif [ "$1" == "--build-date" ]; then
        build_date="${2}"
        shift 2
    elif [ "$1" == "--no-ssh" ]; then
        ssh_param=""
        shift 1
    elif [ "$1" == "--container-runtime" ]; then
        container_runtime="${2}"
        shift 2
    elif [ "$1" == "--additional-cmake-parameters" ]; then
        additional_cmake_parameters="${2}"
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

if [ -n "${build_date}" ]; then
    NOW="${build_date}"
fi

echo "Build date: ${NOW}"
echo "Using container runtime \"${container_runtime}\" for building. Version: $(${container_runtime} --version)"
echo "Additional CMake parameters for EVerest build: \"${additional_cmake_parameters}\""
trap 'echo "Build not successful"; exit 1' ERR
DOCKER_BUILDKIT=1 ${container_runtime} build \
    --build-arg BUILD_DATE="${NOW}" \
    --build-arg REPO="${repo}" \
    --build-arg EVEREST_CONFIG="${conf}" \
    --build-arg OCPP_CONFIG="${ocpp_conf}" \
    --build-arg BRANCH="${branch}" \
    --build-arg ADDITIONAL_CMAKE_PARAMETERS="${additional_cmake_parameters}" \
    -t "${name}" "${ssh_param}" .
${container_runtime} save "${name}":latest | gzip >"$name-${NOW}.tar.gz"
