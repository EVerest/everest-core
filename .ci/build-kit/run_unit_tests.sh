#!/bin/sh

set -e

ninja -j$(nproc) -C build test
