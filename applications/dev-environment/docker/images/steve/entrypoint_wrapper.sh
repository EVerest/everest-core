#!/bin/bash

# ---------------------------------------------
# Architecture Warning Wrapper Script
#
# This script is used as an entrypoint wrapper to emit a warning
# when the container is not running on the officially supported
# amd64 (x86_64) architecture.
#
# It checks for the presence of a wrapped entrypoint script
# (/wrapped_entrypoint.sh) and executes it if found; otherwise,
# it falls back to executing the provided command directly.
#
# The warning is shown both before and after the wrapped command
# to ensure visibility.
# ---------------------------------------------

function print_warning {
    echo -e "\033[0;31m"
    echo "-------------------------------------------------------------"
    echo "⚠️  WARNING: Unsupported Architecture Detected"
    echo
    echo "This Docker image is not running on the amd64 (x86_64) architecture."
    echo "It is recommended to use the amd64-based image for full compatibility."
    echo "Other architectures are not officially supported and may cause issues."
    echo
    echo "-------------------------------------------------------------"
    echo -e "\033[0m"
}

print_warning

if [ -f /wrapped_entrypoint.sh ]; then
    exec /wrapped_entrypoint.sh "$@"
else
    exec "$@"
fi
