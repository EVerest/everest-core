// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ast_app_layer.hpp"

#include <cstring>
#include <endian.h>
#include <errno.h>
#include <everest/logging.hpp>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include <sys/select.h>
#include <sys/time.h>

#include <fmt/core.h>

namespace ast_app_layer {

uint32_t timepoint_to_uint32(date::utc_clock::time_point timepoint) {
    return static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::seconds>(timepoint.time_since_epoch()).count());
}

void insert_u16_as_u8s(std::vector<std::uint8_t>& vec, std::uint16_t u16) {
    std::uint8_t upper = static_cast<std::uint8_t>((u16 >> 8) & 0x00FF);
    std::uint8_t lower = static_cast<std::uint8_t>(u16 & 0x00FF);
    vec.push_back(lower);
    vec.push_back(upper);
}

void insert_u32_as_u8s(std::vector<std::uint8_t>& vec, std::uint32_t u32) {
    vec.push_back(static_cast<std::uint8_t>(u32 & 0x000000FF));
    vec.push_back(static_cast<std::uint8_t>((u32 >> 8) & 0x000000FF));
    vec.push_back(static_cast<std::uint8_t>((u32 >> 16) & 0x000000FF));
    vec.push_back(static_cast<std::uint8_t>((u32 >> 24) & 0x000000FF));
}

std::vector<std::uint8_t> AstAppLayer::create_command(ast_app_layer::Command cmd) {
    std::vector<std::uint8_t> command_data{};

    insert_u16_as_u8s(command_data, static_cast<std::uint16_t>(cmd.type));
    insert_u16_as_u8s(command_data, static_cast<std::uint16_t>(cmd.length));
    command_data.push_back((uint8_t)ast_app_layer::CommandStatus::OK);

    for (uint16_t i = 0; i < cmd.data.size(); i++) {
        command_data.push_back(cmd.data[i]);
    }
    return command_data;
}

std::vector<std::uint8_t> AstAppLayer::create_simple_command(ast_app_layer::CommandType cmd_type) {
    ast_app_layer::Command cmd{};

    cmd.type = cmd_type;
    cmd.length = 0x0005;
    cmd.status = ast_app_layer::CommandStatus::OK;

    return create_command(cmd);
}

int8_t AstAppLayer::get_utc_offset_in_quarter_hours(
    const std::chrono::time_point<std::chrono::system_clock>& timepoint_system_clock) {
    std::stringstream offset;
    std::int8_t offset_quarterhours = 0;

    auto tm = std::chrono::system_clock::to_time_t(timepoint_system_clock);
    offset << std::put_time(std::localtime(&tm), "%z");
    int offset_int = std::stoi(offset.str());

    int offset_h = offset_int / 100;
    int offset_remaining = offset_int % 100; // in case of timezones that are not full-hour offsets of UTC
    if (offset_remaining != 0) {
        std::int8_t offset_remaining_extra_hour = offset_remaining / 60;
        if (offset_remaining_extra_hour != 0) {
            offset_quarterhours += offset_remaining_extra_hour * 4; // can be positive or negative
            offset_remaining -= offset_remaining_extra_hour * 4;
        }
        std::int8_t offset_remaining_quarterhours = offset_remaining / 15;
        offset_quarterhours += offset_remaining_quarterhours;
    }
    offset_quarterhours += offset_h * 4;

    return offset_quarterhours;
}

void AstAppLayer::create_command_start_transaction(ast_app_layer::UserIdStatus user_id_status,
                                                   ast_app_layer::UserIdType user_id_type,
                                                   const std::string& user_id_data,
                                                   std::int8_t gmt_offset_quarter_hours,
                                                   std::vector<std::uint8_t>& command_data) {
    ast_app_layer::Command cmd{};

    cmd.type = ast_app_layer::CommandType::START_TRANSACTION;
    cmd.length = 0x0034;
    cmd.status = ast_app_layer::CommandStatus::OK;

    insert_u32_as_u8s(cmd.data, (timepoint_to_uint32(date::utc_clock::now())));
    cmd.data.push_back(
        static_cast<std::uint8_t>(gmt_offset_quarter_hours)); // GMT offset in quarters of an hour, e.g. 0x08 = +2h
    cmd.data.push_back(static_cast<std::uint8_t>(user_id_status));
    cmd.data.push_back(static_cast<std::uint8_t>(user_id_type));

    std::uint8_t byte_count = 0;
    for (std::uint8_t databyte : user_id_data) { // push up to 40 characters of user id name into command
        cmd.data.push_back(databyte);
        byte_count++;
        if (byte_count >= 40)
            break;
    }
    while (byte_count < 40) { // fill remaining user id name characters with zeros
        cmd.data.push_back(0x00);
        byte_count++;
    }

    command_data = create_command(cmd);
}

