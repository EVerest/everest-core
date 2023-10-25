// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <diagnostics.hpp>

namespace module {

void to_json(json& j, const DeviceData& k) {
    // EVLOG_error << "[DeviceData][to_json()] start";
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
//    j["OCMF_info"] = json();
    j["total_dev_import_energy_Wh"] = k.total_dev_import_energy_Wh;
    j["status"] = module::conversions::to_bin_string(k.ab_status);
    // EVLOG_error << "[DeviceData][to_json()] end";
}

void from_json(const json& j, DeviceData& k) {
    // k.utc_time_s = j.at("");
    // k.gmt_offset_quarterhours = j.at("");
    // k.total_start_import_energy_Wh = j.at("");
    // k.total_stop_import_energy_Wh = j.at("");
    // k.total_transaction_duration_s = j.at("");
    // k.ocmf_stats.number_transactions = j.at("");
    // k.ocmf_stats.timestamp_first_transaction = j.at("");
    // k.ocmf_stats.timestamp_last_transaction = j.at("");
    // k.ocmf_stats.max_number_of_transactions = j.at("");
    // k.last_ocmf_transaction = j.at("");
    // k.total_dev_import_energy_Wh = j.at("");
    EVLOG_error << "[DeviceData][from_json()] not implemented";
}

std::ostream& operator<<(std::ostream& os, const DeviceData& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const DeviceDiagnostics& k) {
    // EVLOG_error << "[DeviceDiagnostics][to_json()] start";
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
    j["dev_info"]["bus_address"] = k.dev_info.bus_address;
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
    if(k.ocmf_config_table.size() > 0) {
        for (uint8_t n = 0; n < k.ocmf_config_table.size(); n++) {
            j["ocmf_config_table"][n] = module::conversions::hexdump((uint8_t)k.ocmf_config_table.at(n));
        }
    }
    // EVLOG_error << "[DeviceDiagnostics][to_json()] end";
}

void from_json(const json& j, DeviceDiagnostics& k) {
    // k.charge_point_id = j.at("");
    // k.log_stats.number_log_entries = j.at("");
    // k.log_stats.timestamp_first_log = j.at("");
    // k.log_stats.timestamp_last_log = j.at("");
    // k.log_stats.max_number_of_logs = j.at("");
    // k.dev_info.type = j.at("");
    // k.dev_info.hw_ver = j.at("");
    // k.dev_info.server_id = j.at("");
    // k.dev_info.serial_number = j.at("");
    // k.dev_info.application.mode = j.at("");
    // k.dev_info.application.fw_ver = j.at("");
    // k.dev_info.application.fw_crc = j.at("");
    // k.dev_info.application.fw_hash = j.at("");
    // k.dev_info.metering.fw_ver = j.at("");
    // k.dev_info.metering.fw_crc = j.at("");
    // k.dev_info.bus_address = j.at("");
    // k.dev_info.bootl_ver = j.at("");
    EVLOG_error << "[DeviceDiagnostics][from_json()] not implemented";
}

std::ostream& operator<<(std::ostream& os, const DeviceDiagnostics& k) {
    os << json(k).dump(4);
    return os;
}

void to_json(json& j, const Logging& k) {
    // EVLOG_error << "[Logging][to_json()] start";
    j["log"] = json();
    j["log"]["last"] = json();
    j["log"]["last"]["type"] = "" + std::to_string((int)k.last_log.type) + ": " + log_type_to_string(k.last_log.type);
    j["log"]["last"]["second_index"] = k.last_log.second_index;
    // j["log"]["last"]["utc_time"] = k.last_log.utc_time;
    j["log"]["last"]["utc_time"] = module::conversions::u32_epoch_to_rfc3339(k.last_log.utc_time);
    j["log"]["last"]["utc_offset_quarterhours"] = k.last_log.utc_offset;
    j["log"]["last"]["old_value"] = module::conversions::hexdump(k.last_log.old_value);
    j["log"]["last"]["new_value"] = module::conversions::hexdump(k.last_log.new_value);
    j["log"]["last"]["server_id"] = module::conversions::hexdump(k.last_log.server_id);
    j["log"]["last"]["signature"] = module::conversions::hexdump(k.last_log.signature);

    j["errors"] = json();
    j["errors"]["system"] = json();
    j["errors"]["system"]["last"] = json::array();
    for (uint8_t n = 0; n < 5; n++) {
        j["errors"]["system"]["last"][n]["id"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::SYSTEM].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST].error[n].id;
        j["errors"]["system"]["last"][n]["priority"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::SYSTEM].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST].error[n].priority;
        j["errors"]["system"]["last"][n]["counter"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::SYSTEM].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST].error[n].counter;
    }
    j["errors"]["system"]["last_critical"] = json::array();
    for (uint8_t n = 0; n < 5; n++) {
        j["errors"]["system"]["last_critical"][n]["id"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::SYSTEM].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST_CRITICAL].error[n].id;
        j["errors"]["system"]["last_critical"][n]["priority"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::SYSTEM].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST_CRITICAL].error[n].priority;
        j["errors"]["system"]["last_critical"][n]["counter"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::SYSTEM].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST_CRITICAL].error[n].counter;
    }
    j["errors"]["communication"] = json();
    j["errors"]["communication"]["last"] = json::array();
    for (uint8_t n = 0; n < 5; n++) {
        j["errors"]["communication"]["last"][n]["id"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::COMMUNICATION].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST].error[n].id;
        j["errors"]["communication"]["last"][n]["priority"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::COMMUNICATION].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST].error[n].priority;
        j["errors"]["communication"]["last"][n]["counter"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::COMMUNICATION].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST].error[n].counter;
    }
    j["errors"]["communication"]["last_critical"] = json::array();
    for (uint8_t n = 0; n < 5; n++) {
        j["errors"]["communication"]["last_critical"][n]["id"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::COMMUNICATION].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST_CRITICAL].error[n].id;
        j["errors"]["communication"]["last_critical"][n]["priority"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::COMMUNICATION].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST_CRITICAL].error[n].priority;
        j["errors"]["communication"]["last_critical"][n]["counter"] = k.source[(uint8_t)gsh01_app_layer::ErrorSource::COMMUNICATION].category[(uint8_t)gsh01_app_layer::ErrorCategory::LAST_CRITICAL].error[n].counter;
    }
    // EVLOG_error << "[Logging][to_json()] end";
}

void from_json(const json& j, Logging& k) {
    // n/a
    EVLOG_error << "[Logging][from_json()] not implemented";
}

std::ostream& operator<<(std::ostream& os, const Logging& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace module
