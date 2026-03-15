#!/usr/bin/env bash

set -euo pipefail

readonly ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
readonly TYPES_CSV="lib/everest/everest_api_types/tests/expected_types_file_hashes.csv"
readonly INTERFACES_CSV="lib/everest/everest_api_types/tests/expected_interfaces_file_hashes.csv"

declare -i failure_count=0

trim() {
    local value="$1"
    value="${value#"${value%%[![:space:]]*}"}"
    value="${value%"${value##*[![:space:]]}"}"
    printf '%s' "${value}"
}

check_hashes() {
    local label="$1"
    local csv_path="$2"
    local source_dir="$3"

    declare -A expected_hashes=()
    local line
    local line_no=0

    echo "Checking ${label} hashes using ${csv_path}"

    while IFS= read -r line || [[ -n "${line}" ]]; do
        line_no=$((line_no + 1))
        line="$(trim "${line}")"

        if [[ -z "${line}" || "${line}" == \#* ]]; then
            continue
        fi

        if [[ "${line}" != *,* ]]; then
            echo "ERROR: Invalid CSV line ${line_no} in ${csv_path}: ${line}"
            failure_count=$((failure_count + 1))
            continue
        fi

        local file_path="${line%%,*}"
        local expected_hash="${line#*,}"
        file_path="$(trim "${file_path}")"
        expected_hash="$(trim "${expected_hash}")"

        if [[ -n "${expected_hashes[${file_path}]+x}" ]]; then
            echo "ERROR: Duplicate CSV entry for ${file_path} in ${csv_path}"
            failure_count=$((failure_count + 1))
            continue
        fi

        expected_hashes["${file_path}"]="${expected_hash}"
        local absolute_file="${ROOT_DIR}/${file_path}"
        if [[ ! -f "${absolute_file}" ]]; then
            echo "ERROR: File listed in ${csv_path} is missing: ${file_path}"
            echo "  expected: ${expected_hash}"
            echo "  actual:   <missing file>"
            failure_count=$((failure_count + 1))
            continue
        fi

        local actual_hash
        actual_hash="$(sha256sum "${absolute_file}" | awk '{print $1}')"
        if [[ "${actual_hash}" != "${expected_hash}" ]]; then
            echo "ERROR: SHA256 mismatch for ${file_path}"
            echo "  expected: ${expected_hash}"
            echo "  actual:   ${actual_hash}"
            failure_count=$((failure_count + 1))
        fi
    done < "${ROOT_DIR}/${csv_path}"

# Check for untracked files - practical, but noisy
#    while IFS= read -r absolute_file; do
#        local relative_file
#        relative_file="${absolute_file#${ROOT_DIR}/}"
#        if [[ -z "${expected_hashes[${relative_file}]+x}" ]]; then
#            local actual_hash
#            actual_hash="$(sha256sum "${absolute_file}" | awk '{print $1}')"
#            echo "WARNING: ${label} file is not tracked in ${csv_path}: ${relative_file}"
#            echo "  expected: <missing CSV entry>"
#            echo "  actual:   ${actual_hash}"
#        fi
#    done < <(find "${ROOT_DIR}/${source_dir}" -maxdepth 1 -type f -name '*.yaml' | sort)
}

check_hashes "type" "${TYPES_CSV}" "types"
check_hashes "interface" "${INTERFACES_CSV}" "interfaces"

if (( failure_count > 0 )); then
    echo
    echo "Source file hash check failed with ${failure_count} issue(s)."
    exit 1
fi

echo
echo "Source file hash check passed."
