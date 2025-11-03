#!/bin/bash
set -e

# Skip the Rust setup until we can properly support Rust.
# Install Rust
# curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
# source "$HOME/.cargo/env"

cmake \
    -B "$EXT_MOUNT/build" \
    -S "$EXT_MOUNT/source" \
    -G Ninja \
    -DEVC_ENABLE_CCACHE=1 \
    -DBUILD_TESTING=ON \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DEVEREST_ENABLE_RS_SUPPORT=OFF # Turn off rust support until we can properly support Rust.

retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Configuring failed with return code $retVal"
    exit $retVal
fi

ninja -C "$EXT_MOUNT/build"
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "Compiling failed with return code $retVal"
    exit $retVal
fi
