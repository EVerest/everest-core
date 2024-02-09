#!/bin/bash
set -e

ninja -j$(nproc) -C build test
