#!/bin/sh

set -e

ninja -j$(nproc) -C "$EXT_MOUNT/build" test
