#!/bin/sh

ninja -j$(nproc) -C build test
