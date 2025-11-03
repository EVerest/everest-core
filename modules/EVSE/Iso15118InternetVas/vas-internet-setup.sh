#!/bin/sh

# --------------------------------------------------------------------------
#
#  WARNING: This script is an example and not intended for production use.
#
#  This script is provided as a starting point for setting up internet
#  access for an EV. It is not a complete solution and may require
#  modification to work in your specific environment.
#
#  It is the responsibility of the user to ensure that the script is
#  secure and does not introduce any security vulnerabilities.
#
# --------------------------------------------------------------------------

# This script manages the full network setup to provide internet access
# to an EV. It handles IP forwarding, NAT, a DHCP server, and a
# router advertisement daemon.

set -euo pipefail

# --- Configuration ---
DHCP_STATIC_IP="172.18.200.1"
SUBNET_MASK="24"
RADVD_IPV6_PREFIX="fd00::/64"

# --- File Paths ---
PID_DIR="/var/run"
DHCP_PID_FILE="${PID_DIR}/vas_udhcpd.pid"
RADVD_PID_FILE="${PID_DIR}/vas_radvd.pid"

CONF_DIR="/tmp"
DHCP_CONF_FILE="${CONF_DIR}/vas_udhcpd.conf"
RADVD_CONF_FILE="${CONF_DIR}/vas_radvd.conf"

# Load optional configuration from /etc/default/vas-internet
[ -f /etc/default/vas-internet ] && . /etc/default/vas-internet

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
    local use_ipv4=$1
    local deps="ip ip6tables radvd"
    if [ "$use_ipv4" = true ]; then
        deps="$deps iptables udhcpd"
    fi

    for cmd in $deps; do
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
  local USE_IPV4=$4

  echo "[+] Starting all internet VAS services for $LAN_IFACE -> $WAN_IFACE"

  # 1. Enable IP Forwarding
  echo "  [1/4] Enabling IP forwarding"
  if [ "$USE_IPV4" = true ]; then
    sysctl -w net.ipv4.ip_forward=1 >/dev/null
  fi
  sysctl -w net.ipv6.conf.all.forwarding=1 >/dev/null

  # 2. Setup NAT and Forwarding Rules
  echo "  [2/4] Setting up NAT and FORWARD rules on $WAN_IFACE"

  # Create custom chains
  if [ "$USE_IPV4" = true ]; then
    iptables -N VAS_FORWARD 2>/dev/null || iptables -F VAS_FORWARD
  fi
  ip6tables -N VAS_FORWARD 2>/dev/null || ip6tables -F VAS_FORWARD

  # Jump to custom chain
  if [ "$USE_IPV4" = true ]; then
    iptables -C FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j VAS_FORWARD 2>/dev/null || \
      iptables -A FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j VAS_FORWARD
  fi
  ip6tables -C FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j VAS_FORWARD 2>/dev/null || \
    ip6tables -A FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j VAS_FORWARD

  # MASQUERADE rule for outbound traffic on WAN interface
  if [ "$USE_IPV4" = true ]; then
    iptables -t nat -C POSTROUTING -o "$WAN_IFACE" -j MASQUERADE 2>/dev/null || \
      iptables -t nat -A POSTROUTING -o "$WAN_IFACE" -j MASQUERADE
  fi
ip6tables -t nat -C POSTROUTING -o "$WAN_IFACE" -j MASQUERADE 2>/dev/null || \
    ip6tables -t nat -A POSTROUTING -o "$WAN_IFACE" -j MASQUERADE

  # Allow return traffic for established connections
  if [ "$USE_IPV4" = true ]; then
    iptables -C FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT 2>/dev/null || \
      iptables -A FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT
  fi
