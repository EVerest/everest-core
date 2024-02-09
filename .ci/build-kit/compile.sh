#!/bin/bash
set -e

# Skip the Rust setup until we can properly support Rust.
# Install Rust
# curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
# source "$HOME/.cargo/env"

cmake \
    -B build \
    -S "$EXT_MOUNT/source" \
    -G Ninja \
    -DEVC_ENABLE_CCACHE=1 \
    -DBUILD_TESTING=ON \
    -DEVEREST_ENABLE_RS_SUPPORT=OFF # Turn off rust support until we can properly support Rust.

ninja -j$(nproc) -C build
