#!/bin/bash
set -euo pipefail

# Unified test runner for E2E tests of EVerest.
#
# Usage: run-tests.sh [OPTIONS] SUITE
#
# Suites:
#   all             All tests
#
#   integration     Core, framework, and async API tests
#   core            Core tests only
#   framework       Framework tests only
#   async           Async API tests only
#
#   ocpp            All OCPP tests (1.6, 2.0.1, 2.1)
#   ocpp16          OCPP 1.6 tests only
#   ocpp201         OCPP 2.0.1 tests only
#   ocpp21          OCPP 2.1 tests only
#
# Options:
#   -j N                 Parallel workers (default: nproc)
#   --serial             Run tests serially
#   --everest-prefix P   EVerest install prefix (default: <repo>/build/dist)
#   --junitxml PATH      JUnit XML output (default: result.xml)
#   --html PATH          HTML report output (default: report.html)
#   --no-isolation       Disable network isolation
#   -h, --help           Show this help
#
# Environment variables:
#   PYTHON_INTERPRETER   Python to use (default: python3)
#   PARALLEL_TESTS       Worker count (overridden by -j)
#   NETWORK_ISOLATION    true/false (overridden by --no-isolation)
#   EVEREST_PREFIX       Install prefix (overridden by --everest-prefix)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE:-$0}")" && pwd)"
EVEREST_CORE_DIR="$(dirname "$SCRIPT_DIR")"
PYTHON="${PYTHON_INTERPRETER:-python3}"

# Defaults
WORKERS="${PARALLEL_TESTS:-$(nproc)}"
SERIAL=false
PREFIX="${EVEREST_PREFIX:-${EVEREST_CORE_DIR}/build/dist}"
JUNITXML="result.xml"
HTML="report.html"
ISOLATION="${NETWORK_ISOLATION:-true}"
SUITE=""

usage() {
    sed -n '3,/^$/s/^# \?//p' "$0"
    exit 0
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -j)                WORKERS="$2"; shift 2;;
        --serial)          SERIAL=true; shift;;
        --everest-prefix)  PREFIX="$2"; shift 2;;
        --junitxml)        JUNITXML="$2"; shift 2;;
        --html)            HTML="$2"; shift 2;;
        --no-isolation)    ISOLATION=false; shift;;
        -h|--help)         usage;;
        -*)                echo "Unknown option: $1" >&2; exit 1;;
        *)                 SUITE="$1"; shift;;
    esac
done

if [[ -z "$SUITE" ]]; then
    echo "Error: no suite specified." >&2
    echo "Run '$(basename "$0") --help' for usage." >&2
    exit 1
fi

echo "Suite:   $SUITE"
echo "Python:  $PYTHON"
echo "Prefix:  $PREFIX"

# Network isolation
ISOLATION_FLAG=""
if [[ "$ISOLATION" == "true" && "$SERIAL" == "false" ]]; then
    SETUP_SCRIPT="$SCRIPT_DIR/setup-network-isolation.sh"

    if ip link show ev_test0 >/dev/null 2>&1; then
        # Veth pairs already exist (e.g. leftover from a previous run or created by CI).
        echo "Network isolation: detected pre-existing veth pairs."
        ISOLATION_FLAG="--network-isolation"
    elif [[ -x "$SETUP_SCRIPT" ]]; then
        echo "Setting up network isolation (veth pairs)..."
        # Try without sudo first (for CI).
        # CTRL+C at the sudo prompt cancels it and falls back to sequential execution.
        if "$SETUP_SCRIPT" setup "$WORKERS" 2>/dev/null; then
            ISOLATION_FLAG="--network-isolation"
        elif sudo "$SETUP_SCRIPT" setup "$WORKERS"; then
            ISOLATION_FLAG="--network-isolation"
        else
            echo "Warning: Could not set up network isolation (no sudo / CAP_NET_ADMIN)."
            echo "         ISO 15118 tests will run sequentially via xdist_group markers."
        fi
    fi

    if [[ -n "$ISOLATION_FLAG" ]]; then
        # Try without sudo (CI/root), then sudo -n (local NOPASSWD), then warn.
        teardown_network_isolation() {
            echo "Tearing down network isolation..."
            "$SETUP_SCRIPT" teardown "$WORKERS" 2>/dev/null \
                || sudo -n "$SETUP_SCRIPT" teardown "$WORKERS" 2>/dev/null \
                || echo "Warning: Could not tear down network isolation. Run manually: sudo $SETUP_SCRIPT teardown"
        }
        trap teardown_network_isolation EXIT
    fi