ip6tables -C FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT 2>/dev/null || \
    ip6tables -A FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT

  # Add specific forwarding rules for outbound traffic from LAN interface
  echo "    - Allowing outgoing traffic on TCP ports: $PORTS"
  if [ "$USE_IPV4" = true ]; then
    # TCP
    iptables -A VAS_FORWARD -p tcp -m multiport --dports "$PORTS" -j ACCEPT
  fi

  # TCP (IPv6)
  ip6tables -A VAS_FORWARD -p tcp -m multiport --dports "$PORTS" -j ACCEPT

  # 3. Start DHCP server
  if [ "$USE_IPV4" = true ]; then
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
option      dns     8.8.8.8 8.8.4.4
option      lease   864000
pidfile     ${DHCP_PID_FILE}
EOF
    udhcpd -S "${DHCP_CONF_FILE}"
  else
    echo "  [3/4] Skipping DHCPv4 server (IPv4 disabled)"
  fi

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
  RDNSS 2001:4860:4860::8888 2001:4860:4860::8844
  {
    AdvRDNSSLifetime 3600;
  };
};
EOF
  radvd -C "${RADVD_CONF_FILE}" -p "${RADVD_PID_FILE}"

  echo "[✓] All services started."
}

stop_services() {
  local LAN_IFACE=$1
  local WAN_IFACE=$2
  local PORTS=$3
  local USE_IPV4=$4

  echo "[+] Stopping all internet VAS services for $LAN_IFACE -> $WAN_IFACE"

  # 1. Stop daemons
  if [ "$USE_IPV4" = true ]; then
    echo "  [1/3] Stopping DHCPv4 server"
    if [ -f "$DHCP_PID_FILE" ]; then
      kill "$(cat "$DHCP_PID_FILE")" || echo "DHCP server already stopped."
      rm -f "$DHCP_PID_FILE"
    else
      echo "DHCP PID file not found. Maybe it was not running."
    fi
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

  # Remove jump rule
  if [ "$USE_IPV4" = true ]; then
    iptables -D FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j VAS_FORWARD 2>/dev/null || true
  fi
  ip6tables -D FORWARD -i "$LAN_IFACE" -o "$WAN_IFACE" -j VAS_FORWARD 2>/dev/null || true

  # Flush and delete custom chain
  if [ "$USE_IPV4" = true ]; then
    iptables -F VAS_FORWARD 2>/dev/null || true
    iptables -X VAS_FORWARD 2>/dev/null || true
  fi
  ip6tables -F VAS_FORWARD 2>/dev/null || true
  ip6tables -X VAS_FORWARD 2>/dev/null || true

  if [ "$USE_IPV4" = true ]; then
    iptables -t nat -D POSTROUTING -o "$WAN_IFACE" -j MASQUERADE 2>/dev/null || true
    iptables -D FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT 2>/dev/null || true
  fi
  ip6tables -t nat -D POSTROUTING -o "$WAN_IFACE" -j MASQUERADE 2>/dev/null || true
  ip6tables -D FORWARD -i "$WAN_IFACE" -o "$LAN_IFACE" -m state --state RELATED,ESTABLISHED -j ACCEPT 2>/dev/null || true

  echo "[✓] All services stopped and rules removed."
}

usage() {
  cat <<EOF
Usage:
  sudo $0 up <LAN_IFACE> <WAN_IFACE> <PORTS> [--ipv4]
  sudo $0 down <LAN_IFACE> <WAN_IFACE> <PORTS> [--ipv4]

Commands:
  up    - Configures interfaces, enables NAT/forwarding, and starts DHCP/RADVD.
  down  - Stops daemons and removes all created network rules.

Arguments:
  PORTS - Comma-separated list of TCP ports to allow (e.g., "80,443").
  --ipv4 - Optional flag to enable all IPv4-related setup (disabled by default).
EOF
}

main() {
  need_root

  local use_ipv4=false
  local args=()
  while [[ $# -gt 0 ]]; do
    case "$1" in
      --ipv4)
        use_ipv4=true
        shift
        ;;
      *)
        args+=("$1")
        shift
        ;;
    esac
  done

  check_deps "$use_ipv4"

  local cmd="${args[0]:-}"
  if [[ ${#args[@]} -ne 4 ]]; then
    usage
    exit 1
  fi

  case "$cmd" in
    up)
      start_services "${args[1]}" "${args[2]}" "${args[3]}" "$use_ipv4"
      ;;
    down)
      stop_services "${args[1]}" "${args[2]}" "${args[3]}" "$use_ipv4"
      ;;
    *)
      usage
      exit 1
      ;;
  esac
}

main "$@"
