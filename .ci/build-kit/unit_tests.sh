#!/usr/bin/env sh
set -ex

ninja -j$(nproc) -C build tests/test
