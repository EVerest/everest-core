#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
EVEREST_CORE_DIR="$(cd -- "${SCRIPT_DIR}/../../../../.." && pwd)"
DIST_DIR="${EVEREST_CORE_DIR}/build/dist"
DIST_ETC_DIR="${DIST_DIR}/etc/everest"
MANAGER_BIN="${DIST_DIR}/bin/manager"
BUPOWERMETER_BIN="${DIST_DIR}/libexec/everest/modules/BUPowermeter/BUPowermeter"

if [[ ! -x "${MANAGER_BIN}" ]]; then
  echo "ERROR: manager binary not found/executable at: ${MANAGER_BIN}" >&2
  echo "Did you build everest-core and generate the dist/ folder?" >&2
  exit 1
fi
if [[ ! -x "${BUPOWERMETER_BIN}" ]]; then
  echo "ERROR: BUPowermeter binary not found/executable at: ${BUPOWERMETER_BIN}" >&2
  echo "Did you build everest-core and generate the dist/ folder?" >&2
  exit 1
fi
if [[ ! -d "${DIST_ETC_DIR}" ]]; then
  echo "ERROR: dist etc dir not found at: ${DIST_ETC_DIR}" >&2
  exit 1
fi

other_pids=()

cleanup() {
  for pid in "${other_pids[@]:-}"; do
    kill "${pid}" 2>/dev/null || true
  done
}

trap cleanup EXIT INT TERM

read -r -p "Start CGEM580 bringup with 1, 6, 7, 12 or 13 devices? [1/6/7/12/13]: " device_count

case "${device_count}" in
  1)
    config_file="config-bringup-CGEM580.yaml"
    ;;
  6)
    config_file="config-bringup-CGEM580-6x.yaml"
    ;;
  7)
    config_file="config-bringup-CGEM580-7x.yaml"
    ;;
  12)
    config_file="config-bringup-CGEM580-12x.yaml"
    ;;
  13)
    config_file="config-bringup-CGEM580-13x.yaml"
    ;;
  *)
    echo "Invalid choice: '${device_count}'. Please enter 1, 6, 7, 12 or 13."
    exit 2
    ;;
esac

if [[ ! -f "${DIST_ETC_DIR}/${config_file}" ]]; then
  echo "ERROR: config file not found at: ${DIST_ETC_DIR}/${config_file}" >&2
  exit 1
fi

# Start manager first, then powermeters. When the manager window closes, all others will be closed as well.
# Important: run with CWD in ${DIST_ETC_DIR} (matches previous scripts and avoids any CWD-sensitive behavior).
xterm -bg black -fg white -geometry 400x150 -e bash -lc "cd \"${DIST_ETC_DIR}\" && \"${MANAGER_BIN}\" --prefix \"${DIST_DIR}\" --conf \"${config_file}\"" &
manager_pid=$!

for i in $(seq 1 "${device_count}"); do
  xterm -bg black -fg white -geometry 200x55 -e bash -lc "cd \"${DIST_ETC_DIR}\" && sleep 1 && \"${BUPOWERMETER_BIN}\" --module cli_${i}" &
  other_pids+=($!)
done

wait "${manager_pid}"