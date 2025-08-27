#!/bin/bash

# This script manages the full network setup to provide internet access
# to an EV. It handles IP forwarding, NAT, a DHCP server, and a
# router advertisement daemon.

set -euo pipefail

# --- Configuration ---
DHCP_STATIC_IP="192.168.28.1"
SUBNET_MASK="24"
RADVD_IPV6_PREFIX="fd00::/64"

# --- File Paths ---
PID_DIR="/var/run"
DHCP_PID_FILE="${PID_DIR}/vas_udhcpd.pid"
RADVD_PID_FILE="${PID_DIR}/vas_radvd.pid"

CONF_DIR="/tmp"
DHCP_CONF_FILE="${CONF_DIR}/vas_udhcpd.conf"
RADVD_CONF_FILE="${CONF_DIR}/vas_radvd.conf"

# --- Helper Functions ---
need_root() {
  if [[ $EUID -ne 0 ]]; then
    echo "Please run as root (use sudo)." >&2
    exit 1
  fi
}

cmd_exists() {
  command -v "$1" >/dev/null 2>&1
}

check_deps() {
    for cmd in ip iptables ip6tables udhcpd radvd; do
        if ! cmd_exists "$cmd"; then
            echo "Error: Command '$cmd' not found. Please install it." >&2
            exit 1
        fi
    done
}

# --- Core Logic Functions ---

start_services() {
  local LAN_IFACE=$1
  local WAN_IFACE=$2
  local PORTS=${3:-}

  echo "[+] Starting all internet VAS services for $LAN_IFACE -> $WAN_IFACE"

  # 1. Enable IP Forwarding
  echo "  [1/4] Enabling IPv4 & IPv6 forwarding"
  sysctl -w net.ipv4.ip_forward=1 >/dev/null
  sysctl -w net.ipv6.conf.all.forwarding=1 >/dev/null

  # 2. Setup NAT and Forwarding Rules
  echo "  [2/4] Setting up NAT and FORWARD rules on $WAN_IFACE"
  # MASQUERADE rule for outbound traffic on WAN interface
  iptables -t nat -C POSTROUTING -o "$WAN_IFACE" -j MASQUERADE 2>/dev/null || \
    iptables -t nat -A POSTROUTING -o "$WAN_IFACE" -j MASQUERADE
ip6tables -t nat -C POSTROUTING -o "$WAN_IFACE" -j MASQUERADE 2>/dev/null || \
    ip6tables -t nat -A POSTROUTING -o "$WAN_IFACE" -j MASQUERADE

  # Allow return traffic for established connections
  iptables -C FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT 2>/dev/null || \
    iptables -A FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT
ip6tables -C FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT 2>/dev/null || \
    ip6tables -A FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT

  # Add specific forwarding rules for outbound traffic from LAN interface
  if [ -z "$PORTS" ]; then
    echo "    - Allowing all outgoing traffic (no port restriction)"
    iptables -C FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j ACCEPT 2>/dev/null || \
      iptables -A FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j ACCEPT
    ip6tables -C FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j ACCEPT 2>/dev/null || \
      ip6tables -A FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j ACCEPT
  else
    echo "    - Allowing outgoing traffic on TCP/UDP ports: $PORTS"
    # TCP
    iptables -C FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p tcp -m multiport --dports "$PORTS" -j ACCEPT 2>/dev/null || \
      iptables -A FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p tcp -m multiport --dports "$PORTS" -j ACCEPT
    # UDP
    iptables -C FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p udp -m multiport --dports "$PORTS" -j ACCEPT 2>/dev/null || \
      iptables -A FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p udp -m multiport --dports "$PORTS" -j ACCEPT
    
    # TCP (IPv6)
    ip6tables -C FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p tcp -m multiport --dports "$PORTS" -j ACCEPT 2>/dev/null || \
      ip6tables -A FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p tcp -m multiport --dports "$PORTS" -j ACCEPT
    # UDP (IPv6)
    ip6tables -C FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p udp -m multiport --dports "$PORTS" -j ACCEPT 2>/dev/null || \
      ip6tables -A FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p udp -m multiport --dports "$PORTS" -j ACCEPT
  fi

  # 3. Start DHCP server
  echo "  [3/4] Starting DHCPv4 server on $LAN_IFACE"
  local IP_WITH_SUBNET="${DHCP_STATIC_IP}/${SUBNET_MASK}"
  ip addr show dev "${LAN_IFACE}" | grep -q "${IP_WITH_SUBNET}" || ip addr add "${IP_WITH_SUBNET}" dev "${LAN_IFACE}"

  local NETWORK_BASE=$(echo "$DHCP_STATIC_IP" | cut -d. -f1-3)
  cat > "${DHCP_CONF_FILE}" << EOF
interface   ${LAN_IFACE}
start       ${NETWORK_BASE}.2
end         ${NETWORK_BASE}.200
option      subnet  255.255.255.0
option      router  ${DHCP_STATIC_IP}
option      lease   864000
pidfile     ${DHCP_PID_FILE}
EOF
  udhcpd -S "${DHCP_CONF_FILE}"

  # 4. Start RADVD server
  echo "  [4/4] Starting DHCPv6/RADVD server on $LAN_IFACE"
  cat > "${RADVD_CONF_FILE}" << EOF
interface ${LAN_IFACE}
{
  AdvSendAdvert on;
  MinRtrAdvInterval 30;
  MaxRtrAdvInterval 100;
  prefix ${RADVD_IPV6_PREFIX}
  {
    AdvOnLink on;
    AdvAutonomous on;
    AdvRouterAddr off;
  };
};
EOF
  radvd -C "${RADVD_CONF_FILE}" -p "${RADVD_PID_FILE}"

  echo "[✓] All services started."
}

