// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef DIAGNOSTICS_HPP
#define DIAGNOSTICS_HPP

#include "app_layer.hpp"
#include <bitset>
#include <date/date.h>
#include <date/tz.h>
#include <everest/logging.hpp>
#include <nlohmann/json.hpp>

namespace module {

using json = nlohmann::json;

struct OcmfStats {
    std::uint32_t number_transactions{};
    std::uint32_t timestamp_first_transaction{};
    std::uint32_t timestamp_last_transaction{};
    std::uint32_t max_number_of_transactions{};
};

struct OcmfInfo {
    std::string gateway_id{};
    std::string manufacturer{};
    std::string model{};
};

struct DeviceData {
    std::uint32_t utc_time_s{};
    std::uint8_t gmt_offset_quarterhours{};
    std::uint64_t total_start_import_energy_Wh{}; // meter value needs to be divided by 10 for Wh
    std::uint64_t total_stop_import_energy_Wh{};  // meter value needs to be divided by 10 for Wh
    std::uint32_t total_transaction_duration_s{}; // must be less than 27 days in total
    OcmfStats ocmf_stats;
    std::string last_ocmf_transaction;
    std::string requested_ocmf;
    std::uint64_t total_dev_import_energy_Wh{}; // meter value needs to be divided by 10 for Wh
    std::uint64_t ab_status{};
};

void to_json(json& j, const DeviceData& k);
void from_json(const json& j, DeviceData& k);
std::ostream& operator<<(std::ostream& os, const DeviceData& k);

struct LogStats {
    std::uint32_t number_log_entries{};
    std::uint32_t timestamp_first_log{};
    std::uint32_t timestamp_last_log{};
    std::uint32_t max_number_of_logs{};
};

struct ApplicationInfo {
    std::string mode;
    std::string fw_ver;
    std::uint16_t fw_crc{};
    std::uint16_t fw_hash{};
};

struct MeteringInfo {
    std::string fw_ver;
    std::uint16_t fw_crc{};
    std::uint8_t mode{};
};

struct DeviceInfo {
    std::string type;
    std::string hw_ver;
    std::string server_id;
    std::uint32_t serial_number{};
    std::uint8_t bus_address{};
    std::string bootl_ver;
    ApplicationInfo application;
    MeteringInfo metering;
};

struct DeviceDiagnostics {
    std::string charge_point_id;
    std::uint8_t charge_point_id_type{0};
    DeviceInfo dev_info;
    LogStats log_stats;
    std::string pubkey_asn1;
    std::string pubkey_str16;
    std::string pubkey;
    std::uint8_t pubkey_str16_format{}; // 0x04 for uncompressed string
    std::uint8_t pubkey_format{};       // 0x04 for uncompressed string
    std::vector<std::uint8_t> ocmf_config_table;
};

void to_json(json& j, const DeviceDiagnostics& k);
void from_json(const json& j, DeviceDiagnostics& k);
std::ostream& operator<<(std::ostream& os, const DeviceDiagnostics& k);

// TODO(LAD): add error data

struct ErrorData {
    std::uint32_t id{0};
    std::uint16_t priority{0};
    std::uint32_t counter{0};
};

struct FiveErrors {
    ErrorData error[5];
};

struct ErrorSet {
    FiveErrors category[4];
};

struct Logging {
    app_layer::LogEntry last_log;
    ErrorSet source[2];
};

void to_json(json& j, const Logging& k);
void from_json(const json& j, Logging& k);
std::ostream& operator<<(std::ostream& os, const Logging& k);

namespace conversions {

template <typename T> static std::string to_bin_string(const T& num) {
    std::stringstream ss{};
    for (std::uint8_t n = 0; n < sizeof(T); n++) {
        ss << std::bitset<8>(num >> ((sizeof(T) - n - 1) * 8));
        if (n % 2) {
            if (n != sizeof(T) - 1) {
                ss << " - ";
            }
        } else {
            ss << " ";
        }
    }
    return ss.str();
}

std::string hexdump(const std::vector<std::uint8_t>& msg);
std::string hexdump(const std::vector<std::uint8_t>& msg, std::uint8_t start, std::uint8_t number_of_chars);
std::string hexdump(std::uint8_t msg);
std::string hexdump(std::uint16_t msg);
std::string hexdump(std::uint64_t msg);
std::string get_string(const std::vector<std::uint8_t>& vec);
std::string u32_epoch_to_rfc3339(std::uint32_t epoch_time);

} // namespace conversions
} // namespace module

#endif // DIAGNOSTICS_HPP
