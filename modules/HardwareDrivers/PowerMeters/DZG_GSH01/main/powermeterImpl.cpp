// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"

#include <fmt/core.h>

namespace module {
namespace main {

inline std::uint16_t get_u16(const std::vector<std::uint8_t>& vec, std::uint8_t start_index) {
    if (vec.size() < start_index + 1)
        return 0;
    return (static_cast<std::uint16_t>(vec[start_index + 1]) << 8) | static_cast<std::uint16_t>(vec[start_index]);
}

inline std::uint16_t get_u16(const std::vector<std::uint8_t>& vec) {
    return get_u16(vec, 0);
}

inline std::uint32_t get_u32(const std::vector<std::uint8_t>& vec, std::uint8_t start_index) {
    if (vec.size() < start_index + 3)
        return 0;
    return (static_cast<std::uint32_t>(vec[start_index + 3]) << 24) |
           (static_cast<std::uint32_t>(vec[start_index + 2]) << 16) |
           (static_cast<std::uint32_t>(vec[start_index + 1]) << 8) | static_cast<std::uint32_t>(vec[start_index]);
}

inline std::uint32_t get_u32(const std::vector<std::uint8_t>& vec) {
    return get_u32(vec, 0);
}

inline std::uint64_t get_u64(const std::vector<std::uint8_t>& vec, std::uint8_t start_index) {
    if (vec.size() < start_index + 7)
        return 0;
    return (static_cast<std::uint64_t>(vec[start_index + 7]) << 56) |
           (static_cast<std::uint64_t>(vec[start_index + 6]) << 48) |
           (static_cast<std::uint64_t>(vec[start_index + 5]) << 40) |
           (static_cast<std::uint64_t>(vec[start_index + 4]) << 32) |
           (static_cast<std::uint64_t>(vec[start_index + 3]) << 24) |
           (static_cast<std::uint64_t>(vec[start_index + 2]) << 16) |
           (static_cast<std::uint64_t>(vec[start_index + 1]) << 8) | static_cast<std::uint64_t>(vec[start_index]);
}

inline std::uint64_t get_u64(const std::vector<std::uint8_t>& vec) {
    return get_u64(vec, 0);
}

inline std::string get_str(const std::vector<std::uint8_t>& vec, std::uint16_t start_index, std::uint16_t length) {
    std::string str = "";
    if ((start_index + length) <= vec.size()) {
        for (std::uint16_t n = start_index; n < (start_index + length); n++) {
            if (vec[n] == 0x00)
                break;
            str += vec[n];
        }
    }
    return str;
}

void powermeterImpl::init() {
    init_default_values();
}

void powermeterImpl::ready() {
    bool connected = false;
    while (!connected) {
        connected =
            serial_device.open_device(config.serial_port, config.baudrate, config.ignore_echo, config.num_of_retries);
        if (!connected &&
            !(error_state_monitor->is_error_active("powermeter/CommunicationFault", "Communication timed out"))) {
            communication_timeout = true;
            EVLOG_error << "Failed to communicate with the powermeter due to serial port : " << config.serial_port
                        << " errors.";
            auto error = error_factory->create_error("powermeter/CommunicationFault", "Communication timed out",
                                                     "This error is raised due to communication timeout");
            raise_error(error);
        } else if (!connected) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    EVLOG_info << "Connected to the serial port : " << config.serial_port;
    communication_timeout = false;

    // do not clear the error yet, it will be cleared once we send / receive messages from the device
    get_status_word();
    // we mark that we will need to clear a dangling transaction before starting a new one
    // we either get a stop transaction (and we clear this bool too if successful) or in the
    // worst case we clear the transaction next time we get a start transaction command
    const std::uint64_t CHARGING_STATUS = 0x20000; // according to documentation it is the bit 17th
    need_to_stop_transaction = (device_data_obj.ab_status & CHARGING_STATUS) != 0;
    if (need_to_stop_transaction) {
        EVLOG_warning << "There is already a running transaction on the powermeter.";
    }
    time_sync();

    std::thread([this] {
        while (true) {
            read_powermeter_values();
            // publish powermeter values
            publish_powermeter(pm_last_values);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }).detach();

    // create device_data publisher thread
    if (config.publish_device_data) {
        std::thread([this] {
            while (true) {
                get_status_word();
                if (device_data_obj.ab_status & CHARGING_STATUS) {
                    no_charging_done = false;
                    charging_in_progress = true;
                } else {
                    charging_in_progress = false;
                }
                read_device_data();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                publish_device_data_topic();
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }).detach();
    }

    // create device_diagnostics publisher thread
    if (config.publish_device_diagnostics) {
        std::thread([this] {
            while (true) {
                read_diagnostics_data();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                publish_device_diagnostics_topic();
                std::this_thread::sleep_for(std::chrono::seconds(10));
            }
        }).detach();
    }

    // create logging publisher thread
    if (config.publish_device_diagnostics) {
        std::thread([this] {
            while (true) {
                publish_logging_topic();
                std::this_thread::sleep_for(std::chrono::seconds(20));
            }
        }).detach();
    }

    // create error diagnostics thread
    if (config.publish_device_diagnostics) {
        std::thread([this] {
            while (true) {
                if (error_diagnostics_target != 0) {
                    error_diagnostics(error_diagnostics_target);
                    error_diagnostics_target = 0;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
        }).detach();
    }
}
void powermeterImpl::time_sync() {
    get_device_time();

    std::chrono::time_point<std::chrono::system_clock> timepoint = std::chrono::system_clock::now();
    int8_t gmt_offset_quarters_of_an_hour = app_layer.get_utc_offset_in_quarter_hours(timepoint);

    std::uint64_t diff = 0;
    const auto time_since_epoch_count = static_cast<std::uint32_t>(
        std::chrono::duration_cast<std::chrono::seconds>(timepoint.time_since_epoch()).count());
    if (device_data_obj.utc_time_s < time_since_epoch_count) {
        diff = time_since_epoch_count - device_data_obj.utc_time_s;
    } else {
        diff = device_data_obj.utc_time_s - time_since_epoch_count;
    }
    if (diff > config.max_clock_diff_s) {
        EVLOG_info << "time diff is: " << diff << " s";
        EVLOG_info << "clock is out of sync --> time is set";
        set_device_time();
    }
}

void powermeterImpl::get_device_time() {
    std::vector<std::uint8_t> data_vect{};
    app_layer.create_command_get_time(data_vect);
    auto slip_msg = slip.package_single(config.powermeter_device_id, data_vect);
    send_receive(slip_msg);
}

void powermeterImpl::set_device_time() {
    std::chrono::time_point<std::chrono::system_clock> timepoint = std::chrono::system_clock::now();
    int8_t gmt_offset_quarters_of_an_hour = app_layer.get_utc_offset_in_quarter_hours(timepoint);

    std::vector<std::uint8_t> set_device_time_cmd{};
    app_layer.create_command_set_time(date::utc_clock::from_sys(timepoint), gmt_offset_quarters_of_an_hour,
                                      set_device_time_cmd);

    auto slip_msg = slip.package_single(config.powermeter_device_id, set_device_time_cmd);
    send_receive(slip_msg);
}

void powermeterImpl::get_meter_bus_address() {
    std::vector<std::uint8_t> data_vect{};
    app_layer.create_command_get_bus_address(data_vect);
    auto slip_msg = slip.package_single(0xFF, data_vect);
    send_receive(slip_msg);
}

void powermeterImpl::set_meter_bus_address(std::uint8_t old_bus_address, std::uint8_t new_bus_address) {
    std::vector<std::uint8_t> set_meter_bus_address_cmd{};
    app_layer.create_command_set_bus_address(new_bus_address, set_meter_bus_address_cmd);

    auto slip_msg = slip.package_single(old_bus_address, set_meter_bus_address_cmd);
    send_receive(slip_msg);
}

void powermeterImpl::get_status_word() {
    std::vector<std::uint8_t> data_vect{};
    app_layer.create_command_get_status_word(data_vect);
    auto slip_msg = slip.package_single(config.powermeter_device_id, data_vect);
    send_receive(slip_msg);
}

void powermeterImpl::publish_device_data_topic() {
    json j;
    to_json(j, device_data_obj);
    std::string dev_data_topic = pm_last_values.meter_id.value() + std::string("/device_data");
    mod->mqtt.publish(dev_data_topic, j.dump());
}

void powermeterImpl::publish_device_diagnostics_topic() {
    json j;
    to_json(j, device_diagnostics_obj);
    std::string dev_diagnostics_topic = pm_last_values.meter_id.value() + std::string("/device_diagnostics");
    mod->mqtt.publish(dev_diagnostics_topic, j.dump());
}

void powermeterImpl::publish_logging_topic() {
    json j;
    to_json(j, logging_obj);
    std::string logging_topic = pm_last_values.meter_id.value() + std::string("/logging");
    mod->mqtt.publish(logging_topic, j.dump());
}

void powermeterImpl::read_device_data() {
    std::vector<std::uint8_t> get_time_cmd{};
    app_layer.create_command_get_time(get_time_cmd);

    std::vector<std::uint8_t> get_total_start_import_energy_cmd{};
    app_layer.create_command_get_total_start_import_energy(get_total_start_import_energy_cmd);

    // This value is not available while charging
    std::vector<std::uint8_t> get_total_stop_import_energy_cmd{};
    app_layer.create_command_get_total_stop_import_energy(get_total_stop_import_energy_cmd);

    std::vector<std::uint8_t> get_total_transaction_duration_cmd{};
    app_layer.create_command_get_total_transaction_duration(get_total_transaction_duration_cmd);

    std::vector<std::uint8_t> get_ocmf_stats_cmd{};
    app_layer.create_command_get_ocmf_stats(get_ocmf_stats_cmd);

    std::vector<std::uint8_t> get_last_transaction_ocmf_cmd{};
    app_layer.create_command_get_last_transaction_ocmf(get_last_transaction_ocmf_cmd);

    std::vector<std::uint8_t> slip_msg;
    if (no_charging_done) {
        slip_msg = slip.package_multi(config.powermeter_device_id, {get_time_cmd,
                                                                    // get_total_start_import_energy_cmd,
                                                                    // get_total_stop_import_energy_cmd,
                                                                    // get_total_transaction_duration_cmd,
                                                                    get_ocmf_stats_cmd, get_last_transaction_ocmf_cmd});
    } else if (charging_in_progress) {
        slip_msg = slip.package_multi(config.powermeter_device_id, {get_time_cmd, get_total_start_import_energy_cmd,
                                                                    // get_total_stop_import_energy_cmd,
                                                                    get_total_transaction_duration_cmd,
                                                                    get_ocmf_stats_cmd, get_last_transaction_ocmf_cmd});
    } else {
        slip_msg =
            slip.package_multi(config.powermeter_device_id,
                               {get_time_cmd, get_total_start_import_energy_cmd, get_total_stop_import_energy_cmd,
                                get_total_transaction_duration_cmd, get_ocmf_stats_cmd, get_last_transaction_ocmf_cmd});
    }
    send_receive(slip_msg);
}

void powermeterImpl::get_device_public_key() {
    std::vector<std::uint8_t> get_device_public_key_asn1_cmd{};
    app_layer.create_command_get_pubkey_asn1(get_device_public_key_asn1_cmd);

    std::vector<std::uint8_t> get_device_public_key_str16_cmd{};
    app_layer.create_command_get_pubkey_str16(get_device_public_key_str16_cmd);

    auto slip_msg = slip.package_multi(config.powermeter_device_id,
                                       {get_device_public_key_asn1_cmd, get_device_public_key_str16_cmd});
    send_receive(slip_msg);
}

void powermeterImpl::request_device_type() {
    std::vector<std::uint8_t> data_vect{};
    app_layer.create_command_get_device_type(data_vect);
    auto slip_msg = slip.package_single(config.powermeter_device_id, data_vect);
    send_receive(slip_msg);
}

void powermeterImpl::get_app_fw_version() {
    std::vector<std::uint8_t> data_vect{};
    app_layer.create_command_get_application_fw_version(data_vect);
    auto slip_msg = slip.package_single(config.powermeter_device_id, data_vect);
    send_receive(slip_msg);
}

void powermeterImpl::get_application_operation_mode() {
    std::vector<std::uint8_t> data_vect{};
    app_layer.create_command_get_application_mode(data_vect);
    auto slip_msg = slip.package_single(config.powermeter_device_id, data_vect);
    send_receive(slip_msg);
}
void powermeterImpl::set_application_operation_mode(app_layer::ApplicationBoardMode mode) {
    std::vector<std::uint8_t> set_operation_mode_cmd{};
    app_layer.create_command_set_application_mode(mode, set_operation_mode_cmd);
    auto slip_msg = slip.package_single(config.powermeter_device_id, set_operation_mode_cmd);
    send_receive(slip_msg);
}

void powermeterImpl::get_line_loss_impedance() {
    std::vector<std::uint8_t> data_vect{};
    app_layer.create_command_get_line_loss_impedance(data_vect);
    auto slip_msg = slip.package_single(config.powermeter_device_id, data_vect);
    send_receive(slip_msg);
}
void powermeterImpl::set_line_loss_impedance(std::uint16_t ll_impedance) {
    std::vector<std::uint8_t> set_ll_impedance_cmd{};
    app_layer.create_command_set_line_loss_impedance(ll_impedance, set_ll_impedance_cmd);
    auto slip_msg = slip.package_single(config.powermeter_device_id, set_ll_impedance_cmd);
    send_receive(slip_msg);
}

void powermeterImpl::request_error_diagnostics(std::uint8_t addr) {
    error_diagnostics_target = addr;
}

/* retrieve last error objects from device */
void powermeterImpl::error_diagnostics(std::uint8_t addr) {
    std::vector<std::uint8_t> last_log_entry_cmd{};
    app_layer.create_command_get_last_log_entry(last_log_entry_cmd);
    auto slip_msg = slip.package_single(config.powermeter_device_id, last_log_entry_cmd);
    send_receive(slip_msg);
}

void powermeterImpl::read_diagnostics_data() {
    // part 1 - basic info
    {
        std::vector<std::uint8_t> get_charge_point_id_cmd{};
        app_layer.create_command_get_charge_point_id(get_charge_point_id_cmd);

        std::vector<std::uint8_t> get_server_id_cmd{};
        app_layer.create_command_get_server_id(get_server_id_cmd);

        std::vector<std::uint8_t> get_serial_number_cmd{};
        app_layer.create_command_get_serial_number(get_serial_number_cmd);

        std::vector<std::uint8_t> get_hardware_version_cmd{};
        app_layer.create_command_get_hardware_version(get_hardware_version_cmd);

        std::vector<std::uint8_t> get_device_type_cmd{};
        app_layer.create_command_get_device_type(get_device_type_cmd);

        std::vector<std::uint8_t> get_bootloader_version_cmd{};
        app_layer.create_command_get_bootloader_version(get_bootloader_version_cmd);

        auto slip_msg = slip.package_multi(config.powermeter_device_id,
                                           {get_charge_point_id_cmd, get_server_id_cmd, get_serial_number_cmd,
                                            get_hardware_version_cmd, get_device_type_cmd, get_bootloader_version_cmd});
        send_receive(slip_msg);
    }

    // part 2 - log stats
    {
        std::vector<std::uint8_t> get_log_stats_cmd{};
        app_layer.create_command_get_log_stats(get_log_stats_cmd);

        auto slip_msg = slip.package_single(config.powermeter_device_id, get_log_stats_cmd);
        send_receive(slip_msg);
    }

    // part 3 - application/metering info
    {
        std::vector<std::uint8_t> get_application_fw_version_cmd{};
        app_layer.create_command_get_application_fw_version(get_application_fw_version_cmd);

        std::vector<std::uint8_t> get_application_fw_checksum_cmd{};
        app_layer.create_command_get_application_fw_checksum(get_application_fw_checksum_cmd);

        std::vector<std::uint8_t> get_application_fw_hash_cmd{};
        app_layer.create_command_get_application_fw_hash(get_application_fw_hash_cmd);

        std::vector<std::uint8_t> get_application_mode_cmd{};
        app_layer.create_command_get_application_mode(get_application_mode_cmd);

        std::vector<std::uint8_t> get_metering_fw_version_cmd{};
        app_layer.create_command_get_metering_fw_version(get_metering_fw_version_cmd);

        std::vector<std::uint8_t> get_metering_fw_checksum_cmd{};
        app_layer.create_command_get_metering_fw_checksum(get_metering_fw_checksum_cmd);

        std::vector<std::uint8_t> get_metering_mode_cmd{};
        app_layer.create_command_get_metering_mode(get_metering_mode_cmd);

        auto slip_msg =
            slip.package_multi(config.powermeter_device_id,
                               {get_application_fw_version_cmd, get_application_fw_checksum_cmd,
                                get_application_fw_hash_cmd, get_application_mode_cmd, get_metering_fw_version_cmd,
                                get_metering_fw_checksum_cmd, get_metering_mode_cmd});
        send_receive(slip_msg);
    }

    // part 4 - public key
    get_device_public_key();
}

void powermeterImpl::read_powermeter_values() {
    readRegisters();
    pm_last_values.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
}

void powermeterImpl::init_default_values() {
    pm_last_values.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    pm_last_values.meter_id = "GSH01_Powermeter_addr_" + std::to_string(config.powermeter_device_id);

    types::units::Energy import_device_energy_Wh;
    import_device_energy_Wh.total = 0.0f;
    pm_last_values.energy_Wh_import = import_device_energy_Wh;
    types::units::Power import_device_power_W;
    import_device_power_W.total = 0.0f;
    pm_last_values.power_W = import_device_power_W;

    types::units::Voltage voltage_V;
    voltage_V.DC = 0.0f;
    pm_last_values.voltage_V = voltage_V;

    types::units::Current current_A;
    current_A.DC = 0.0f;
    pm_last_values.current_A = current_A;
}

void powermeterImpl::readRegisters() {
    std::vector<std::uint8_t> get_time_cmd{};
    app_layer.create_command_get_time(get_time_cmd);

    std::vector<std::uint8_t> get_voltage_cmd{};
    app_layer.create_command_get_voltage(get_voltage_cmd);

    std::vector<std::uint8_t> get_current_cmd{};
    app_layer.create_command_get_current(get_current_cmd);

    std::vector<std::uint8_t> get_import_power_cmd{};
    app_layer.create_command_get_import_power(get_import_power_cmd);

    std::vector<std::uint8_t> get_import_energy_cmd{};
    app_layer.create_command_get_total_dev_import_energy(get_import_energy_cmd);

    // ToDo other instanceous registers: f.i. GET_TOTAL_IMPORT_MAINS_ENERGY

    auto slip_msg = slip.package_multi(config.powermeter_device_id, {get_time_cmd, get_voltage_cmd, get_current_cmd,
                                                                     get_import_power_cmd, get_import_energy_cmd});
    send_receive(slip_msg);
}

// ############################################################################################################################################
// ############################################################################################################################################

app_layer::CommandResult powermeterImpl::process_response(const std::vector<std::uint8_t>& response_message) {
    app_layer::CommandResult response_status{app_layer::CommandResult::OK};
    size_t response_size = response_message.size();

    // split into multiple command responses
    std::uint8_t dest_addr = response_message.at(0);
    std::uint16_t i = 1;
    while ((i + 4) <= response_size) {
        std::uint16_t part_cmd = (static_cast<std::uint16_t>(response_message.at(i + 1)) << 8) | response_message.at(i);
        std::uint16_t part_len =
            (static_cast<std::uint16_t>(response_message.at(i + 3)) << 8) | response_message.at(i + 2);
        std::uint16_t part_data_len = part_len - 5;
        app_layer::CommandResult part_status = static_cast<app_layer::CommandResult>(response_message.at(i + 4));
        if (response_status ==
            app_layer::CommandResult::OK) { // only update response status if not already error present
            response_status = part_status;
        }

        if ((i + part_len - 1) <= response_size) {
            std::vector<std::uint8_t> part_data((response_message.begin() + i + 5),
                                                (response_message.begin() + i + part_len));

            if (part_status != app_layer::CommandResult::OK) {
                EVLOG_error << "Powermeter at address " << int(dest_addr) << " ("
                            << module::conversions::hexdump(dest_addr) << ")"
                            << " has signaled an error (status: (" << (int)part_status << ") \""
                            << app_layer::command_result_to_string(part_status) << "\") at response "
                            << module::conversions::hexdump(part_cmd) << " !";
                {
                    EVLOG_info << "Retrieving diagnostics data for error at command "
                               << module::conversions::hexdump(part_cmd) << "...";
                    request_error_diagnostics(dest_addr);
                    i += part_len; // skip remaining data and go to next command in message
                    continue;
                }
            }

            // process response
            switch (part_cmd) {

                // operational values

            case (int)app_layer::CommandType::START_TRANSACTION: {
                start_transaction_msg_status = MessageStatus::RECEIVED;
                start_transact_result = part_status;
            } break;

            case (int)app_layer::CommandType::STOP_TRANSACTION: {
                stop_transaction_msg_status = MessageStatus::RECEIVED;
                stop_transact_result = part_status;
            } break;

            case (int)app_layer::CommandType::TIME: {
                if (part_data_len < 5)
                    break;
                device_data_obj.utc_time_s = get_u32(part_data);
                device_data_obj.gmt_offset_quarterhours = part_data[4];
            } break;

            case (int)app_layer::CommandType::GET_VOLTAGE_L1: {
                if (part_data_len < 4)
                    break;
                types::units::Voltage volt = pm_last_values.voltage_V.value();
                volt.DC = (float)get_u32(part_data) / 100.0; // powermeter reports in 100 * [V]
                pm_last_values.voltage_V = volt;
            } break;

            case (int)app_layer::CommandType::GET_CURRENT_L1: {
                if (part_data_len < 4)
                    break;
                types::units::Current amp = pm_last_values.current_A.value();
                amp.DC = (float)get_u32(part_data) / 1000.0; // powermeter reports in [mA]
                pm_last_values.current_A = amp;
            } break;

            case (int)app_layer::CommandType::GET_IMPORT_DEV_POWER: {
                if (part_data_len < 4)
                    break;
                types::units::Power power = pm_last_values.power_W.value();
                power.total = (float)get_u32(part_data) / 100.0; // powermeter reports in [W * 100]
                pm_last_values.power_W = power;
            } break;

            case (int)app_layer::CommandType::GET_TOTAL_DEV_POWER: {
                EVLOG_info << "(GET_TOTAL_DEV_POWER) Not yet implemented. ["
                           << (float)get_u32(part_data) / 100.0 /* powermeter reports in [W * 100] */ << " W]";
            } break;

            case (int)app_layer::CommandType::GET_TOTAL_IMPORT_DEV_ENERGY: {
                if (part_data_len < 8)
                    break;
                types::units::Energy energy_in = pm_last_values.energy_Wh_import;
                energy_in.total = (float)get_u64(part_data) / 10.0; // powermeter reports in [Wh * 10]
                pm_last_values.energy_Wh_import = energy_in;

                // device_data_obj.total_dev_import_energy_Wh = get_u64(part_data) / 10.0;
                // powermeter reports in [Wh * 10]
            } break;

            case (int)app_layer::CommandType::GET_TOTAL_START_IMPORT_DEV_ENERGY: {
                if (part_data_len < 8)
                    break;
                device_data_obj.total_start_import_energy_Wh =
                    get_u64(part_data) / 10.0; // powermeter reports in [Wh * 10]
            } break;

            case (int)app_layer::CommandType::GET_TOTAL_STOP_IMPORT_DEV_ENERGY: {
                if (part_data_len < 8)
                    break;
                device_data_obj.total_stop_import_energy_Wh =
                    get_u64(part_data) / 10.0; // powermeter reports in [Wh * 10]
            } break;

            case (int)app_layer::CommandType::GET_TRANSACT_TOTAL_DURATION: {
                if (part_data_len < 4)
                    break;
                device_data_obj.total_transaction_duration_s = get_u32(part_data);
            } break;

            case (int)app_layer::CommandType::GET_PUBKEY_STR16: {
                if (part_data_len < 130)
                    break;
                device_diagnostics_obj.pubkey_str16_format = part_data[0];
                device_diagnostics_obj.pubkey_str16 = module::conversions::hexdump(part_data, 1, 129);
            } break;

            case (int)app_layer::CommandType::GET_PUBKEY_ASN1: {
                if (part_data_len < 176)
                    break;
                device_diagnostics_obj.pubkey_asn1 = module::conversions::hexdump(part_data, 0, 176);
            } break;

                // diagnostics

            case (int)app_layer::CommandType::OCMF_STATS: {
                if (part_data_len < 16)
                    break;
                device_data_obj.ocmf_stats.number_transactions = get_u32(part_data);
                device_data_obj.ocmf_stats.timestamp_first_transaction = get_u32(part_data, 4);
                device_data_obj.ocmf_stats.timestamp_last_transaction = get_u32(part_data, 8);
                device_data_obj.ocmf_stats.max_number_of_transactions = get_u32(part_data, 12);
            } break;

            case (int)app_layer::CommandType::GET_OCMF: {
                if (part_data_len < 16)
                    break;
                device_data_obj.requested_ocmf = get_str(part_data, 0, part_data_len);
            } break;

            case (int)app_layer::CommandType::GET_LAST_OCMF: {
                if (get_transaction_values_msg_status == MessageStatus::SENT) {
                    get_transaction_values_msg_status = MessageStatus::RECEIVED;
                }
                if (part_status == app_layer::CommandResult::OK) {
                    device_data_obj.last_ocmf_transaction = get_str(part_data, 0, part_data_len);
                    last_ocmf_str = device_data_obj.last_ocmf_transaction;
                } else {
                    last_ocmf_str = "";
                }
            } break;

            case (int)app_layer::CommandType::CHARGE_POINT_ID: {
                if (part_data_len < 3)
                    break;
                device_diagnostics_obj.charge_point_id_type = part_data[0];
                device_diagnostics_obj.charge_point_id = get_str(part_data, 1, part_data_len);
            } break;

            case (int)app_layer::CommandType::GET_LOG_STATS: {
                if (part_data_len < 16)
                    break;
                device_diagnostics_obj.log_stats.number_log_entries = get_u32(part_data);
                device_diagnostics_obj.log_stats.timestamp_first_log = get_u32(part_data, 4);
                device_diagnostics_obj.log_stats.timestamp_last_log = get_u32(part_data, 8);
                device_diagnostics_obj.log_stats.max_number_of_logs = get_u32(part_data, 12);
            } break;

            case (int)app_layer::CommandType::GET_LAST_LOG_ENTRY: {
                if (part_data_len < 86)
                    break;
                logging_obj.last_log.type = static_cast<app_layer::LogType>(part_data[0]);
                logging_obj.last_log.second_index = get_u32(part_data, 1);
                logging_obj.last_log.utc_time = get_u32(part_data, 5);
                logging_obj.last_log.utc_offset = part_data[9];
                std::uint16_t value_len = (part_data_len - 84) / 2;
                logging_obj.last_log.old_value.clear();
                for (std::uint8_t n = 10; n < 10 + value_len; n++) {
                    logging_obj.last_log.old_value.push_back(part_data[n]);
                }
                logging_obj.last_log.new_value.clear();
                for (std::uint8_t n = 10 + value_len; n < 10 + value_len * 2; n++) {
                    logging_obj.last_log.new_value.push_back(part_data[n]);
                }
                logging_obj.last_log.server_id.clear();
                for (std::uint8_t n = part_data_len - 74; n < part_data_len - 64; n++) {
                    logging_obj.last_log.server_id.push_back(part_data[n]);
                }
                logging_obj.last_log.signature.clear();
                for (std::uint8_t n = part_data_len - 64; n < part_data_len; n++) {
                    logging_obj.last_log.signature.push_back(part_data[n]);
                }
            } break;

                // device parameters

            case (int)app_layer::CommandType::APP_MODE: {
                if (part_data_len < 1)
                    break;
                std::uint8_t mode = part_data[0];
                if (mode == static_cast<std::uint8_t>(app_layer::ApplicationBoardMode::ASSEMBLY)) {
                    device_diagnostics_obj.dev_info.application.mode = "Assembly";
                } else if (mode == static_cast<std::uint8_t>(app_layer::ApplicationBoardMode::APPLICATION)) {
                    device_diagnostics_obj.dev_info.application.mode = "Application";
                } else {
                    // EVLOG_info << "Operation mode is unknown";
                    device_diagnostics_obj.dev_info.application.mode = "Unknown";
                }
            } break;

            case (int)app_layer::CommandType::LINE_LOSS_IMPEDANCE: {
                if (part_data_len < 1)
                    break;
            } break;

            case (int)app_layer::CommandType::METER_BUS_ADDR: {
                if (part_data_len < 1)
                    break;
                device_diagnostics_obj.dev_info.bus_address = part_data[0];
            } break;

            case (int)app_layer::CommandType::HW_VERSION: {
                std::uint8_t delimiter_pos = 0;
                for (std::uint8_t i = 0; i < part_data_len; i++) {
                    if (part_data[i] == '|') {
                        delimiter_pos = i;
                    }
                }
                device_diagnostics_obj.dev_info.hw_ver =
                    get_str(part_data, delimiter_pos + 1, (part_data_len - delimiter_pos - 1));
            } break;

            case (int)app_layer::CommandType::SERVER_ID: {
                if (part_data_len < 10)
                    break;
                device_diagnostics_obj.dev_info.server_id = get_str(part_data, 0, 10);
            } break;

            case (int)app_layer::CommandType::SERIAL_NR: {
                if (part_data_len < 4)
                    break;
                device_diagnostics_obj.dev_info.serial_number = get_u32(part_data);
            } break;

            case (int)app_layer::CommandType::APP_FW_VERSION: {
                if (part_data_len < 1)
                    break;
                device_diagnostics_obj.dev_info.application.fw_ver = get_str(part_data, 0, part_data_len);
            } break;

            case (int)app_layer::CommandType::APP_FW_CHECKSUM: {
                if (part_data_len < 2)
                    break;
                device_diagnostics_obj.dev_info.application.fw_crc = get_u16(part_data);
            } break;

            case (int)app_layer::CommandType::APP_FW_HASH: {
                if (part_data_len < 2)
                    break;
                device_diagnostics_obj.dev_info.application.fw_hash = get_u16(part_data);
            } break;

            case (int)app_layer::CommandType::MT_FW_VERSION: {
                if (part_data_len < 1)
                    break;
                device_diagnostics_obj.dev_info.metering.fw_ver = get_str(part_data, 0, part_data_len);
            } break;

            case (int)app_layer::CommandType::MT_FW_CHECKSUM: {
                if (part_data_len < 2)
                    break;
                device_diagnostics_obj.dev_info.metering.fw_crc = get_u16(part_data);
            } break;

            case (int)app_layer::CommandType::MT_MODE: {
                if (part_data_len < 1)
                    break;
                device_diagnostics_obj.dev_info.metering.mode = part_data[0];
            } break;

            case (int)app_layer::CommandType::BOOTL_VERSION: {
                if (part_data_len < 1)
                    break;
                device_diagnostics_obj.dev_info.bootl_ver = get_str(part_data, 0, part_data_len);
            } break;

            case (int)app_layer::CommandType::DEVICE_TYPE: {
                if (part_data_len < 1)
                    break;
                device_diagnostics_obj.dev_info.type = get_str(part_data, 0, part_data_len);
            } break;

            case (int)app_layer::CommandType::STATUS_WORD: {
                if (part_data_len < 8)
                    break;
                device_data_obj.ab_status = get_u64(part_data);
                // app_layer::StatusWord::print(device_data_obj.ab_status);
            } break;

                // not (yet) implemented

            case (int)app_layer::CommandType::TRANSPARENT_MODE:
            case (int)app_layer::CommandType::MEASUREMENT_MODE:
            case (int)app_layer::CommandType::GET_NORMAL_VOLTAGE:
            case (int)app_layer::CommandType::GET_NORMAL_CURRENT:
            case (int)app_layer::CommandType::GET_MAX_CURRENT:
            case (int)app_layer::CommandType::REVERSE_MODE:
            case (int)app_layer::CommandType::CLEAR_METER_STATUS:
            case (int)app_layer::CommandType::INIT_METER:
            case (int)app_layer::CommandType::LINE_LOSS_MEAS_MODE:
            case (int)app_layer::CommandType::APP_CONFIG_COMPLETE:
            case (int)app_layer::CommandType::AS_CONFIG_COMPLETE:
            case (int)app_layer::CommandType::GET_OCMF_REVERSE:
            case (int)app_layer::CommandType::GET_TRANSACT_IMPORT_LINE_LOSS_ENERGY:
            case (int)app_layer::CommandType::GET_TRANSACT_TOTAL_IMPORT_DEV_ENERGY:
            case (int)app_layer::CommandType::GET_TRANSACT_TOTAL_IMPORT_MAINS_ENERGY:
            case (int)app_layer::CommandType::GET_TOTAL_START_IMPORT_LINE_LOSS_ENERGY:
            case (int)app_layer::CommandType::GET_TOTAL_START_IMPORT_MAINS_ENERGY:
            case (int)app_layer::CommandType::GET_TOTAL_STOP_IMPORT_LINE_LOSS_ENERGY:
            case (int)app_layer::CommandType::GET_TOTAL_STOP_IMPORT_MAINS_ENERGY:
            case (int)app_layer::CommandType::GET_LOG_ENTRY:
            case (int)app_layer::CommandType::GET_LOG_ENTRY_REVERSE:
            case (int)app_layer::CommandType::GET_TOTAL_IMPORT_MAINS_ENERGY:
            case (int)app_layer::CommandType::GET_TOTAL_IMPORT_MAINS_POWER:
            case (int)app_layer::CommandType::GET_POS_DEV_VOLTAGE:
            case (int)app_layer::CommandType::GET_NEG_DEV_VOLTAGE:
            case (int)app_layer::CommandType::GET_TOTAL_VOLTAGE:
            case (int)app_layer::CommandType::GET_IMPORT_LINE_LOSS_POWER:
            case (int)app_layer::CommandType::GET_TOTAL_IMPORT_LINE_LOSS_ENERGY:
            case (int)app_layer::CommandType::GET_SECOND_INDEX:
            case (int)app_layer::CommandType::GET_PUBKEY_STR32:
            case (int)app_layer::CommandType::GET_PUBKEY_CSTR16:
            case (int)app_layer::CommandType::GET_PUBKEY_CSTR32: {
                EVLOG_error << "Command not (yet) implemented. (" << module::conversions::hexdump(part_cmd) << ")";
            } break;

            default: {
                EVLOG_error << "Command ID invalid. (" << module::conversions::hexdump(part_cmd) << ")";
            } break;
            }

        } else {
            EVLOG_error << "Error: Malformed data.";
        }
        i += part_len;
    }

    return response_status;
}

app_layer::CommandResult powermeterImpl::handle_response(std::vector<std::uint8_t>& response) {
    app_layer::CommandResult retval = app_layer::CommandResult::OK;

    if (response.size() >= 5) {
        app_layer::CommandResult result{};
        auto ret = slip.unpack(response, config.powermeter_device_id);
        if (ret != slip_protocol::SlipReturnStatus::SLIP_OK) {
            EVLOG_info << "SLIP_error = " << (int)ret;
            return app_layer::CommandResult::FORMAT_INVALID;
        }
        while (slip.get_message_counter() > 0) {
            std::vector<std::uint8_t> message_from_queue = slip.retrieve_single_message();
            result = process_response(message_from_queue);
            if (result !=
                app_layer::CommandResult::OK) { // always report (at least one) error instead of OK, if available
                retval = result;
            }
        }
    } else if (response.size() == 0) {
        return app_layer::CommandResult::TIMEOUT;
    } else {
        EVLOG_info << "Received partial message. Skipping. [" << module::conversions::hexdump(response) << "]";
        return app_layer::CommandResult::COMMUNICATION_FAILED;
    }
    return retval;
}

void powermeterImpl::send_receive(std::vector<std::uint8_t>& request) {
    app_layer::CommandResult result = app_layer::CommandResult::TIMEOUT;
    while (app_layer::CommandResult::TIMEOUT == result) {
        std::vector<std::uint8_t> response{};
        response.reserve(app_layer::PM_GSH01_MAX_RX_LENGTH);
        serial_device.tx_rx_blocking(request, response, app_layer::PM_GSH01_SERIAL_RX_INITIAL_TIMEOUT_MS,
                                     app_layer::PM_GSH01_SERIAL_RX_WITHIN_MESSAGE_TIMEOUT_MS);
        result = handle_response(response);
        if (app_layer::CommandResult::TIMEOUT == result) {
            if (!error_state_monitor->is_error_active("powermeter/CommunicationFault", "Communication timed out")) {
                communication_timeout = true;
                EVLOG_error << "Failed to communicate with the powermeter with the id : "
                            << config.powermeter_device_id;
                auto error = error_factory->create_error("powermeter/CommunicationFault", "Communication timed out",
                                                         "This error is raised due to communication timeout");
                raise_error(error);
            }
        } else {
            if (error_state_monitor->is_error_active("powermeter/CommunicationFault", "Communication timed out")) {
                communication_timeout = false;
                clear_error("powermeter/CommunicationFault", "Communication timed out");
            }
        }
    }
}

// ############################################################################################################################################
// ############################################################################################################################################
// ############################################################################################################################################

static app_layer::UserIdType ocmf_to_appidtype(types::powermeter::OCMFIdentificationType t) {
    using everest_id = types::powermeter::OCMFIdentificationType;
    using app_id = app_layer::UserIdType;
    switch (t) {
    case everest_id::NONE:
        return app_id::NONE;
    case everest_id::DENIED:
        return app_id::DENIED;
    case everest_id::UNDEFINED:
        return app_id::UNDEFINED;
    case everest_id::ISO14443:
        return app_id::ISO14443;
    case everest_id::ISO15693:
        return app_id::ISO15693;
    case everest_id::EMAID:
        return app_id::EMAID;
    case everest_id::EVCCID:
        return app_id::EVCCID;
    case everest_id::EVCOID:
        return app_id::EVCOID;
    case everest_id::ISO7812:
        return app_id::ISO7812;
    case everest_id::CARD_TXN_NR:
        return app_id::CAR_TXN_NR;
    case everest_id::CENTRAL:
        return app_id::CENTRAL;
    case everest_id::CENTRAL_1:
        return app_id::CENTRAL_1;
    case everest_id::CENTRAL_2:
        return app_id::CENTRAL_2;
    case everest_id::LOCAL:
        return app_id::LOCAL;
    case everest_id::LOCAL_1:
        return app_id::LOCAL_1;
    case everest_id::LOCAL_2:
        return app_id::LOCAL_2;
    case everest_id::PHONE_NUMBER:
        return app_id::PHONE_NUMBER;
    case everest_id::KEY_CODE:
        return app_id::KEY_CODE;
    }
    return app_id::NONE;
}

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    types::powermeter::TransactionStartResponse r;
    if (false == communication_timeout) {
        time_sync();
        cleanup_dangling_transaction();

        start_transact_result = app_layer::CommandResult::PENDING;

        app_layer::UserIdStatus user_id_status = app_layer::UserIdStatus::USER_NOT_ASSIGNED;
        app_layer::UserIdType user_id_type = app_layer::UserIdType::NONE;

        if (value.identification_status == types::powermeter::OCMFUserIdentificationStatus::ASSIGNED) {
            user_id_status = app_layer::UserIdStatus::USER_ASSIGNED;
        }

        user_id_type = ocmf_to_appidtype(value.identification_type);

        std::string user_id_data = value.identification_data.value_or("");

        std::vector<std::uint8_t> data_vect{};
        app_layer.create_command_start_transaction(user_id_status, user_id_type, user_id_data, data_vect);
        auto slip_msg = slip.package_single(config.powermeter_device_id, data_vect);

        start_transaction_msg_status = MessageStatus::SENT;
        send_receive(slip_msg);

        if (start_transaction_msg_status != MessageStatus::RECEIVED) {
            start_transact_result = app_layer::CommandResult::TIMEOUT;
        }

        if (start_transact_result != app_layer::CommandResult::OK) {
            r.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
        } else {
            r.status = types::powermeter::TransactionRequestStatus::OK;
            // need to mark that the transaction needs to be stopped since the device
            // is not failing start transaction if the transaction is already started
            // it will be just ignored
            need_to_stop_transaction = true;
        }
    } else {
        r.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
    }
    return r;
}

types::powermeter::TransactionStopResponse powermeterImpl::do_stop_transaction(const std::string& transaction_id) {
    types::powermeter::TransactionStopResponse r;
    stop_transact_result = app_layer::CommandResult::PENDING;

    std::vector<std::uint8_t> data_vect{};
    app_layer.create_command_stop_transaction(data_vect);
    auto slip_msg = slip.package_single(config.powermeter_device_id, data_vect);

    stop_transaction_msg_status = MessageStatus::SENT;
    send_receive(slip_msg);

    if (stop_transaction_msg_status != MessageStatus::RECEIVED) {
        stop_transact_result = app_layer::CommandResult::TIMEOUT;
    }

    if (stop_transact_result != app_layer::CommandResult::OK) {
        r.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
    } else {
        std::string ocmf_result = get_meter_ocmf();
        std::string error_substring{"Error: command failed with code "};
        if (ocmf_result.find(error_substring) != std::string::npos) {
            r.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
        } else {
            r.status = types::powermeter::TransactionRequestStatus::OK;
            r.signed_meter_value = types::units_signed::SignedMeterValue{ocmf_result, "", "OCMF"};
        }
    }
    return r;
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    types::powermeter::TransactionStopResponse r;
    if (false == communication_timeout) {
        auto result = do_stop_transaction(transaction_id);
        if (types::powermeter::TransactionRequestStatus::OK == result.status) {
            need_to_stop_transaction = false;
        }
        return result;
    } else {
        r.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
        need_to_stop_transaction = true;
    }
    return r;
}

void powermeterImpl::cleanup_dangling_transaction(void) {
    if (need_to_stop_transaction) {
        EVLOG_warning << "Clean up the running transaction on the powermeter.";
        auto result = do_stop_transaction("");
        if (types::powermeter::TransactionRequestStatus::OK == result.status) {
            need_to_stop_transaction = false;
        }
    }
}

std::string powermeterImpl::get_meter_ocmf() {
    std::vector<std::uint8_t> data_vect{};
    app_layer.create_command_get_last_transaction_ocmf(data_vect);
    auto slip_msg = slip.package_single(config.powermeter_device_id, data_vect);

    get_transaction_values_msg_status = MessageStatus::SENT;
    send_receive(slip_msg);

    if (get_transaction_values_msg_status != MessageStatus::RECEIVED) {
        stop_transact_result = app_layer::CommandResult::TIMEOUT;
    }

    if (stop_transact_result != app_layer::CommandResult::OK) {
        std::stringstream ss;
        ss << "Error: command failed with code " << int(stop_transact_result) << ". ("
           << command_result_to_string(stop_transact_result) << ")";
        return ss.str();
    }

    return last_ocmf_str;
}

} // namespace main
} // namespace module