void AstAppLayer::create_command_stop_transaction(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::STOP_TRANSACTION);
}

void AstAppLayer::create_command_get_time(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::TIME);
}

void AstAppLayer::create_command_set_time(date::utc_clock::time_point timepoint,
                                          std::int8_t gmt_offset_quarters_of_an_hour,
                                          std::vector<std::uint8_t>& command_data) {
    ast_app_layer::Command cmd{};

    cmd.type = ast_app_layer::CommandType::TIME;
    cmd.length = 0x000A;
    cmd.status = ast_app_layer::CommandStatus::OK;

    insert_u32_as_u8s(cmd.data, timepoint_to_uint32(timepoint));
    cmd.data.push_back(gmt_offset_quarters_of_an_hour);

    command_data = create_command(cmd);
}

void AstAppLayer::create_command_get_voltage(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_VOLTAGE_L1);
}

void AstAppLayer::create_command_get_current(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_CURRENT_L1);
}

void AstAppLayer::create_command_get_import_power(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_IMPORT_DEV_POWER);
}

void AstAppLayer::create_command_get_export_power(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_EXPORT_DEV_POWER);
}

void AstAppLayer::create_command_get_total_dev_import_energy(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_TOTAL_IMPORT_DEV_ENERGY);
}

void AstAppLayer::create_command_get_total_dev_export_energy(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_TOTAL_EXPORT_DEV_ENERGY);
}

void AstAppLayer::create_command_get_total_power(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_TOTAL_DEV_POWER);
}

void AstAppLayer::create_command_get_total_start_import_energy(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_TOTAL_START_IMPORT_DEV_ENERGY);
}

void AstAppLayer::create_command_get_total_stop_import_energy(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_TOTAL_STOP_IMPORT_DEV_ENERGY);
}

void AstAppLayer::create_command_get_total_start_export_energy(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_TOTAL_START_EXPORT_DEV_ENERGY);
}

void AstAppLayer::create_command_get_total_stop_export_energy(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_TOTAL_STOP_EXPORT_DEV_ENERGY);
}

void AstAppLayer::create_command_get_total_transaction_duration(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_TRANSACT_TOTAL_DURATION);
}

void AstAppLayer::create_command_get_pubkey_str16(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_PUBKEY_STR16);
}

void AstAppLayer::create_command_get_pubkey_asn1(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_PUBKEY_ASN1);
}

void AstAppLayer::create_command_get_meter_pubkey(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::REQUEST_METER_PUBKEY);
}

void AstAppLayer::create_command_get_ocmf_stats(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::OCMF_STATS);
}

/* OCMF ID: 1..235000
    OCMF data from specified transaction will be at minimum import energy of transaction
*/
void AstAppLayer::create_command_get_transaction_ocmf(uint32_t ocmf_id, std::vector<std::uint8_t>& command_data) {
    ast_app_layer::Command cmd{};

    cmd.type = ast_app_layer::CommandType::GET_OCMF;
    cmd.length = 0x0009;
    cmd.status = ast_app_layer::CommandStatus::OK;

    insert_u32_as_u8s(cmd.data, ocmf_id);

    command_data = create_command(cmd);
}

void AstAppLayer::create_command_get_last_transaction_ocmf(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_LAST_OCMF);
}

void AstAppLayer::create_command_get_ocmf_info(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::OCMF_INFO);
}

void AstAppLayer::create_command_get_ocmf_config(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::OCMF_CONFIG);
}

void AstAppLayer::create_command_get_charge_point_id(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::CHARGE_POINT_ID);
}