stop_services() {
  local LAN_IFACE=$1
  local WAN_IFACE=$2
  local PORTS=${3:-}

  echo "[+] Stopping all internet VAS services for $LAN_IFACE -> $WAN_IFACE"

  # 1. Stop daemons
  echo "  [1/3] Stopping DHCPv4 server"
  if [ -f "$DHCP_PID_FILE" ]; then
    kill "$(cat "$DHCP_PID_FILE")" || echo "DHCP server already stopped."
    rm -f "$DHCP_PID_FILE"
  else
    echo "DHCP PID file not found. Maybe it was not running."
  fi

  echo "  [2/3] Stopping DHCPv6/RADVD server"
  if [ -f "$RADVD_PID_FILE" ]; then
    kill "$(cat "$RADVD_PID_FILE")" || echo "RADVD server already stopped."
    rm -f "$RADVD_PID_FILE"
  else
    echo "RADVD PID file not found. Maybe it was not running."
  fi

  # 2. Remove NAT and FORWARD rules
  echo "  [3/3] Removing NAT and FORWARD rules"
  iptables -t nat -D POSTROUTING -o "$WAN_IFACE" -j MASQUERADE 2>/dev/null || true
  ip6tables -t nat -D POSTROUTING -o "$WAN_IFACE" -j MASQUERADE 2>/dev/null || true

  iptables -D FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT 2>/dev/null || true
  ip6tables -D FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT 2>/dev/null || true

  if [ -z "$PORTS" ]; then
    iptables -D FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j ACCEPT 2>/dev/null || true
    ip6tables -D FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j ACCEPT 2>/dev/null || true
  else
    # TCP
    iptables -D FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p tcp -m multiport --dports "$PORTS" -j ACCEPT 2>/dev/null || true
    # UDP
    iptables -D FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p udp -m multiport --dports "$PORTS" -j ACCEPT 2>/dev/null || true

    # TCP (IPv6)
    ip6tables -D FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p tcp -m multiport --dports "$PORTS" -j ACCEPT 2>/dev/null || true
    # UDP (IPv6)
    ip6tables -D FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -p udp -m multiport --dports "$PORTS" -j ACCEPT 2>/dev/null || true
  fi

  echo "[✓] All services stopped and rules removed."
}

usage() {
  cat <<EOF
Usage:
  sudo $0 up <LAN_IFACE> <WAN_IFACE> [PORTS]
  sudo $0 down <LAN_IFACE> <WAN_IFACE> [PORTS]

Commands:
  up    - Configures interfaces, enables NAT/forwarding, and starts DHCP/RADVD.
  down  - Stops daemons and removes all created network rules.

Arguments:
  PORTS - Optional comma-separated list of TCP/UDP ports to allow (e.g., "80,443").
          If omitted, all traffic is allowed.
EOF
}

main() {
  need_root
  check_deps

  local cmd="${1:-}"
  if [[ $# -lt 3 || $# -gt 4 ]]; then
    usage
    exit 1
  fi

  case "$cmd" in
    up)
      start_services "$2" "$3" "${4:-}"
      ;; 
    down)
      stop_services "$2" "$3" "${4:-}"
      ;; 
    *)
      usage
      exit 1
      ;; 
  esac
}

main "$@"
