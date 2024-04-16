#!/bin/sh

ninja -j$(nproc) -C "$EXT_MOUNT/build" test
