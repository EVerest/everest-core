#!/usr/bin/env sh
set -x

ninja -j$(nproc) -C build tests/test
