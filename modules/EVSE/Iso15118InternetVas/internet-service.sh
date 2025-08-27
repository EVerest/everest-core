#!/usr/bin/env bash
# NAT + forwarding helper with optional namespace test
# Usage:
#   sudo ./nat-helper.sh enable <LAN_IFACE> <WAN_IFACE>
#   sudo ./nat-helper.sh disable <LAN_IFACE> <WAN_IFACE>
#   sudo ./nat-helper.sh test <WAN_IFACE> [TEST_NET_CIDR=192.168.100.0/24]
#   sudo ./nat-helper.sh cleanup-test

set -euo pipefail

# ---- config for test mode ----
NS_NAME="nat-testns"
VETH_HOST="veth-nat-host"
VETH_NS="veth-nat-ns"
TEST_NET_CIDR_DEFAULT="192.168.100.0/24"
TEST_HOST_IP_DEFAULT="192.168.100.1"
TEST_NS_IP_DEFAULT="192.168.100.2"

need_root() {
  if [[ $EUID -ne 0 ]]; then
    echo "Please run as root (use sudo)." >&2
    exit 1
  fi
}

cmd_exists() { command -v "$1" >/dev/null 2>&1; }

enable_nat() {
  local LAN_IFACE=$1
  local WAN_IFACE=$2

  echo "[+] Enabling IPv4 forwarding"
  sysctl -w net.ipv4.ip_forward=1 >/dev/null

  echo "[+] Adding MASQUERADE on $WAN_IFACE"
  iptables -t nat -C POSTROUTING -o "$WAN_IFACE" -j MASQUERADE 2>/dev/null || \
    iptables -t nat -A POSTROUTING -o "$WAN_IFACE" -j MASQUERADE

  echo "[+] Allowing forward $LAN_IFACE -> $WAN_IFACE"
  iptables -C FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j ACCEPT 2>/dev/null || \
    iptables -A FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j ACCEPT

  echo "[+] Allowing return traffic $WAN_IFACE -> $LAN_IFACE"
  iptables -C FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT 2>/dev/null || \
    iptables -A FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT

  echo "[✓] NAT enabled: $LAN_IFACE -> $WAN_IFACE"
}

disable_nat() {
  local LAN_IFACE=$1
  local WAN_IFACE=$2

  echo "[+] Disabling NAT/forwarding rules for $LAN_IFACE <-> $WAN_IFACE"
  iptables -t nat -D POSTROUTING -o "$WAN_IFACE" -j MASQUERADE 2>/dev/null || true
  iptables -D FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j ACCEPT 2>/dev/null || true
  iptables -D FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT 2>/dev/null || true

  # Do NOT blanket-disable ip_forward globally; other services might need it.
  echo "[✓] Rules removed (left ip_forward as-is)"
}

test_setup() {
  local WAN_IFACE=$1
  local TEST_NET_CIDR=${2:-$TEST_NET_CIDR_DEFAULT}

  local NET="${TEST_NET_CIDR%/*}"
  local PREFIX="${TEST_NET_CIDR#*/}"
  local HOST_IP="${TEST_HOST_IP_DEFAULT}"
  local NS_IP="${TEST_NS_IP_DEFAULT}"

  echo "[+] Creating test namespace '$NS_NAME' and veth pair"
  ip netns add "$NS_NAME" 2>/dev/null || true
  # Remove any stale veths
  ip link del "$VETH_HOST" 2>/dev/null || true
  ip link add "$VETH_HOST" type veth peer name "$VETH_NS"

  ip link set "$VETH_NS" netns "$NS_NAME"

  echo "[+] Assigning addresses"
  ip addr add "$HOST_IP/$PREFIX" dev "$VETH_HOST" 2>/dev/null || true
  ip link set "$VETH_HOST" up

  ip netns exec "$NS_NAME" ip addr add "$NS_IP/$PREFIX" dev "$VETH_NS" 2>/dev/null || true
  ip netns exec "$NS_NAME" ip link set "$VETH_NS" up

  echo "[+] Setting default route inside namespace"
  ip netns exec "$NS_NAME" ip route replace default via "$HOST_IP"

  echo "[+] Enabling NAT from $VETH_HOST -> $WAN_IFACE"
  enable_nat "$VETH_HOST" "$WAN_IFACE"

  echo
  echo "[*] Smoke tests from namespace (ping + HTTP)"
  set +e
  ip netns exec "$NS_NAME" ping -c 3 8.8.8.8
  ip netns exec "$NS_NAME" curl -sS http://example.com >/dev/null && echo "HTTP OK" || echo "HTTP FAILED"
  set -e

  echo
  echo "[✓] Test setup complete. To shell into the namespace:"
  echo "    sudo ip netns exec $NS_NAME bash"
}

test_cleanup() {
  echo "[+] Cleaning test namespace and veth"
  # Try to disable NAT for the veth if it exists (no WAN_IFACE provided here)
  # We can't know the WAN iface used; rules are harmless if left, but we try to remove by interface match:
  # (List rules; user can run disable if needed.)

  ip link del "$VETH_HOST" 2>/dev/null || true
  ip netns del "$NS_NAME" 2>/dev/null || true
  echo "[✓] Test artifacts removed"
}

usage() {
  cat <<EOF
Usage:
  sudo $0 enable <LAN_IFACE> <WAN_IFACE>
  sudo $0 disable <LAN_IFACE> <WAN_IFACE>
  sudo $0 test <WAN_IFACE> [TEST_NET_CIDR=192.168.100.0/24]
  sudo $0 cleanup-test

Notes:
  - 'enable'/'disable' manage ONLY the specific rules for the given interfaces.
  - 'test' creates a network namespace ($NS_NAME) with a veth pair:
      host: $VETH_HOST @ TEST_NET_CIDR, ns: $VETH_NS
    It then enables NAT from $VETH_HOST to the given WAN_IFACE and runs ping/curl.
  - 'cleanup-test' removes the namespace and veth.
EOF
}

main() {
  need_root

  if ! cmd_exists iptables || ! cmd_exists ip; then
    echo "This script needs 'ip' and 'iptables'. Install iproute2 and iptables." >&2
    exit 1
  fi

  local cmd="${1:-}"
  case "$cmd" in
    enable)
      [[ $# -eq 3 ]] || { usage; exit 1; }
      enable_nat "$2" "$3"
      ;;
    disable)
      [[ $# -eq 3 ]] || { usage; exit 1; }
      disable_nat "$2" "$3"
      ;;
    test)
      [[ $# -ge 2 && $# -le 3 ]] || { usage; exit 1; }
      test_setup "$2" "${3:-$TEST_NET_CIDR_DEFAULT}"
      ;;
    cleanup-test)
      test_cleanup
      ;;
    *)
      usage
      exit 1
      ;;
  esac
}

main "$@"
