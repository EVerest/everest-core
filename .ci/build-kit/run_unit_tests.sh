#!/bin/sh

set -e

ninja -C "$EXT_MOUNT/build" test