/* only works in "assembly mode" */
void AstAppLayer::create_command_set_charge_point_id(ast_app_layer::UserIdType id_type, std::string id_data,
                                                     std::vector<std::uint8_t>& command_data) {
    ast_app_layer::Command cmd{};

    cmd.type = ast_app_layer::CommandType::CHARGE_POINT_ID;
    cmd.length = 0x0013;
    cmd.status = ast_app_layer::CommandStatus::OK;

    cmd.data.push_back((uint8_t)id_type);

    std::uint8_t byte_count = 0;
    for (std::uint8_t databyte : id_data) { // push up to 13 characters of id data into command
        cmd.data.push_back(databyte);
        byte_count++;
        if (byte_count >= 13)
            break;
    }
    while (byte_count < 13) { // fill remaining user id name characters with zeros
        cmd.data.push_back(0x00);
        byte_count++;
    }

    command_data = create_command(cmd);
}

void AstAppLayer::create_command_get_errors(ast_app_layer::ErrorCategory category, ast_app_layer::ErrorSource src,
                                            std::vector<std::uint8_t>& command_data) {
    ast_app_layer::Command cmd{};

    cmd.type = ast_app_layer::CommandType::GET_ERRORS;
    cmd.length = 0x0007;
    cmd.status = ast_app_layer::CommandStatus::OK;

    cmd.data.push_back((uint8_t)category);
    cmd.data.push_back((uint8_t)src);

    command_data = create_command(cmd);
}

void AstAppLayer::create_command_get_log_stats(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_LOG_STATS);
}

/* log entry ids: 1..2500 */
void AstAppLayer::create_command_get_log_entry(uint32_t log_entry_id, std::vector<std::uint8_t>& command_data) {
    ast_app_layer::Command cmd{};

    cmd.type = ast_app_layer::CommandType::GET_LOG_ENTRY;
    cmd.length = 0x0009;
    cmd.status = ast_app_layer::CommandStatus::OK;

    insert_u32_as_u8s(cmd.data, log_entry_id);

    command_data = create_command(cmd);
}

void AstAppLayer::create_command_get_last_log_entry(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::GET_LAST_LOG_ENTRY);
}

/* log entry ids: 1..2500
   thus: if 20 log entries and log_entry_id == 2, then log entry 18 will be returned
*/
void AstAppLayer::create_command_get_log_entry_reverse(uint32_t log_entry_id, std::vector<std::uint8_t>& command_data) {
    ast_app_layer::Command cmd{};

    cmd.type = ast_app_layer::CommandType::GET_LOG_ENTRY_REVERSE;
    cmd.length = 0x0009;
    cmd.status = ast_app_layer::CommandStatus::OK;

    insert_u32_as_u8s(cmd.data, log_entry_id);

    command_data = create_command(cmd);
}

void AstAppLayer::create_command_get_application_board_mode(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::AB_MODE_SET);
}

void AstAppLayer::create_command_set_application_board_mode(ast_app_layer::ApplicationBoardMode mode,
                                                            std::vector<std::uint8_t>& command_data) {
    ast_app_layer::Command cmd{};

    cmd.type = ast_app_layer::CommandType::AB_MODE_SET;
    cmd.length = 0x0006;
    cmd.status = ast_app_layer::CommandStatus::OK;

    cmd.data.push_back((uint8_t)mode);

    command_data = create_command(cmd);
}

// diagnostics

void AstAppLayer::create_command_get_hardware_version(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::AB_HW_VERSION);
}

void AstAppLayer::create_command_get_application_board_server_id(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::AB_SERVER_ID);
}

void AstAppLayer::create_command_get_application_board_serial_number(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::AB_SERIAL_NR);
}

void AstAppLayer::create_command_get_application_board_software_version(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::AB_SW_VERSION);
}

void AstAppLayer::create_command_get_application_board_fw_checksum(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::AB_FW_CHECKSUM);
}

void AstAppLayer::create_command_get_application_board_fw_hash(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::AB_FW_HASH);
}

void AstAppLayer::create_command_get_application_board_status(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::AB_STATUS);
}

void AstAppLayer::create_command_get_metering_board_software_version(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::MB_SW_VERSION);
}

void AstAppLayer::create_command_get_metering_board_fw_checksum(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::MB_FW_CHECKSUM);
}

/* doubles as OCMF "meter model name" */
void AstAppLayer::create_command_get_device_type(std::vector<std::uint8_t>& command_data) {
    command_data = create_simple_command(ast_app_layer::CommandType::AB_DEVICE_TYPE);
}

} // namespace ast_app_layer
