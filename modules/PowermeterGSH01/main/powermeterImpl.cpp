// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"

#include <fmt/core.h>
#include "Timeout.hpp"

namespace module {
namespace main {

inline uint16_t get_u16(std::vector<uint8_t>& vec, uint8_t start_index) {
    if (vec.size() < start_index + 1) return 0;
    return std::move(((uint16_t)vec[start_index + 1] <<  8) | 
                      (uint16_t)vec[start_index]);
}

inline uint16_t get_u16(std::vector<uint8_t>& vec) {
    return std::move(get_u16(vec, 0));
}

inline uint32_t get_u32(std::vector<uint8_t>& vec, uint8_t start_index) {
    if (vec.size() < start_index + 3) return 0;
    return std::move(((uint32_t)vec[start_index + 3] << 24) | 
                     ((uint32_t)vec[start_index + 2] << 16) | 
                     ((uint32_t)vec[start_index + 1] <<  8) | 
                      (uint32_t)vec[start_index]);
}

inline uint32_t get_u32(std::vector<uint8_t>& vec) {
    return std::move(get_u32(vec, 0));
}

inline uint64_t get_u64(std::vector<uint8_t>& vec, uint8_t start_index) {
    if (vec.size() < start_index + 7) return 0;
    return std::move(((uint64_t)vec[start_index + 7] << 56) |
                     ((uint64_t)vec[start_index + 6] << 48) |
                     ((uint64_t)vec[start_index + 5] << 40) |
                     ((uint64_t)vec[start_index + 4] << 32) |
                     ((uint64_t)vec[start_index + 3] << 24) | 
                     ((uint64_t)vec[start_index + 2] << 16) | 
                     ((uint64_t)vec[start_index + 1] <<  8) | 
                      (uint64_t)vec[start_index]);
}

inline uint64_t get_u64(std::vector<uint8_t>& vec) {
    return std::move(get_u64(vec, 0));
}

inline std::string get_str(std::vector<uint8_t>& vec, uint8_t start_index, uint8_t length) {
    std::string str = "";
    if ((start_index + length) <= vec.size()) {
        for (uint16_t n = start_index; n < (start_index + length); n++){
            if (vec[n] == 0x00) break;
            str += vec[n];
        }
    }
    return std::move(str);
}

void powermeterImpl::init() {
    if (!this->serial_device.open_device(config.serial_port, config.baudrate, config.ignore_echo)) {
        EVLOG_AND_THROW(Everest::EverestConfigError(fmt::format("Cannot open serial port {}.", config.serial_port)));
    }
    this->init_default_values();

    request_device_type();
    set_device_time();
}

void powermeterImpl::ready() {
    std::thread ([this] {
        while (true) {
            read_powermeter_values();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }).detach();

    // create device_data publisher thread
    if (this->config.publish_device_data) {
        std::thread ([this] {
            while (true) {
                read_device_data();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                publish_device_data_topic();
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }).detach();
    }

    // create device_diagnostics publisher thread
    if (this->config.publish_device_diagnostics) {
        std::thread ([this] {
            while (true) {
                read_diagnostics_data();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                publish_device_diagnostics_topic();
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }
        }).detach();
    }

    // create logging publisher thread
    if (this->config.publish_device_diagnostics) {
        std::thread ([this] {
            while (true) {
                publish_logging_topic();
                std::this_thread::sleep_for(std::chrono::seconds(20));
            }
        }).detach();
    }

    // create error diagnostics thread
    if (this->config.publish_device_diagnostics) {
        std::thread ([this] {
            while (true) {
                if (this->error_diagnostics_target != 0) {
                    error_diagnostics(this->error_diagnostics_target);
                    this->error_diagnostics_target = 0;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
        }).detach();
    }
}

void powermeterImpl::set_device_time() {
    std::chrono::time_point<std::chrono::system_clock> timepoint = std::chrono::system_clock::now();
    int8_t gmt_offset_quarters_of_an_hour = app_layer.get_utc_offset_in_quarter_hours(timepoint);

    std::vector<uint8_t> set_device_time_cmd{};
    app_layer.create_command_set_time(date::utc_clock::from_sys(timepoint), gmt_offset_quarters_of_an_hour, set_device_time_cmd);

    std::vector<uint8_t> slip_msg_set_device_time = std::move(this->slip.package_single(this->config.powermeter_device_id, set_device_time_cmd));
    this->serial_device.tx(slip_msg_set_device_time);
    receive_response();
}

void powermeterImpl::set_device_charge_point_id(ast_app_layer::UserIdType id_type, std::string charge_point_id) {
    std::vector<uint8_t> set_charge_point_id_cmd{};
    app_layer.create_command_set_charge_point_id(id_type, charge_point_id, set_charge_point_id_cmd);
    
    std::vector<uint8_t> slip_msg_set_charge_point_id = std::move(this->slip.package_single(this->config.powermeter_device_id, set_charge_point_id_cmd));
    this->serial_device.tx(slip_msg_set_charge_point_id);
    receive_response();
}

void powermeterImpl::publish_device_data_topic() {
    if (config.publish_device_data) {
        json j;
        to_json(j, device_data_obj);
        std::string dev_data_topic = this->pm_last_values.meter_id.value() + std::string("/device_data");
        mod->mqtt.publish(dev_data_topic, j.dump());
    }
}

void powermeterImpl::publish_device_diagnostics_topic() {
    if (config.publish_device_diagnostics) {
        json j;
        to_json(j, device_diagnostics_obj);
        std::string dev_diagnostics_topic = this->pm_last_values.meter_id.value() + std::string("/device_diagnostics");
        mod->mqtt.publish(dev_diagnostics_topic, j.dump());
    }
}

void powermeterImpl::publish_logging_topic() {
    if (config.publish_device_diagnostics) {
        json j;
        to_json(j, logging_obj);
        std::string logging_topic = this->pm_last_values.meter_id.value() + std::string("/logging");
        mod->mqtt.publish(logging_topic, j.dump());
    }
}

void powermeterImpl::read_device_data() {
    {
        std::vector<uint8_t> get_time_cmd{};
        app_layer.create_command_get_time(get_time_cmd);

        std::vector<uint8_t> get_total_start_import_energy_cmd{};
        app_layer.create_command_get_total_start_import_energy(get_total_start_import_energy_cmd);

        std::vector<uint8_t> get_total_stop_import_energy_cmd{};
        app_layer.create_command_get_total_stop_import_energy(get_total_stop_import_energy_cmd);

        std::vector<uint8_t> get_total_start_export_energy_cmd{};
        app_layer.create_command_get_total_start_export_energy(get_total_start_export_energy_cmd);

        std::vector<uint8_t> get_total_stop_export_energy_cmd{};
        app_layer.create_command_get_total_stop_export_energy(get_total_stop_export_energy_cmd);

        // std::vector<uint8_t> get_total_transaction_duration_cmd{};
        // app_layer.create_command_get_total_transaction_duration(get_total_transaction_duration_cmd);

        std::vector<uint8_t> get_ocmf_stats_cmd{};
        app_layer.create_command_get_ocmf_stats(get_ocmf_stats_cmd);

        std::vector<uint8_t> get_last_transaction_ocmf_cmd{};
        app_layer.create_command_get_last_transaction_ocmf(get_last_transaction_ocmf_cmd);

        std::vector<uint8_t> get_ocmf_info_cmd{};
        app_layer.create_command_get_ocmf_info(get_ocmf_info_cmd);

        std::vector<uint8_t> get_ocmf_config_cmd{};
        app_layer.create_command_get_ocmf_config(get_ocmf_config_cmd);

        std::vector<uint8_t> slip_msg_read_device_data = std::move(this->slip.package_multi(this->config.powermeter_device_id,
                                                                                            {
                                                                                                get_time_cmd,
                                                                                                get_total_start_import_energy_cmd,
                                                                                                get_total_stop_import_energy_cmd,
                                                                                                get_total_start_export_energy_cmd,
                                                                                                get_total_stop_export_energy_cmd,
                                                                                                // get_total_transaction_duration_cmd,
                                                                                                get_ocmf_stats_cmd,
                                                                                                get_last_transaction_ocmf_cmd,
                                                                                                get_ocmf_info_cmd,
                                                                                                get_ocmf_config_cmd
                                                                                            }));
        this->serial_device.tx(slip_msg_read_device_data);
        receive_response();
    }
    {
        std::vector<uint8_t> get_total_dev_import_energy_cmd{};
        app_layer.create_command_get_total_dev_import_energy(get_total_dev_import_energy_cmd);

        std::vector<uint8_t> get_total_dev_export_energy_cmd{};
        app_layer.create_command_get_total_dev_export_energy(get_total_dev_export_energy_cmd);

        std::vector<uint8_t> get_application_board_status_cmd{};
        app_layer.create_command_get_application_board_status(get_application_board_status_cmd);

        std::vector<uint8_t> slip_msg_read_device_data_2 = std::move(this->slip.package_multi(this->config.powermeter_device_id,
                                                                                              {
                                                                                                  get_total_dev_import_energy_cmd,
                                                                                                  get_total_dev_export_energy_cmd,
                                                                                                  get_application_board_status_cmd
                                                                                              }));
        this->serial_device.tx(slip_msg_read_device_data_2);
        receive_response();
    }
}

void powermeterImpl::get_device_public_key() {
    {
        std::vector<uint8_t> get_device_public_key_cmd{};
        app_layer.create_command_get_meter_pubkey(get_device_public_key_cmd);

        std::vector<uint8_t> get_device_public_key_asn1_cmd{};
        app_layer.create_command_get_pubkey_asn1(get_device_public_key_asn1_cmd);

        std::vector<uint8_t> get_device_public_key_str16_cmd{};
        app_layer.create_command_get_pubkey_str16(get_device_public_key_str16_cmd);

        std::vector<uint8_t> slip_msg_get_device_public_keys = std::move(this->slip.package_multi(this->config.powermeter_device_id,
                                                                                                  {
                                                                                                      get_device_public_key_cmd,
                                                                                                      get_device_public_key_asn1_cmd,
                                                                                                      get_device_public_key_str16_cmd
                                                                                                  }));
        this->serial_device.tx(slip_msg_get_device_public_keys);
        receive_response();

        // std::vector<uint8_t> get_device_public_key_cmd{};
        // // TODO (LAD): check which is best / applies to use case
        // // create_command_get_pubkey_str16()
        // // create_command_get_pubkey_asn1()
        // // create_command_get_meter_pubkey()
        // app_layer.create_command_get_pubkey_str16(get_device_public_key_cmd);

        // std::vector<uint8_t> slip_msg_get_device_public_key = std::move(this->slip.package_single(this->config.powermeter_device_id, get_device_public_key_cmd));

        // this->serial_device.tx(slip_msg_get_device_public_key);
        // receive_response();
    }
}

void powermeterImpl::request_device_type() {
    std::vector<uint8_t> data_vect{};
    app_layer.create_command_get_device_type(data_vect);
    std::vector<uint8_t> slip_msg_device_type = std::move(this->slip.package_single(this->config.powermeter_device_id, data_vect));
    this->serial_device.tx(slip_msg_device_type);
    receive_response();
}

void powermeterImpl::request_error_diagnostics(uint8_t addr) {
    this->error_diagnostics_target = addr;
}

/* retrieve last error objects from device */
void powermeterImpl::error_diagnostics(uint8_t addr) {
    std::vector<uint8_t> last_log_entry_cmd{};
    app_layer.create_command_get_last_log_entry(last_log_entry_cmd);
    std::vector<uint8_t> slip_msg_last_log_entry = std::move(this->slip.package_single(this->config.powermeter_device_id, last_log_entry_cmd));
    this->serial_device.tx(slip_msg_last_log_entry);
    receive_response();

    std::vector<uint8_t> last_system_errors_cmd{};
    app_layer.create_command_get_errors(ast_app_layer::ErrorCategory::LAST,
                                        ast_app_layer::ErrorSource::SYSTEM,
                                        last_system_errors_cmd);
    category_requested = ast_app_layer::ErrorCategory::LAST;
    source_requested = ast_app_layer::ErrorSource::SYSTEM;
    std::vector<uint8_t> slip_msg_last_system_errors = std::move(this->slip.package_single(this->config.powermeter_device_id, last_system_errors_cmd));
    this->serial_device.tx(slip_msg_last_system_errors);
    receive_response();

    std::vector<uint8_t> last_critical_system_errors_cmd{};
    app_layer.create_command_get_errors(ast_app_layer::ErrorCategory::LAST_CRITICAL,
                                        ast_app_layer::ErrorSource::SYSTEM,
                                        last_critical_system_errors_cmd);
    category_requested = ast_app_layer::ErrorCategory::LAST_CRITICAL;
    source_requested = ast_app_layer::ErrorSource::SYSTEM;
    std::vector<uint8_t> slip_msg_last_critical_system_errors = std::move(this->slip.package_single(this->config.powermeter_device_id, last_critical_system_errors_cmd));
    this->serial_device.tx(slip_msg_last_critical_system_errors);
    receive_response();

    std::vector<uint8_t> last_comm_errors_cmd{};
    app_layer.create_command_get_errors(ast_app_layer::ErrorCategory::LAST,
                                        ast_app_layer::ErrorSource::COMMUNICATION,
                                        last_comm_errors_cmd);
    category_requested = ast_app_layer::ErrorCategory::LAST;
    source_requested = ast_app_layer::ErrorSource::COMMUNICATION;
    std::vector<uint8_t> slip_msg_last_communication_errors = std::move(this->slip.package_single(this->config.powermeter_device_id, last_comm_errors_cmd));
    this->serial_device.tx(slip_msg_last_communication_errors);
    receive_response();

    std::vector<uint8_t> last_critical_comm_errors_cmd{};
    app_layer.create_command_get_errors(ast_app_layer::ErrorCategory::LAST_CRITICAL,
                                        ast_app_layer::ErrorSource::COMMUNICATION,
                                        last_critical_comm_errors_cmd);
    category_requested = ast_app_layer::ErrorCategory::LAST_CRITICAL;
    source_requested = ast_app_layer::ErrorSource::COMMUNICATION;
    std::vector<uint8_t> slip_msg_last_critical_communication_errors = std::move(this->slip.package_single(this->config.powermeter_device_id, last_critical_comm_errors_cmd));
    this->serial_device.tx(slip_msg_last_critical_communication_errors);
    receive_response();
}

void powermeterImpl::read_diagnostics_data() {

    // part 1 - basic info
    {
        std::vector<uint8_t> get_charge_point_id_cmd{};
        app_layer.create_command_get_charge_point_id(get_charge_point_id_cmd);

        std::vector<uint8_t> get_device_type_cmd{};
        app_layer.create_command_get_device_type(get_device_type_cmd);
        
        std::vector<uint8_t> get_hardware_version_cmd{};
        app_layer.create_command_get_hardware_version(get_hardware_version_cmd);

        std::vector<uint8_t> get_application_board_server_id_cmd{};
        app_layer.create_command_get_application_board_server_id(get_application_board_server_id_cmd);

        std::vector<uint8_t> get_application_board_mode_cmd{};
        app_layer.create_command_get_application_board_mode(get_application_board_mode_cmd);

        std::vector<uint8_t> slip_msg_get_diagnostics_data_1 = std::move(this->slip.package_multi(this->config.powermeter_device_id,
                                                                                                  {
                                                                                                      get_charge_point_id_cmd,
                                                                                                      get_device_type_cmd,
                                                                                                      get_hardware_version_cmd,
                                                                                                      get_application_board_server_id_cmd,
                                                                                                      get_application_board_mode_cmd
                                                                                                  }));
        this->serial_device.tx(slip_msg_get_diagnostics_data_1);
        receive_response();
    }

    // part 2 - log stats
    {
        std::vector<uint8_t> get_log_stats_cmd{};
        app_layer.create_command_get_log_stats(get_log_stats_cmd);

        std::vector<uint8_t> slip_msg_get_diagnostics_data_2 = std::move(this->slip.package_single(this->config.powermeter_device_id, get_log_stats_cmd));
        this->serial_device.tx(slip_msg_get_diagnostics_data_2);
        receive_response();
    }

    // part 3 - HW/SW info
    {
        std::vector<uint8_t> get_application_board_serial_number_cmd{};
        app_layer.create_command_get_application_board_serial_number(get_application_board_serial_number_cmd);

        std::vector<uint8_t> get_application_board_software_version_cmd{};
        app_layer.create_command_get_application_board_software_version(get_application_board_software_version_cmd);
        
        std::vector<uint8_t> get_application_board_fw_checksum_cmd{};
        app_layer.create_command_get_application_board_fw_checksum(get_application_board_fw_checksum_cmd);

        std::vector<uint8_t> get_application_board_fw_hash_cmd{};
        app_layer.create_command_get_application_board_fw_hash(get_application_board_fw_hash_cmd);

        std::vector<uint8_t> get_metering_board_software_version_cmd{};
        app_layer.create_command_get_metering_board_software_version(get_metering_board_software_version_cmd);

        std::vector<uint8_t> get_metering_board_fw_checksum_cmd{};
        app_layer.create_command_get_metering_board_fw_checksum(get_metering_board_fw_checksum_cmd);
        
        std::vector<uint8_t> get_ocmf_config_cmd{};
        app_layer.create_command_get_ocmf_config(get_ocmf_config_cmd);

        std::vector<uint8_t> slip_msg_get_diagnostics_data_3 = std::move(this->slip.package_multi(this->config.powermeter_device_id,
                                                                                                  {
                                                                                                      get_application_board_serial_number_cmd,
                                                                                                      get_application_board_software_version_cmd,
                                                                                                      get_application_board_fw_checksum_cmd,
                                                                                                      get_application_board_fw_hash_cmd,
                                                                                                      get_metering_board_software_version_cmd,
                                                                                                      get_metering_board_fw_checksum_cmd,
                                                                                                      get_ocmf_config_cmd
                                                                                                  }));
        this->serial_device.tx(slip_msg_get_diagnostics_data_3);
        receive_response();
    }
    // part 4 - public key
    get_device_public_key();
}

void powermeterImpl::read_powermeter_values() {
    this->readRegisters();
    this->pm_last_values.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
}

void powermeterImpl::init_default_values() {
    this->pm_last_values.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    this->pm_last_values.meter_id = "AST_Powermeter_addr_" + std::to_string(this->config.powermeter_device_id);

    this->pm_last_values.energy_Wh_import.total = 0.0f;
    // this->pm_last_values.energy_Wh_import.L1 = 0.0f;

    types::units::Energy energy_Wh;
    energy_Wh.total = 0.0f;
    this->pm_last_values.energy_Wh_export = energy_Wh;
    // this->pm_last_values.energy_Wh_export.L1 = 0.0f;

    types::units::Power power_W;
    power_W.total = 0.0f;
    this->pm_last_values.power_W = power_W;
    // this->pm_last_values.power_W.L1 = 0.0f;

    types::units::Voltage voltage_V;
    voltage_V.DC = 0.0f;
    this->pm_last_values.voltage_V = voltage_V;
    // this->pm_last_values.voltage_V.L1 = 0.0f;

    types::units::Current current_A;
    current_A.DC = 0.0f;
    this->pm_last_values.current_A = current_A;
}

void powermeterImpl::readRegisters() {
    std::vector<uint8_t> get_voltage_cmd{};
    app_layer.create_command_get_voltage(get_voltage_cmd);

    std::vector<uint8_t> get_current_cmd{};
    app_layer.create_command_get_current(get_current_cmd);

    std::vector<uint8_t> get_import_power_cmd{};
    app_layer.create_command_get_import_power(get_import_power_cmd);

    std::vector<uint8_t> export_power_cmd{};
    app_layer.create_command_get_export_power(export_power_cmd);

    std::vector<uint8_t> get_total_power_cmd{};
    app_layer.create_command_get_total_power(get_total_power_cmd);

    std::vector<uint8_t> slip_msg_read_registers = std::move(this->slip.package_multi(this->config.powermeter_device_id,
                                                                                      {
                                                                                          get_voltage_cmd,
                                                                                          get_current_cmd,
                                                                                          get_import_power_cmd,
                                                                                          export_power_cmd,
                                                                                          get_total_power_cmd
                                                                                      }));
    this->serial_device.tx(slip_msg_read_registers);
    receive_response();
}


// ############################################################################################################################################
// ############################################################################################################################################

ast_app_layer::CommandResult powermeterImpl::process_response(const std::vector<uint8_t>& response_message) {
    ast_app_layer::CommandResult response_status{ast_app_layer::CommandResult::OK};
    size_t response_size = response_message.size();

    // split into multiple command responses
    uint8_t dest_addr = response_message.at(0);
    uint16_t i = 1;
    while ((i + 4) <= response_size) {
        uint16_t part_cmd = ((uint16_t)response_message.at(i + 1) << 8) | response_message.at(i);
        uint16_t part_len = ((uint16_t)response_message.at(i + 3) << 8) | response_message.at(i + 2);
        uint16_t part_data_len = part_len - 5;
        ast_app_layer::CommandResult part_status = static_cast<ast_app_layer::CommandResult>(response_message.at(i + 4));
        if (response_status == ast_app_layer::CommandResult::OK) {  // only update response status if not already error present
            response_status = part_status;
        }

        if ((i + part_len - 1) <= response_size) {
            std::vector<uint8_t> part_data((response_message.begin() + i + 5), (response_message.begin() + i + part_len));

            // EVLOG_info << "\n\n"
            //             << "response received from ID " << int(dest_addr) << ": \n"
            //             << "   cmd: " << module::conversions::hexdump(part_cmd) 
            //             << "   len: " << part_len 
            //             << "   status: " << (int)part_status
            //             << "   data: " << ((part_len > 5) ? module::conversions::hexdump(part_data) : "none") 
            //             << "\n\n";

            if (part_status != ast_app_layer::CommandResult::OK) {
                EVLOG_error << "Powermeter at address " << int(dest_addr) << " (" 
                            << module::conversions::hexdump(dest_addr) << ")"
                            << " has signaled an error (status: (" << (int)part_status << ") \"" 
                            << ast_app_layer::command_result_to_string(part_status) 
                            << "\") at response " << module::conversions::hexdump(part_cmd) << " !";

                // skip error diagnostics for transaction or error diagnostics related commands,
                // request detailed error report for others
                if ((part_cmd != (uint16_t)ast_app_layer::CommandType::START_TRANSACTION)  &&
                    (part_cmd != (uint16_t)ast_app_layer::CommandType::STOP_TRANSACTION)   &&
                    (part_cmd != (uint16_t)ast_app_layer::CommandType::GET_LAST_LOG_ENTRY) &&
                    (part_cmd != (uint16_t)ast_app_layer::CommandType::GET_ERRORS)         &&
                    (part_cmd != (uint16_t)ast_app_layer::CommandType::GET_LAST_OCMF)) {
                    EVLOG_info << "Retrieving diagnostics data for error at command " << module::conversions::hexdump(part_cmd) << "...";
                    request_error_diagnostics(dest_addr);
                    i += part_len;  // skip remaining data and go to next command in message
                    continue; 
                }
            }

            // process response
            switch (part_cmd) {

            // operational values

                case (int)ast_app_layer::CommandType::START_TRANSACTION:
                    {
                        this->start_transaction_msg_status = MessageStatus::RECEIVED;
                        this->start_transact_result = part_status;
                        EVLOG_info << "START_TRANSACTION received.";
                    }
                    break;

                case (int)ast_app_layer::CommandType::STOP_TRANSACTION:
                    {
                        this->stop_transaction_msg_status = MessageStatus::RECEIVED;
                        this->stop_transact_result = part_status;
                        EVLOG_info << "STOP_TRANSACTION received.";
                    }
                    break;

                case (int)ast_app_layer::CommandType::TIME:
                    {
                        if (part_data_len < 5) break;
                        device_data_obj.utc_time_s = get_u32(part_data);
                        device_data_obj.gmt_offset_quarterhours = part_data[4];
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_VOLTAGE_L1:
                    {
                        if (part_data_len < 4) break;
                        types::units::Voltage volt = this->pm_last_values.voltage_V.value();
                        volt.DC = (float)get_u32(part_data) / 100.0; // powermeter reports in 100 * [V]
                        this->pm_last_values.voltage_V = volt;
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_CURRENT_L1:
                    {
                        if (part_data_len < 4) break;
                        types::units::Current amp = this->pm_last_values.current_A.value();
                        amp.DC = (float)get_u32(part_data) / 1000.0;  // powermeter reports in [mA]
                        this->pm_last_values.current_A = amp;
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_IMPORT_DEV_POWER:
                    {
                        if (part_data_len < 4) break;
                        types::units::Power power = this->pm_last_values.power_W.value();
                        power.total = (float)get_u32(part_data) / 100.0;  // powermeter reports in [W * 100]
                        this->pm_last_values.power_W = power;
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_EXPORT_DEV_POWER:
                    {
                        EVLOG_info << "(GET_EXPORT_DEV_POWER) Not yet implemented. [" << (float)get_u32(part_data) / 100.0  /* powermeter reports in [W * 100] */ << " W]";
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_TOTAL_DEV_POWER:
                    {
                        EVLOG_info << "(GET_TOTAL_DEV_POWER) Not yet implemented. [" << (float)get_u32(part_data) / 100.0  /* powermeter reports in [W * 100] */ << " W]";
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_TOTAL_IMPORT_DEV_ENERGY:
                    {
                        if (part_data_len < 8) break;
                        types::units::Energy energy_in = this->pm_last_values.energy_Wh_import;
                        energy_in.total = (float)get_u64(part_data) / 10.0;  // powermeter reports in [Wh * 10]
                        this->pm_last_values.energy_Wh_import = energy_in;

                        device_data_obj.total_dev_import_energy_Wh = get_u64(part_data) / 10.0;  // powermeter reports in [Wh * 10]
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_TOTAL_EXPORT_DEV_ENERGY:
                    {
                        if (part_data_len < 8) break;
                        types::units::Energy energy_out{};
                        if (this->pm_last_values.energy_Wh_export.has_value()) {
                            energy_out = this->pm_last_values.energy_Wh_export.value();
                        }
                        energy_out.total = (float)get_u64(part_data) / 10.0;  // powermeter reports in [Wh * 10]
                        this->pm_last_values.energy_Wh_export = energy_out;

                        device_data_obj.total_dev_export_energy_Wh = get_u64(part_data) / 10.0;  // powermeter reports in [Wh * 10]
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_TOTAL_START_IMPORT_DEV_ENERGY:
                    {
                        if (part_data_len < 8) break;
                        device_data_obj.total_start_import_energy_Wh = get_u64(part_data) / 10.0;  // powermeter reports in [Wh * 10]
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_TOTAL_STOP_IMPORT_DEV_ENERGY:
                    {
                        if (part_data_len < 8) break;
                        device_data_obj.total_stop_import_energy_Wh = get_u64(part_data) / 10.0;  // powermeter reports in [Wh * 10]  
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_TOTAL_START_EXPORT_DEV_ENERGY:
                    {
                        if (part_data_len < 8) break;
                        device_data_obj.total_start_export_energy_Wh = get_u64(part_data) / 10.0;  // powermeter reports in [Wh * 10]  
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_TOTAL_STOP_EXPORT_DEV_ENERGY:
                    {
                        if (part_data_len < 8) break;
                        device_data_obj.total_stop_export_energy_Wh = get_u64(part_data) / 10.0;  // powermeter reports in [Wh * 10]
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_TRANSACT_TOTAL_DURATION:
                    {
                        if (part_data_len < 4) break;
                        device_data_obj.total_transaction_duration_s = get_u32(part_data);
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_PUBKEY_STR16:
                    {
                        if (part_data_len < 130) break;
                        device_diagnostics_obj.pubkey_str16_format = part_data[0];
                        device_diagnostics_obj.pubkey_str16 = module::conversions::hexdump(part_data, 1, 129);
                        // device_diagnostics_obj.pubkey_str16 = module::conversions::hexdump(part_data, 0, 130);
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_PUBKEY_ASN1:
                    {
                        if (part_data_len < 176) break;
                        device_diagnostics_obj.pubkey_asn1 = module::conversions::hexdump(part_data, 0, 176);
                    }
                    break;

                case (int)ast_app_layer::CommandType::REQUEST_METER_PUBKEY:
                    {
                        if (part_data_len < 65) break;
                        device_diagnostics_obj.pubkey_format = part_data[0];
                        device_diagnostics_obj.pubkey = module::conversions::hexdump(part_data, 1, 64);
                    }
                    break;

            // diagnostics

                case (int)ast_app_layer::CommandType::OCMF_STATS:
                    {
                        if (part_data_len < 16) break;
                        device_data_obj.ocmf_stats.number_transactions         = get_u32(part_data);
                        device_data_obj.ocmf_stats.timestamp_first_transaction = get_u32(part_data, 4);
                        device_data_obj.ocmf_stats.timestamp_last_transaction  = get_u32(part_data, 8);
                        device_data_obj.ocmf_stats.max_number_of_transactions  = get_u32(part_data, 12);
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_OCMF:
                    {
                        if (part_data_len < 16) break;
                        device_data_obj.requested_ocmf = get_str(part_data, 0, part_data_len);
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_LAST_OCMF:
                    {
                        if (this->get_transaction_values_msg_status == MessageStatus::SENT) {
                            this->get_transaction_values_msg_status = MessageStatus::RECEIVED;
                        }
                        if (part_status == ast_app_layer::CommandResult::OK) {
                            device_data_obj.last_ocmf_transaction = get_str(part_data, 0, part_data_len);
                            this->last_ocmf_str = device_data_obj.last_ocmf_transaction;
                        } else {
                            this->last_ocmf_str = "";
                        }
                    }
                    break;

                case (int)ast_app_layer::CommandType::OCMF_INFO:
                    {
                        if (part_data_len < 1) break;
                        // gateway_id
                        if (part_data_len < part_data[0]) break;  // error, data too short
                        uint8_t length_gateway_id = part_data[0];
                        if (length_gateway_id > 18) length_gateway_id = 18; // max length
                        if (length_gateway_id > 0) {
                            device_data_obj.ocmf_info.gateway_id = get_str(part_data, 1, length_gateway_id);
                            length_gateway_id++;    // add length info byte for following calculations
                        } else {
                            device_data_obj.ocmf_info.gateway_id = "";
                            length_gateway_id = 1; // length info always requires at least one byte
                        }

                        // manufacturer
                        if (part_data_len < (length_gateway_id + part_data[length_gateway_id] + 1)) break;  // error, data too short
                        uint8_t length_manufacturer = part_data[length_gateway_id];
                        if (length_manufacturer > 4) length_manufacturer = 4; // max length
                        if (length_manufacturer > 0) {
                            device_data_obj.ocmf_info.manufacturer = get_str(part_data, length_gateway_id + 1, length_manufacturer);
                            length_manufacturer++;    // add length info byte for following calculations
                        } else {
                            device_data_obj.ocmf_info.manufacturer = "";
                            length_manufacturer = 1; // length info always requires at least one byte
                        }

                        // model
                        if (part_data_len < (length_gateway_id + length_manufacturer + part_data[length_gateway_id + length_manufacturer])) break;  // error, data too short
                        uint8_t length_model = part_data[length_gateway_id + length_manufacturer];
                        if (length_model > 18) length_model = 18; // max length
                        if (length_model > 0) {
                            device_data_obj.ocmf_info.model = get_str(part_data, (length_gateway_id + length_manufacturer + 1), length_model);
                        } else {
                            device_data_obj.ocmf_info.model = "";
                        }
                    }
                    break;

                case (int)ast_app_layer::CommandType::OCMF_CONFIG:
                    {
                        if (part_data_len < 16) break;
                        device_diagnostics_obj.ocmf_config_table.clear();
                        for (uint8_t n = 0; n < part_data_len; n++) {
                            device_diagnostics_obj.ocmf_config_table.push_back(part_data[n]);
                        }
                    }
                    break;

                case (int)ast_app_layer::CommandType::CHARGE_POINT_ID:
                    {
                        if (part_data_len < 14) break;
                        device_diagnostics_obj.charge_point_id_type = part_data[0];
                        device_diagnostics_obj.charge_point_id = get_str(part_data, 1, 13);
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_ERRORS:
                    {
                        if (part_data_len < 50) break;
                        uint j = 0;
                        for (uint8_t i = 0; i < 5; i++) {
                            j = i * 10;
                            logging_obj.source[(uint8_t)source_requested].category[(uint8_t)category_requested].error[i].id       = get_u32(part_data, j);
                            logging_obj.source[(uint8_t)source_requested].category[(uint8_t)category_requested].error[i].priority = get_u16(part_data, j + 4);
                            logging_obj.source[(uint8_t)source_requested].category[(uint8_t)category_requested].error[i].counter  = get_u32(part_data, j + 6);
                            EVLOG_info << "Error #" << (int)i << " for source (" << (int)source_requested << ") and category (" << (int)category_requested 
                                        << "):\nID:      " << get_u32(part_data, j)
                                        << "\nPrio:    " << get_u16(part_data, j + 4)
                                        << "\nCounter: " << get_u32(part_data, j + 6);
                        }
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_LOG_STATS:
                    {
                        if (part_data_len < 16) break;
                        device_diagnostics_obj.log_stats.number_log_entries  = get_u32(part_data);
                        device_diagnostics_obj.log_stats.timestamp_first_log = get_u32(part_data, 4);
                        device_diagnostics_obj.log_stats.timestamp_last_log  = get_u32(part_data, 8);
                        device_diagnostics_obj.log_stats.max_number_of_logs  = get_u32(part_data, 12);
                    }
                    break;

                case (int)ast_app_layer::CommandType::GET_LAST_LOG_ENTRY:
                    {
                        if (part_data_len < 104) break;
                        logging_obj.last_log.type         = static_cast<ast_app_layer::LogType>(part_data[0]);
                        logging_obj.last_log.second_index = get_u32(part_data, 1);
                        logging_obj.last_log.utc_time     = get_u32(part_data, 5);
                        logging_obj.last_log.utc_offset   = part_data[9];
                        for (uint8_t n = 10; n < 20; n++){
                            logging_obj.last_log.old_value.push_back(part_data[n]);
                        }
                        for (uint8_t n = 20; n < 30; n++){
                            logging_obj.last_log.new_value.push_back(part_data[n]);
                        }
                        for (uint8_t n = 30; n < 40; n++){
                            logging_obj.last_log.server_id.push_back(part_data[n]);
                        }
                        for (uint8_t n = 40; n < 104; n++){
                            logging_obj.last_log.signature.push_back(part_data[n]);
                        }
                    }
                    break;

            // device parameters

                case (int)ast_app_layer::CommandType::AB_MODE_SET:
                    {
                        if (part_data_len < 1) break;
                        device_diagnostics_obj.app_board.mode = part_data[0];
                    }
                    break;

                case (int)ast_app_layer::CommandType::AB_HW_VERSION:
                    {
                        uint8_t delimiter_pos = 0;
                        for (uint8_t i = 0; i < part_data_len; i++) {
                            if (part_data[i] == '|') {
                                delimiter_pos = i;
                            }
                        }
                        device_diagnostics_obj.app_board.hw_ver = get_str(part_data, 0, delimiter_pos);
                        device_diagnostics_obj.m_board.hw_ver   = get_str(part_data, delimiter_pos + 1, 
                                                                          (part_data_len - delimiter_pos - 1));
                    }
                    break;

                case (int)ast_app_layer::CommandType::AB_SERVER_ID:
                    {
                        if (part_data_len < 10) break;
                        device_diagnostics_obj.app_board.server_id = get_str(part_data, 0, 10);
                    }
                    break;

                case (int)ast_app_layer::CommandType::AB_SERIAL_NR:
                    {
                        if (part_data_len < 4) break;
                        device_diagnostics_obj.app_board.serial_number = get_u32(part_data);
                    }
                    break;

                case (int)ast_app_layer::CommandType::AB_SW_VERSION:
                    {
                        if (part_data_len < 20) break;
                        device_diagnostics_obj.app_board.sw_ver = get_str(part_data, 0, 20);
                    }
                    break;

                case (int)ast_app_layer::CommandType::AB_FW_CHECKSUM:
                    {
                        if (part_data_len < 2) break;
                        device_diagnostics_obj.app_board.fw_crc = get_u16(part_data);
                    }
                    break;

                case (int)ast_app_layer::CommandType::AB_FW_HASH:
                    {
                        if (part_data_len < 2) break;
                        device_diagnostics_obj.app_board.fw_hash = get_u16(part_data);
                    }
                    break;

                case (int)ast_app_layer::CommandType::MB_SW_VERSION:
                    {
                        if (part_data_len < 20) break;
                        device_diagnostics_obj.m_board.sw_ver = get_str(part_data, 0, 20);
                    }
                    break;

                case (int)ast_app_layer::CommandType::MB_FW_CHECKSUM:
                    {
                        if (part_data_len < 2) break;
                        device_diagnostics_obj.m_board.fw_crc = get_u16(part_data);
                    }
                    break;

                case (int)ast_app_layer::CommandType::AB_DEVICE_TYPE:
                    {
                        if (part_data_len < 18) break;
                        device_diagnostics_obj.app_board.type = get_str(part_data, 0, 18);
                    }
                    break;

                case (int)ast_app_layer::CommandType::AB_STATUS:
                    {
                        if (part_data_len < 8) break;
                        device_data_obj.ab_status = get_u64(part_data);
                    }
                    break;

            // not (yet) implemented

                case (int)ast_app_layer::CommandType::RESET_DC_METER:
                case (int)ast_app_layer::CommandType::MEASUREMENT_MODE:
                case (int)ast_app_layer::CommandType::GET_NORMAL_VOLTAGE:
                case (int)ast_app_layer::CommandType::GET_NORMAL_CURRENT:
                case (int)ast_app_layer::CommandType::GET_MAX_CURRENT:
                case (int)ast_app_layer::CommandType::LINE_LOSS_IMPEDANCE:
                case (int)ast_app_layer::CommandType::LINE_LOSS_MEAS_MODE:
                case (int)ast_app_layer::CommandType::TEMPERATURE:
                case (int)ast_app_layer::CommandType::METER_BUS_ADDR:
                case (int)ast_app_layer::CommandType::GET_OCMF_REVERSE:
                case (int)ast_app_layer::CommandType::GET_PUBKEY_BIN:
                case (int)ast_app_layer::CommandType::GET_TRANSACT_IMPORT_LINE_LOSS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TRANSACT_EXPORT_LINE_LOSS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TRANSACT_TOTAL_IMPORT_DEV_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TRANSACT_TOTAL_EXPORT_DEV_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TRANSACT_TOTAL_IMPORT_MAINS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TRANSACT_TOTAL_EXPORT_MAINS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TOTAL_START_IMPORT_LINE_LOSS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TOTAL_START_EXPORT_LINE_LOSS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TOTAL_START_IMPORT_MAINS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TOTAL_START_EXPORT_MAINS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TOTAL_STOP_IMPORT_LINE_LOSS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TOTAL_STOP_EXPORT_LINE_LOSS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TOTAL_STOP_IMPORT_MAINS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TOTAL_STOP_EXPORT_MAINS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_LOG_ENTRY:
                case (int)ast_app_layer::CommandType::GET_LOG_ENTRY_REVERSE:
                case (int)ast_app_layer::CommandType::REGISTER_DISPLAY_PUBKEY:
                case (int)ast_app_layer::CommandType::REQUEST_CHALLENGE:
                case (int)ast_app_layer::CommandType::SET_SIGNED_CHALLENGE:
                case (int)ast_app_layer::CommandType::GET_TOTAL_IMPORT_MAINS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TOTAL_EXPORT_MAINS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TOTAL_IMPORT_MAINS_POWER:
                case (int)ast_app_layer::CommandType::GET_TOTAL_EXPORT_MAINS_POWER:
                case (int)ast_app_layer::CommandType::GET_DEV_VOLTAGE_L1:
                case (int)ast_app_layer::CommandType::GET_IMPORT_LINE_LOSS_POWER:
                case (int)ast_app_layer::CommandType::GET_EXPORT_LINE_LOSS_POWER:
                case (int)ast_app_layer::CommandType::GET_TOTAL_IMPORT_LINE_LOSS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_TOTAL_EXPORT_LINE_LOSS_ENERGY:
                case (int)ast_app_layer::CommandType::GET_SECOND_INDEX:
                case (int)ast_app_layer::CommandType::GET_PUBKEY_STR32:
                case (int)ast_app_layer::CommandType::GET_PUBKEY_CSTR16:
                case (int)ast_app_layer::CommandType::GET_PUBKEY_CSTR32:
                case (int)ast_app_layer::CommandType::REPEAT_DATA:
                case (int)ast_app_layer::CommandType::AB_DMC:
                case (int)ast_app_layer::CommandType::AB_PROD_DATE:
                case (int)ast_app_layer::CommandType::SET_REQUEST_CHALLENGE:
                    {
                        EVLOG_error << "Command not (yet) implemented. (" << module::conversions::hexdump(part_cmd) << ")";
                    }
                    break;

                default:
                    {
                        EVLOG_error << "Command ID invalid. (" << module::conversions::hexdump(part_cmd) << ")";
                    }
                    break;
            }

        } else {
            EVLOG_error << "Error: Malformed data.";
        }
        i += part_len;
    }

    // publish powermeter values
    this->publish_powermeter(this->pm_last_values);

    return response_status;
}

ast_app_layer::CommandResult powermeterImpl::receive_response() {
    ast_app_layer::CommandResult retval = ast_app_layer::CommandResult::OK;
    std::vector<uint8_t> response{};
    response.reserve(ast_app_layer::PM_AST_MAX_RX_LENGTH);
    this->serial_device.rx(response, ast_app_layer::PM_AST_SERIAL_RX_INITIAL_TIMEOUT_MS, ast_app_layer::PM_AST_SERIAL_RX_WITHIN_MESSAGE_TIMEOUT_MS);

    EVLOG_critical << "\n\nRECEIVE: " << module::conversions::hexdump(response) << " length: " << response.size() << "\n\n";
    
    if (response.size() >= 5) {
        ast_app_layer::CommandResult result{};
        this->slip.unpack(response, config.powermeter_device_id);
        while (this->slip.get_message_counter() > 0) {
            std::vector<uint8_t> message_from_queue = std::move(this->slip.retrieve_single_message());
            result = process_response(message_from_queue);
            if (result != ast_app_layer::CommandResult::OK) {  // always report (at least one) error instead of OK, if available
                retval = result;
            }
        }
    } else {
        EVLOG_info << "Received partial message. Skipping. [" << module::conversions::hexdump(response) << "]";
        return ast_app_layer::CommandResult::COMMUNICATION_FAILED;
    }
    return retval;
}

// ############################################################################################################################################
// ############################################################################################################################################
// ############################################################################################################################################

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    types::powermeter::TransactionStartResponse r;
    this->start_transact_result = ast_app_layer::CommandResult::PENDING;
    ast_app_layer::UserIdStatus user_id_status = ast_app_layer::UserIdStatus::USER_NOT_ASSIGNED;
    if (value.transaction_assigned_to_user) {
        user_id_status = ast_app_layer::UserIdStatus::USER_ASSIGNED;
    }

    ast_app_layer::UserIdType user_id_type = ast_app_layer::UserIdType::NONE;
    if (value.user_identification_type.has_value()) {
        user_id_type = ast_app_layer::user_id_type_conversion_everest_to_ast(value.user_identification_type.value());
    }

    std::string user_id_data = "unidentified_user";
    if (value.user_identification_name.has_value()) {
        user_id_data = value.user_identification_name.value();
    }
    
    std::vector<uint8_t> data_vect{};
    app_layer.create_command_start_transaction(user_id_status, user_id_type, user_id_data, this->config.gmt_offset_quarter_hours, data_vect);
    std::vector<uint8_t> slip_msg_start_transaction = std::move(this->slip.package_single(this->config.powermeter_device_id, data_vect));
    this->serial_device.tx(slip_msg_start_transaction);
    this->start_transaction_msg_status = MessageStatus::SENT;
    Timeout timeout(TIMEOUT_2s);
    while (this->start_transaction_msg_status != MessageStatus::RECEIVED) {
        receive_response();
        if(timeout.reached()) {
            this->stop_transact_result = ast_app_layer::CommandResult::TIMEOUT;
            break;
        }
    }

    if (this->start_transact_result != ast_app_layer::CommandResult::OK) {
        r.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
    } else {
        r.status = types::powermeter::TransactionRequestStatus::OK;
    }
    return r;
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    types::powermeter::TransactionStopResponse r;
    this->stop_transact_result = ast_app_layer::CommandResult::PENDING;
    std::vector<uint8_t> data_vect{};
    app_layer.create_command_stop_transaction(data_vect);
    std::vector<uint8_t> slip_msg_stop_transaction = std::move(this->slip.package_single(this->config.powermeter_device_id, data_vect));
    this->serial_device.tx(slip_msg_stop_transaction);
    this->stop_transaction_msg_status = MessageStatus::SENT;
    Timeout timeout(TIMEOUT_2s);
    while (this->stop_transaction_msg_status != MessageStatus::RECEIVED) {
        receive_response();
        if(timeout.reached()) {
            this->stop_transact_result = ast_app_layer::CommandResult::TIMEOUT;
            break;
        }
    }

    if (this->stop_transact_result != ast_app_layer::CommandResult::OK) {
        r.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
    } else {
        std::string ocmf_result = get_meter_ocmf();
        std::string error_substring{"Error: command failed with code "};
        if (ocmf_result.find(error_substring) != std::string::npos) {
            r.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
        } else {
            r.status = types::powermeter::TransactionRequestStatus::OK;
            r.ocmf = ocmf_result;
        }
    }
    return r;
}

std::string powermeterImpl::get_meter_ocmf() {
    std::vector<uint8_t> data_vect{};
    app_layer.create_command_get_last_transaction_ocmf(data_vect);
    std::vector<uint8_t> slip_msg_get_last_ocmf = std::move(this->slip.package_single(this->config.powermeter_device_id, data_vect));
    this->serial_device.tx(slip_msg_get_last_ocmf);
    this->get_transaction_values_msg_status = MessageStatus::SENT;
    Timeout timeout(TIMEOUT_2s);
    while (this->get_transaction_values_msg_status != MessageStatus::RECEIVED) {
        receive_response();
        if(timeout.reached()) {
            this->stop_transact_result = ast_app_layer::CommandResult::TIMEOUT;
            break;
        }
    }

    if (this->stop_transact_result != ast_app_layer::CommandResult::OK) {
        std::stringstream ss;
        ss << "Error: command failed with code " << int(this->stop_transact_result) << ". (" << command_result_to_string(this->stop_transact_result) << ")";
        return ss.str();
    }

    return this->last_ocmf_str;
}

} // namespace main
} // namespace module
