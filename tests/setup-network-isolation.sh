#!/bin/bash
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
# Setup/teardown script for virtual ethernet pairs used in parallel ISO 15118 testing.
#
# Usage:
#   ./setup-network-isolation.sh setup [COUNT]   - Create COUNT veth pairs (default: nproc)
#   ./setup-network-isolation.sh teardown [COUNT] - Remove COUNT veth pairs (default: 64)
#   ./setup-network-isolation.sh status           - Show current veth pair status
#
# Requires: root/sudo or CAP_NET_ADMIN

set -euo pipefail

VETH_PREFIX="ev_test"

setup() {
    local count="${1:-$(nproc)}"
    echo "Creating $count veth pairs (${VETH_PREFIX}0..${VETH_PREFIX}$((count - 1)))..."

    for i in $(seq 0 $((count - 1))); do
        local iface="${VETH_PREFIX}${i}"
        local peer="${VETH_PREFIX}${i}_peer"

        # Remove if leftover from previous run
        ip link delete "$iface" 2>/dev/null || true

        ip link add "$iface" type veth peer name "$peer"
        ip link set "$iface" up
        ip link set "$peer" up
    done

    echo "Done."
}

teardown() {
    local max_index="${1:-64}"

    local removed=0
    for i in $(seq 0 $((max_index - 1))); do
        local iface="${VETH_PREFIX}${i}"
        if ip link show "$iface" &>/dev/null; then
            ip link delete "$iface"
            removed=$((removed + 1))
        fi
    done

    echo "Removed $removed veth pairs."
}

status() {
    echo "Current veth pairs (prefix: ${VETH_PREFIX}):"
    ip link show | grep -E "${VETH_PREFIX}[0-9]+" || echo "  (none found)"
}

case "${1:-}" in
    setup)
        setup "${2:-}"
        ;;
    teardown)
        teardown "${2:-}"
        ;;
    status)
        status
        ;;
    *)
        echo "Usage: $0 {setup [COUNT]|teardown [COUNT]|status}"
        echo ""
        echo "  setup [COUNT]    Create COUNT veth pairs (default: nproc = $(nproc))"
        echo "  teardown [COUNT] Remove veth pairs (scan up to COUNT, default: 64)"
        echo "  status           Show existing veth pairs"
        exit 1
        ;;
esac