fi

# Common pytest arguments
PYTEST_ARGS=(
    -rA
    --self-contained-html
    --max-worker-restart=0
    --timeout=300
    --everest-prefix "$PREFIX"
    --config-file "$SCRIPT_DIR/pytest.ini"
)

if [[ "$SERIAL" == "false" ]]; then
    echo "Workers: $WORKERS"
    PYTEST_ARGS+=(-n "$WORKERS" --dist=loadgroup)
else
    echo "Workers: serial"
fi

[[ -n "$ISOLATION_FLAG" ]] && PYTEST_ARGS+=("$ISOLATION_FLAG")

# OCPP setup (certs + configs)
setup_ocpp() {
    local aux_dir="$SCRIPT_DIR/ocpp_tests/test_sets/everest-aux"
    if [[ -d "$aux_dir" ]]; then
        echo "Installing OCPP test certs and configs..."
        (cd "$aux_dir" && ./install_certs.sh "$PREFIX" && ./install_configs.sh "$PREFIX")
    fi
}

# Run
case "$SUITE" in

    all)
        echo "Running all test suites"
        cd "$SCRIPT_DIR"

        setup_ocpp

        "$PYTHON" -m pytest "${PYTEST_ARGS[@]}" \
            --junitxml="$JUNITXML" --html="$HTML" \
            core_tests/*.py \
            framework_tests/*.py \
            async_api_tests/*.py \
            ocpp_tests/test_sets/ocpp16/*.py \
            ocpp_tests/test_sets/ocpp201/*.py \
            ocpp_tests/test_sets/ocpp21/*.py
        ;;

    integration)
        cd "$SCRIPT_DIR"
        "$PYTHON" -m pytest "${PYTEST_ARGS[@]}" \
            --junitxml="$JUNITXML" --html="$HTML" \
            core_tests/*.py \
            framework_tests/*.py \
            async_api_tests/*.py
        ;;

    core)
        cd "$SCRIPT_DIR"
        "$PYTHON" -m pytest "${PYTEST_ARGS[@]}" \
            --junitxml="$JUNITXML" --html="$HTML" \
            core_tests/*.py
        ;;

    framework)
        cd "$SCRIPT_DIR"
        "$PYTHON" -m pytest "${PYTEST_ARGS[@]}" \
            --junitxml="$JUNITXML" --html="$HTML" \
            framework_tests/*.py
        ;;

    async)
        cd "$SCRIPT_DIR"
        "$PYTHON" -m pytest "${PYTEST_ARGS[@]}" \
            --junitxml="$JUNITXML" --html="$HTML" \
            async_api_tests/*.py
        ;;

    ocpp)
        setup_ocpp
        cd "$SCRIPT_DIR"
        "$PYTHON" -m pytest "${PYTEST_ARGS[@]}" \
            --junitxml="$JUNITXML" --html="$HTML" \
            ocpp_tests/test_sets/ocpp16/*.py \
            ocpp_tests/test_sets/ocpp201/*.py \
            ocpp_tests/test_sets/ocpp21/*.py
        ;;

    ocpp16)
        setup_ocpp
        cd "$SCRIPT_DIR"
        "$PYTHON" -m pytest "${PYTEST_ARGS[@]}" \
            --junitxml="$JUNITXML" --html="$HTML" \
            ocpp_tests/test_sets/ocpp16/*.py
        ;;

    ocpp201)
        setup_ocpp
        cd "$SCRIPT_DIR"
        "$PYTHON" -m pytest "${PYTEST_ARGS[@]}" \
            --junitxml="$JUNITXML" --html="$HTML" \
            ocpp_tests/test_sets/ocpp201/*.py
        ;;

    ocpp21)
        setup_ocpp
        cd "$SCRIPT_DIR"
        "$PYTHON" -m pytest "${PYTEST_ARGS[@]}" \
            --junitxml="$JUNITXML" --html="$HTML" \
            ocpp_tests/test_sets/ocpp21/*.py
        ;;

    *)
        echo "Unknown suite: $SUITE" >&2
        echo "Valid suites: all, integration, core, framework, async, ocpp, ocpp16, ocpp201, ocpp21" >&2
        exit 1
        ;;

esac
