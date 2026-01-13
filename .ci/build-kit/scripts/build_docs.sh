#!/bin/sh

TRAILBOOK_everest_IS_RELEASE=${TRAILBOOK_everest_IS_RELEASE:-"OFF"}
TRAILBOOK_everest_INSTANCE_NAME=${TRAILBOOK_everest_INSTANCE_NAME:-"nightly"}
TRAILBOOK_everest_OVERWRITE_EXISTING_INSTANCE=${TRAILBOOK_everest_OVERWRITE_EXISTING_INSTANCE:-"OFF"}

mkdir -p ~/.ssh
ssh-keyscan github.com >> ~/.ssh/known_hosts
chmod 600 ~/.ssh/known_hosts

cmake \
    -B "$EXT_MOUNT/build" \
    -S "$EXT_MOUNT/source" \
    -G Ninja \
    -D EVC_ENABLE_CCACHE=ON \
    -D EVEREST_ENABLE_COMPILE_WARNINGS=ON \
    -D EVEREST_BUILD_DOCS=ON \
    -D TRAILBOOK_everest_DOWNLOAD_ALL_VERSIONS=ON \
    -D TRAILBOOK_everest_IS_RELEASE="$TRAILBOOK_everest_IS_RELEASE" \
    -D TRAILBOOK_everest_INSTANCE_NAME="$TRAILBOOK_everest_INSTANCE_NAME" \
    -D TRAILBOOK_everest_OVERWRITE_EXISTING_INSTANCE="$TRAILBOOK_everest_OVERWRITE_EXISTING_INSTANCE" \
    -D EVEREST_DOCS_REPO_URL="$EVEREST_DOCS_REPO_URL"
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Configuring failed with return code $retVal"
    exit $retVal
fi

ninja -C "$EXT_MOUNT/build" trailbook_everest
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Compiling failed with return code $retVal"
    exit $retVal
fi
