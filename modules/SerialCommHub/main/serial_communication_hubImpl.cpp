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
    using namespace std::chrono;
    Everest::GpioSettings rxtx_gpio_settings;

    rxtx_gpio_settings.chip_name = config.rxtx_gpio_chip;
    rxtx_gpio_settings.line_number = config.rxtx_gpio_line;
    rxtx_gpio_settings.inverted = config.rxtx_gpio_tx_high;

    if (!modbus.open_device(config.serial_port, config.baudrate, config.ignore_echo, rxtx_gpio_settings,
                            static_cast<tiny_modbus::Parity>(config.parity), config.rtscts,
                            milliseconds(config.initial_timeout_ms), milliseconds(config.within_message_timeout_ms))) {
        EVLOG_error << "Cannot open serial port {}, ModBus will not work.", config.serial_port;
    }
}

void serial_communication_hubImpl::ready() {
}

// Commands

types::serial_comm_hub_requests::Result
serial_communication_hubImpl::handle_modbus_read_holding_registers(int& target_device_id, int& first_register_address,
                                                                   int& num_registers_to_read) {

    types::serial_comm_hub_requests::Result result;
    std::vector<uint16_t> response;

    {
        std::scoped_lock lock(serial_mutex);

        auto retry_counter = this->num_resends_on_error;
        while (retry_counter > 0) {

            EVLOG_debug << fmt::format("Try {} Call modbus_client->read_holding_register(id {} addr {} len {})",
                                       (int)retry_counter, (uint8_t)target_device_id, (uint16_t)first_register_address,
                                       (uint16_t)num_registers_to_read);

            response = modbus.txrx(target_device_id, tiny_modbus::FunctionCode::READ_MULTIPLE_HOLDING_REGISTERS,
                                   first_register_address, num_registers_to_read, config.max_packet_size);
            if (response.size() > 0) {
                break;
            }
            retry_counter--;
        }
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

types::serial_comm_hub_requests::Result
serial_communication_hubImpl::handle_modbus_read_input_registers(int& target_device_id, int& first_register_address,
                                                                 int& num_registers_to_read) {
    types::serial_comm_hub_requests::Result result;
    std::vector<uint16_t> response;

    {
        std::scoped_lock lock(serial_mutex);

        uint8_t retry_counter{this->num_resends_on_error};
        while (retry_counter-- > 0) {

            EVLOG_debug << fmt::format("Try {} Call modbus_client->read_input_register(id {} addr {} len {})",
                                       (int)retry_counter, (uint8_t)target_device_id, (uint16_t)first_register_address,
                                       (uint16_t)num_registers_to_read);

            response = modbus.txrx(target_device_id, tiny_modbus::FunctionCode::READ_INPUT_REGISTERS,
                                   first_register_address, num_registers_to_read, config.max_packet_size);
            if (response.size() > 0) {
                break;
            }
        }
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
    int& target_device_id, int& first_register_address, types::serial_comm_hub_requests::VectorUint16& data_raw) {

    types::serial_comm_hub_requests::Result result;
    std::vector<uint16_t> response;

    std::vector<uint16_t> data;
    append_array<uint16_t, int>(data, data_raw.data);

    {
        std::scoped_lock lock(serial_mutex);

        uint8_t retry_counter{this->num_resends_on_error};
        while (retry_counter-- > 0) {

            EVLOG_debug << fmt::format("Try {} Call modbus_client->write_multiple_registers(id {} addr {} len {})",
                                       (int)retry_counter, (uint8_t)target_device_id, (uint16_t)first_register_address,
                                       (uint16_t)data.size());

            response = modbus.txrx(target_device_id, tiny_modbus::FunctionCode::WRITE_MULTIPLE_HOLDING_REGISTERS,
                                   first_register_address, data.size(), config.max_packet_size, true, data);
            if (response.size() > 0) {
                break;
            }
        }
    }

    EVLOG_debug << fmt::format("Done writing (size {})", response.size());
    // process response
    if (response.size() > 0) {
        return types::serial_comm_hub_requests::StatusCodeEnum::Success;
    } else {
        return types::serial_comm_hub_requests::StatusCodeEnum::Error;
    }
}

types::serial_comm_hub_requests::StatusCodeEnum
serial_communication_hubImpl::handle_modbus_write_single_register(int& target_device_id, int& register_address,
                                                                  int& data) {
    types::serial_comm_hub_requests::Result result;
    std::vector<uint16_t> response;

    {
        std::scoped_lock lock(serial_mutex);

        uint8_t retry_counter{this->num_resends_on_error};
        while (retry_counter-- > 0) {

            EVLOG_debug << fmt::format("Try {} Call modbus_client->write_single_register(id {} addr {} data 0x{:04x})",
                                       (int)retry_counter, (uint8_t)target_device_id, (uint16_t)register_address,
                                       (uint16_t)data);

            response = modbus.txrx(target_device_id, tiny_modbus::FunctionCode::WRITE_SINGLE_HOLDING_REGISTER,
                                   register_address, 1, config.max_packet_size, true, {static_cast<uint16_t>(data)});
            if (response.size() > 0) {
                break;
            }
        }
    }
    EVLOG_debug << fmt::format("Done writing (size {})", response.size());
    // process response
    if (response.size() > 0) {
        return types::serial_comm_hub_requests::StatusCodeEnum::Success;
    } else {
        return types::serial_comm_hub_requests::StatusCodeEnum::Error;
    }
}

void serial_communication_hubImpl::handle_nonstd_write(int& target_device_id, int& first_register_address,
                                                       int& num_registers_to_read) {
}

types::serial_comm_hub_requests::Result serial_communication_hubImpl::handle_nonstd_read(int& target_device_id,
                                                                                         int& first_register_address,
                                                                                         int& num_registers_to_read) {
    types::serial_comm_hub_requests::Result result;
    result.status_code = types::serial_comm_hub_requests::StatusCodeEnum::Error;
    return result;
}

} // namespace main
} // namespace module
