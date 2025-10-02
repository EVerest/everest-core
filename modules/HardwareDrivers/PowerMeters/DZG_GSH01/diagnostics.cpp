// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <diagnostics.hpp>

namespace module {

void to_json(json& j, const DeviceData& k) {
    j["UTC"] = module::conversions::u32_epoch_to_rfc3339(k.utc_time_s);
    j["GMT_offset_quarterhours"] = k.gmt_offset_quarterhours;
    j["total_start_import_energy_Wh"] = k.total_start_import_energy_Wh;
    j["total_stop_import_energy_Wh"] = k.total_stop_import_energy_Wh;
    j["total_transaction_duration_s"] = k.total_transaction_duration_s;
    j["OCMF_stats"] = json();
    j["OCMF_stats"]["number_transactions"] = k.ocmf_stats.number_transactions;
    j["OCMF_stats"]["timestamp_first_transaction"] = k.ocmf_stats.timestamp_first_transaction;
    j["OCMF_stats"]["timestamp_last_transaction"] = k.ocmf_stats.timestamp_last_transaction;
    j["OCMF_stats"]["max_number_of_transactions"] = k.ocmf_stats.max_number_of_transactions;
    j["last_ocmf_transaction"] = k.last_ocmf_transaction;
    j["requested_ocmf"] = k.requested_ocmf;
    j["total_dev_import_energy_Wh"] = k.total_dev_import_energy_Wh;
    j["status"] = module::conversions::to_bin_string(k.ab_status);
}

std::ostream& operator<<(std::ostream& os, const DeviceData& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const DeviceDiagnostics& k) {
    j["charge_point_id"] = k.charge_point_id;
    j["charge_point_id_type"] = k.charge_point_id_type;
    j["log_stats"] = json();
    j["log_stats"]["number_log_entries"] = k.log_stats.number_log_entries;
    j["log_stats"]["timestamp_first_log"] = k.log_stats.timestamp_first_log;
    j["log_stats"]["timestamp_last_log"] = k.log_stats.timestamp_last_log;
    j["log_stats"]["max_number_of_logs"] = k.log_stats.max_number_of_logs;
    j["dev_info"] = json();
    j["dev_info"]["type"] = k.dev_info.type;
    j["dev_info"]["hw_ver"] = k.dev_info.hw_ver;
    j["dev_info"]["server_id"] = k.dev_info.server_id;
    j["dev_info"]["serial_nr"] = k.dev_info.serial_number;
    j["dev_info"]["application"] = json();
    j["dev_info"]["application"]["mode"] = k.dev_info.application.mode;
    j["dev_info"]["application"]["FW_ver"] = k.dev_info.application.fw_ver;
    j["dev_info"]["application"]["FW_CRC"] = module::conversions::hexdump(k.dev_info.application.fw_crc);
    j["dev_info"]["application"]["FW_hash"] = module::conversions::hexdump(k.dev_info.application.fw_hash);
    j["dev_info"]["metering"] = json();
    j["dev_info"]["metering"]["FW_ver"] = k.dev_info.metering.fw_ver;
    j["dev_info"]["metering"]["FW_CRC"] = module::conversions::hexdump(k.dev_info.metering.fw_crc);
    j["dev_info"]["metering"]["mode"] = k.dev_info.metering.mode;
    j["dev_info"]["bus_address"] = module::conversions::hexdump(k.dev_info.bus_address);
    j["dev_info"]["bootl_ver"] = k.dev_info.bootl_ver;
    j["pubkey"] = json();
    j["pubkey"]["asn1"] = json();
    j["pubkey"]["str16"] = json();
    j["pubkey"]["default"] = json();
    j["pubkey"]["asn1"]["key"] = k.pubkey_asn1;
    j["pubkey"]["str16"]["key"] = k.pubkey_str16;
    j["pubkey"]["str16"]["format"] = k.pubkey_str16_format;
    j["pubkey"]["default"]["key"] = k.pubkey;
    j["pubkey"]["default"]["format"] = k.pubkey_format;
    j["ocmf_config_table"] = json::array();
    if (k.ocmf_config_table.size() > 0) {
        for (std::uint8_t n = 0; n < k.ocmf_config_table.size(); n++) {
            j["ocmf_config_table"][n] =
                module::conversions::hexdump(static_cast<std::uint8_t>(k.ocmf_config_table.at(n)));
        }
    }
}

void from_json(const json& j, DeviceDiagnostics& k) {
    EVLOG_error << "[DeviceDiagnostics][from_json()] not implemented";
}

std::ostream& operator<<(std::ostream& os, const DeviceDiagnostics& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const Logging& k) {
    j["log"] = json();
    j["log"]["last"] = json();
    j["log"]["last"]["type"] = "" + std::to_string((int)k.last_log.type) + ": " + log_type_to_string(k.last_log.type);
    j["log"]["last"]["second_index"] = k.last_log.second_index;
    j["log"]["last"]["utc_time"] = module::conversions::u32_epoch_to_rfc3339(k.last_log.utc_time);
    j["log"]["last"]["utc_offset_quarterhours"] = k.last_log.utc_offset;
    j["log"]["last"]["old_value"] = module::conversions::hexdump(k.last_log.old_value);
    j["log"]["last"]["new_value"] = module::conversions::hexdump(k.last_log.new_value);
    j["log"]["last"]["server_id"] = module::conversions::hexdump(k.last_log.server_id);
    j["log"]["last"]["signature"] = module::conversions::hexdump(k.last_log.signature);

    j["errors"] = json();
    j["errors"]["system"] = json();
    j["errors"]["system"]["last"] = json::array();
    for (std::uint8_t n = 0; n < 5; n++) {
        j["errors"]["system"]["last"][n]["id"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::SYSTEM)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST)]
                .error[n]
                .id;
        j["errors"]["system"]["last"][n]["priority"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::SYSTEM)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST)]
                .error[n]
                .priority;
        j["errors"]["system"]["last"][n]["counter"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::SYSTEM)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST)]
                .error[n]
                .counter;
    }
    j["errors"]["system"]["last_critical"] = json::array();
    for (std::uint8_t n = 0; n < 5; n++) {
        j["errors"]["system"]["last_critical"][n]["id"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::SYSTEM)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST_CRITICAL)]
                .error[n]
                .id;
        j["errors"]["system"]["last_critical"][n]["priority"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::SYSTEM)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST_CRITICAL)]
                .error[n]
                .priority;
        j["errors"]["system"]["last_critical"][n]["counter"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::SYSTEM)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST_CRITICAL)]
                .error[n]
                .counter;
    }
    j["errors"]["communication"] = json();
    j["errors"]["communication"]["last"] = json::array();
    for (std::uint8_t n = 0; n < 5; n++) {
        j["errors"]["communication"]["last"][n]["id"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::COMMUNICATION)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST)]
                .error[n]
                .id;
        j["errors"]["communication"]["last"][n]["priority"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::COMMUNICATION)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST)]
                .error[n]
                .priority;
        j["errors"]["communication"]["last"][n]["counter"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::COMMUNICATION)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST)]
                .error[n]
                .counter;
    }
    j["errors"]["communication"]["last_critical"] = json::array();
    for (std::uint8_t n = 0; n < 5; n++) {
        j["errors"]["communication"]["last_critical"][n]["id"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::COMMUNICATION)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST_CRITICAL)]
                .error[n]
                .id;
        j["errors"]["communication"]["last_critical"][n]["priority"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::COMMUNICATION)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST_CRITICAL)]
                .error[n]
                .priority;
        j["errors"]["communication"]["last_critical"][n]["counter"] =
            k.source[static_cast<std::uint8_t>(app_layer::ErrorSource::COMMUNICATION)]
                .category[static_cast<std::uint8_t>(app_layer::ErrorCategory::LAST_CRITICAL)]
                .error[n]
                .counter;
    }
}

