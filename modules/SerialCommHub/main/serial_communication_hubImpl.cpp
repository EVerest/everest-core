// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest

#include "serial_communication_hubImpl.hpp"

#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <fmt/core.h>
#include <typeinfo>

namespace module {
namespace main {

template <typename T, typename U> static void append_array(std::vector<T>& m, const std::vector<U>& a) {
    for (auto it = a.begin(); it != a.end(); ++it)
        m.push_back(*it);
}

// Helper functions

static std::vector<int> vector_to_int(const std::vector<uint16_t>& response) {
    std::vector<int> i;
    i.reserve(response.size());
    for (auto r : response) {
        i.push_back((int)r);
    }
    return i;
}

// Implementation

void serial_communication_hubImpl::init() {
    last_message_end_time = std::chrono::steady_clock::now();

    if (!modbus.openDevice(config.serial_port, config.baudrate)) {
        EVLOG_AND_THROW(Everest::EverestConfigError(fmt::format("Cannot open serial port {}.", config.serial_port)));
    }
}

void serial_communication_hubImpl::ready() {
}

// Commands

types::serial_comm_hub_requests::Result serial_communication_hubImpl::handle_modbus_read_holding_registers(
    int& target_device_id, int& first_register_address, int& num_registers_to_read, int& pause_between_messages) {

    types::serial_comm_hub_requests::Result result;
    std::vector<uint16_t> response;

    {
        std::scoped_lock lock(serial_mutex);
        auto time_since_last_message = std::chrono::steady_clock::now() - last_message_end_time;
        auto pause_time = std::chrono::milliseconds(pause_between_messages);
        //if (time_since_last_message < pause_time)
        //    std::this_thread::sleep_for(pause_time - time_since_last_message);

        uint8_t retry_counter{this->num_resends_on_error};
        while (retry_counter-- > 0) {

            //EVLOG_info << fmt::format("Try {} Call modbus_client->read_holding_register(id {} addr {} len {})",
            //                          (int)retry_counter, (uint8_t)target_device_id, (uint16_t)first_register_address,
            //                          (uint16_t)num_registers_to_read);

            response = modbus.txrx(target_device_id, tinymod::function_code::read_multiple_holding_registers,
                                   first_register_address, num_registers_to_read);
            if (response.size() > 0) {
                break;
            }
        }

        last_message_end_time = std::chrono::steady_clock::now();
    }

    EVLOG_debug << fmt::format("Process response (size {})", response.size());
    // process response
    if (response.size() > 0) {
        result.status_code = types::serial_comm_hub_requests::StatusCodeEnum::Success;
        result.value = vector_to_int(response);
    } else {
        result.status_code = types::serial_comm_hub_requests::StatusCodeEnum::Error;
    }
    return result;
}

types::serial_comm_hub_requests::Result serial_communication_hubImpl::handle_modbus_read_input_registers(
    int& target_device_id, int& first_register_address, int& num_registers_to_read, int& pause_between_messages) {
    types::serial_comm_hub_requests::Result result;
    std::vector<uint16_t> response;

    {
        std::scoped_lock lock(serial_mutex);
        auto time_since_last_message = std::chrono::steady_clock::now() - last_message_end_time;
        auto pause_time = std::chrono::milliseconds(pause_between_messages);
        //if (time_since_last_message < pause_time)
        //    std::this_thread::sleep_for(pause_time - time_since_last_message);

        uint8_t retry_counter{this->num_resends_on_error};
        while (retry_counter-- > 0) {

            //EVLOG_info << fmt::format("Try {} Call modbus_client->read_input_register(id {} addr {} len {})",
            //                          (int)retry_counter, (uint8_t)target_device_id, (uint16_t)first_register_address,
            //                          (uint16_t)num_registers_to_read);

            response = modbus.txrx(target_device_id, tinymod::function_code::read_input_registers,
                                   first_register_address, num_registers_to_read);
            if (response.size() > 0) {
                break;
            }
        }

        last_message_end_time = std::chrono::steady_clock::now();
    }

    EVLOG_debug << fmt::format("Process response (size {})", response.size());
    // process response
    if (response.size() > 0) {
        result.status_code = types::serial_comm_hub_requests::StatusCodeEnum::Success;
        result.value = vector_to_int(response);
    } else {
        result.status_code = types::serial_comm_hub_requests::StatusCodeEnum::Error;
    }
    return result;
}

types::serial_comm_hub_requests::StatusCodeEnum serial_communication_hubImpl::handle_modbus_write_multiple_registers(
    int& target_device_id, int& first_register_address, types::serial_comm_hub_requests::VectorUint16& data_raw,
    int& pause_between_messages) {

    types::serial_comm_hub_requests::Result result;
    std::vector<uint16_t> response;

    std::vector<uint16_t> data;
    append_array<uint16_t, int>(data, data_raw.data);

    {
        std::scoped_lock lock(serial_mutex);

        auto time_since_last_message = std::chrono::steady_clock::now() - last_message_end_time;
        auto pause_time = std::chrono::milliseconds(pause_between_messages);
        //if (time_since_last_message < pause_time)
        //    std::this_thread::sleep_for(pause_time - time_since_last_message);

        uint8_t retry_counter{this->num_resends_on_error};
        while (retry_counter-- > 0) {

            //EVLOG_info << fmt::format("Try {} Call modbus_client->write_multiple_registers(id {} addr {} len {})",
            //                          (int)retry_counter, (uint8_t)target_device_id, (uint16_t)first_register_address,
            //                          (uint16_t)data.size());

            response = modbus.txrx(target_device_id, tinymod::function_code::write_multiple_holding_registers,
                                   first_register_address, data.size(), true, data);
            if (response.size() > 0) {
                break;
            }
        }

        last_message_end_time = std::chrono::steady_clock::now();
    }

    EVLOG_debug << fmt::format("Done writing");
    // process response
    if (response.size() > 0) {
        return types::serial_comm_hub_requests::StatusCodeEnum::Success;
    } else {
        return types::serial_comm_hub_requests::StatusCodeEnum::Error;
    }
}

void serial_communication_hubImpl::handle_nonstd_write(int& target_device_id, int& first_register_address,
                                                       int& num_registers_to_read, int& pause_between_messages) {
}

types::serial_comm_hub_requests::Result serial_communication_hubImpl::handle_nonstd_read(int& target_device_id,
                                                                                         int& first_register_address,
                                                                                         int& num_registers_to_read,
                                                                                         int& pause_between_messages) {
    types::serial_comm_hub_requests::Result result;
    result.status_code = types::serial_comm_hub_requests::StatusCodeEnum::Error;
    return result;
}

} // namespace main
} // namespace module
