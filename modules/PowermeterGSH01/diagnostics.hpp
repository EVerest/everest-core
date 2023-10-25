// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DIAGNOSTICS_HPP
#define DIAGNOSTICS_HPP

#include <date/date.h>
#include <date/tz.h>
#include <everest/logging.hpp>
#include <nlohmann/json.hpp>
#include "gsh01_app_layer.hpp"

namespace module {

using json = nlohmann::json;

struct OcmfStats {
    uint32_t number_transactions{};
    uint32_t timestamp_first_transaction{};
    uint32_t timestamp_last_transaction{};
    uint32_t max_number_of_transactions{};    // ???
};

struct OcmfInfo {
    std::string gateway_id{};
    std::string manufacturer{};
    std::string model{};
};

struct DeviceData {
    uint32_t utc_time_s{};
    uint8_t gmt_offset_quarterhours{};
    uint64_t total_start_import_energy_Wh{};  // meter value needs to be divided by 10 for Wh
    uint64_t total_stop_import_energy_Wh{};   // meter value needs to be divided by 10 for Wh
    uint32_t total_transaction_duration_s{};  // must be less than 27 days in total
    OcmfStats ocmf_stats;
    std::string last_ocmf_transaction{};
    std::string requested_ocmf{};
    uint64_t total_dev_import_energy_Wh{};  // meter value needs to be divided by 10 for Wh
    uint64_t ab_status{};
};

void to_json(json& j, const DeviceData& k);
void from_json(const json& j, DeviceData& k);
std::ostream& operator<<(std::ostream& os, const DeviceData& k);

struct LogStats {
    uint32_t number_log_entries{};
    uint32_t timestamp_first_log{};
    uint32_t timestamp_last_log{};
    uint32_t max_number_of_logs{};    // ???
};

struct ApplicationInfo {
    std::string mode{};
    std::string fw_ver{};
    uint16_t fw_crc{};
    uint16_t fw_hash{};
};

struct MeteringInfo {
    std::string fw_ver{};
    uint16_t fw_crc{};
};

struct DeviceInfo {
    std::string type{};
    std::string hw_ver{};
    std::string server_id{};
    uint32_t serial_number{};
    uint8_t bus_address{};
    std::string bootl_ver{};
    ApplicationInfo application;
    MeteringInfo metering;
};

struct DeviceDiagnostics {
    std::string charge_point_id{};
    uint8_t charge_point_id_type{0};
    DeviceInfo dev_info;
    LogStats log_stats;
    std::string pubkey_asn1{};
    std::string pubkey_str16{};
    std::string pubkey{};
    uint8_t pubkey_str16_format{};  // 0x04 for uncompressed string
    uint8_t pubkey_format{};        // 0x04 for uncompressed string
    std::vector<uint8_t> ocmf_config_table{};
};

void to_json(json& j, const DeviceDiagnostics& k);
void from_json(const json& j, DeviceDiagnostics& k);
std::ostream& operator<<(std::ostream& os, const DeviceDiagnostics& k);

// TODO(LAD): add error data

struct ErrorData {
    uint32_t id{0};
    uint16_t priority{0};
    uint32_t counter{0};
};

struct FiveErrors {
    ErrorData error[5];
};

struct ErrorSet {
    FiveErrors category[4];
};

struct Logging {
    gsh01_app_layer::LogEntry last_log;
    ErrorSet source[2];
};

void to_json(json& j, const Logging& k);
void from_json(const json& j, Logging& k);
std::ostream& operator<<(std::ostream& os, const Logging& k);


namespace conversions {

// std::string state_to_string(State e);
// State string_to_state(const std::string& s);

template<typename T>
static std::string to_bin_string(const T& num){
    std::stringstream ss{};
    for (uint8_t n = 0; n < sizeof(T); n++){
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

static std::string hexdump(std::vector<uint8_t> msg) {
    std::stringstream ss;
    for (auto index : msg) {
        ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)index << " ";
    }
    return ss.str();
}

static std::string hexdump(std::vector<uint8_t> msg, uint8_t start, uint8_t number_of_chars) {
    if ((start + number_of_chars) > msg.size()) return std::string{};
    std::stringstream ss;
    for (uint8_t n = start; n < (start + number_of_chars); n++) {
        ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)msg.at(n);
        if (n < (start + number_of_chars - 1)) ss << " ";
    }
    return ss.str();
}

static std::string hexdump(uint8_t msg) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)msg;
    return ss.str();
}

static std::string hexdump(uint16_t msg) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << (uint16_t)msg;
    return ss.str();
}

static std::string get_string(std::vector<uint8_t>& vec) {
    std::string str = "";
    for (uint16_t n = 0; n < vec.size(); n++){
        if ((vec[n] < ' ') || (vec[n] > '~')) {
            str += " ";    
        } else {
            str += vec[n];
        }
    }
    return std::move(str);
}

static std::string u32_epoch_to_rfc3339(uint32_t epoch_time) {
    time_t tt = static_cast<time_t>(epoch_time);
    std::tm tm = *std::gmtime(&tt);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S.000Z");
    return std::move(ss.str());
}


} // namespace conversions
} // namespace module

#endif // DIAGNOSTICS_HPP