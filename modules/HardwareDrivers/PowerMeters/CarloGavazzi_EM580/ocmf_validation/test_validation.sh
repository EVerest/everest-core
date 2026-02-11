#!/usr/bin/env bash
# Quick test script for OCMF validation
#
# Usage:
#   1. Edit this script to set your PUBLIC_KEY and OCMF_DATA_FILE
#   2. Run: ./test_validation.sh
#
# Or set environment variables:
#   PUBLIC_KEY="04..." OCMF_DATA_FILE="path/to/ocmf.txt" ./test_validation.sh

# Default values - edit these or set as environment variables
PUBLIC_KEY="${PUBLIC_KEY:-04521C09090AB6A2826A613D36483A71F789F6C0D900F9A9106415EA8BE3F6AFEB5926B39E264CB3727647DA49B153370221F18048B343AC0318203F7043F840CD8BB5C9C6734C0DB46B19711AD94A0DB8F1FA854E2D60D25B33D7DDE145F61E6C}"
OCMF_DATA_FILE="${OCMF_DATA_FILE:-./text.txt}"

# Check if OCMF data file exists
if [ ! -f "$OCMF_DATA_FILE" ]; then
    echo "Error: OCMF data file not found: $OCMF_DATA_FILE"
    echo "Please set OCMF_DATA_FILE environment variable or edit this script."
    exit 1
fi

OCMF_DATA=$(cat "$OCMF_DATA_FILE")

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

python3 "$SCRIPT_DIR/validate_ocmf_signature.py" \
    --public-key "$PUBLIC_KEY" \
    --ocmf-string "$OCMF_DATA"