void from_json(const json& j, Logging& k) {
    // n/a
    EVLOG_error << "[Logging][from_json()] not implemented";
}

std::ostream& operator<<(std::ostream& os, const Logging& k) {
    os << json(k).dump(4);
    return os;
}

namespace conversions {
std::string hexdump(const std::vector<std::uint8_t>& msg) {
    std::stringstream ss;
    for (auto index : msg) {
        ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(index) << " ";
    }
    return ss.str();
}

std::string hexdump(const std::vector<std::uint8_t>& msg, std::uint8_t start, std::uint8_t number_of_chars) {
    if ((start + number_of_chars) > msg.size())
        return std::string{};
    std::stringstream ss;
    for (std::uint8_t n = start; n < (start + number_of_chars); n++) {
        ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(msg.at(n));
        if (n < (start + number_of_chars - 1))
            ss << " ";
    }
    return ss.str();
}

std::string hexdump(std::uint8_t msg) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(msg);
    return ss.str();
}

std::string hexdump(std::uint16_t msg) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << static_cast<std::uint16_t>(msg);
    return ss.str();
}

std::string hexdump(std::uint64_t msg) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << static_cast<std::uint64_t>(msg);
    return ss.str();
}

std::string get_string(const std::vector<std::uint8_t>& vec) {
    std::string str{};
    for (std::uint16_t n = 0; n < vec.size(); n++) {
        if ((vec[n] < ' ') || (vec[n] > '~')) {
            str += " ";
        } else {
            str += vec[n];
        }
    }
    return str;
}

std::string u32_epoch_to_rfc3339(std::uint32_t epoch_time) {
    time_t tt = static_cast<time_t>(epoch_time);
    std::tm tm = *std::gmtime(&tt);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S.000Z");
    return ss.str();
}

} // namespace conversions
} // namespace module
