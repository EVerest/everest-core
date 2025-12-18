// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest

#include "serial_communication_hubImpl.hpp"

#include <chrono>
#include <cstdint>
#include <date/date.h>
#include <date/tz.h>
#include <fmt/core.h>
#include <mutex>
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

/**
 * @brief Converts a Result to a ResultBool by looking at each bit of the uint16_t values and converting them to
 * bools in the right order. Used for Modbus read coils responses where the result is a bit-packed array of coil states.
 * @param result The Result to convert
 * @param number_of_coils The number of coils that were requested to read, used to limit the number of bools in the
 * output
 * @return The converted ResultBool
 */
static types::serial_comm_hub_requests::ResultBool
convert_read_coils_result(const types::serial_comm_hub_requests::Result& result, size_t number_of_coils) {
    constexpr uint8_t BITS_PER_BYTE = 8;
    constexpr uint16_t BYTE_MASK = 0xFF;

    types::serial_comm_hub_requests::ResultBool out;
    out.status_code = result.status_code;

    if (result.value.has_value()) {
        std::vector<bool> result_bool;
        for (const uint16_t packed_bytes : result.value.value()) {
            // Modbus read coils response packs bits into raw bytes, the modbus library uses big-endian to build uint16
            // from those. Here we extract the original MSB and LSB from the BE uint16_t and process them in the correct
            // order.
            const auto msb = static_cast<uint8_t>((packed_bytes >> BITS_PER_BYTE) & BYTE_MASK);
            const auto lsb = static_cast<uint8_t>(packed_bytes & BYTE_MASK);

            for (const uint8_t byte : {msb, lsb}) {
                for (int bit = 0; bit < BITS_PER_BYTE; bit++) {
                    if (result_bool.size() >= number_of_coils) {
                        break;
                    }
                    result_bool.push_back((byte & (1U << bit)) != 0);
                }
            }
        }

        out.value = std::move(result_bool);
    }

    return out;
}

// Implementation

void serial_communication_hubImpl::init() {
    using namespace std::chrono;
    Everest::GpioSettings rxtx_gpio_settings;

    rxtx_gpio_settings.chip_name = config.rxtx_gpio_chip;
    rxtx_gpio_settings.line_number = config.rxtx_gpio_line;
    rxtx_gpio_settings.inverted = config.rxtx_gpio_tx_high;

    system_error_logged = false;

    if (!modbus.open_device(config.serial_port, config.baudrate, config.ignore_echo, rxtx_gpio_settings,
                            static_cast<tiny_modbus::Parity>(config.parity), config.rtscts,
                            milliseconds(config.initial_timeout_ms), milliseconds(config.within_message_timeout_ms))) {
        EVLOG_error << fmt::format("Cannot open serial port {}, ModBus will not work.", config.serial_port);
    }
}

void serial_communication_hubImpl::ready() {
}

types::serial_comm_hub_requests::Result
serial_communication_hubImpl::perform_modbus_request(uint8_t device_address, tiny_modbus::FunctionCode function,
                                                     uint16_t first_register_address, uint16_t register_quantity,
                                                     bool wait_for_reply, std::vector<uint16_t> request) {
    std::scoped_lock lock(serial_mutex);
    types::serial_comm_hub_requests::Result result;
    std::vector<uint16_t> response;
    auto retry_counter = config.retries + 1;
    bool last_error_was_timeout = false;

    while (retry_counter > 0) {
        auto current_trial = config.retries + 1 - retry_counter + 1;

        EVLOG_debug << fmt::format("Trial {}/{}: calling {}(id {} addr {}({:#06x}) len {})", current_trial,
                                   config.retries + 1, tiny_modbus::FunctionCode_to_string_with_hex(function),
                                   device_address, first_register_address, first_register_address, register_quantity);

        last_error_was_timeout = false;
        try {
            response = modbus.txrx(device_address, function, first_register_address, register_quantity,
                                   config.max_packet_size, wait_for_reply, request);
        } catch (const tiny_modbus::TimeoutException& e) {
            // TimeoutException is a specific type of communication error
            last_error_was_timeout = true;
            auto logmsg = fmt::format("Modbus call {} for device id {} addr {}({:#06x}) failed: {}",
                                      tiny_modbus::FunctionCode_to_string_with_hex(function), device_address,
                                      first_register_address, first_register_address, e.what());

            if (retry_counter != 1)
                EVLOG_debug << logmsg;
            else
                EVLOG_warning << logmsg;
        } catch (const tiny_modbus::TinyModbusException& e) {
            auto logmsg = fmt::format("Modbus call {} for device id {} addr {}({:#06x}) failed: {}",
                                      tiny_modbus::FunctionCode_to_string_with_hex(function), device_address,
                                      first_register_address, first_register_address, e.what());

            if (retry_counter != 1)
                EVLOG_debug << logmsg;
            else
                EVLOG_warning << logmsg;
        } catch (const std::logic_error& e) {
            EVLOG_warning << "Logic error in Modbus implementation: " << e.what();
        } catch (const std::system_error& e) {
            // FIXME: report this to the infrastructure, as soon as an error interface for this is available
            // Log this only once, as we are convinced this will not go away
            if (not system_error_logged) {
                EVLOG_error << "System error in accessing Modbus: [" << e.code() << "] " << e.what();
                system_error_logged = true;
            }
        }

        if (response.size() > 0)
            break;

        retry_counter--;
    }

    if (response.size() > 0) {
        EVLOG_debug << fmt::format("Process response (size {})", response.size());
        result.status_code = types::serial_comm_hub_requests::StatusCodeEnum::Success;
        result.value = vector_to_int(response);
        system_error_logged = false; // reset after success
    } else {
        // If the last error was a timeout, return Timeout status, otherwise Error
        if (last_error_was_timeout) {
            result.status_code = types::serial_comm_hub_requests::StatusCodeEnum::Timeout;
        } else {
            result.status_code = types::serial_comm_hub_requests::StatusCodeEnum::Error;
        }
    }
    return result;
}

// Commands

types::serial_comm_hub_requests::Result
serial_communication_hubImpl::handle_modbus_read_holding_registers(int& target_device_id, int& first_register_address,
                                                                   int& num_registers_to_read) {

    return perform_modbus_request(target_device_id, tiny_modbus::FunctionCode::READ_MULTIPLE_HOLDING_REGISTERS,
                                  first_register_address, num_registers_to_read);
}

types::serial_comm_hub_requests::Result
serial_communication_hubImpl::handle_modbus_read_input_registers(int& target_device_id, int& first_register_address,
                                                                 int& num_registers_to_read) {

    return perform_modbus_request(target_device_id, tiny_modbus::FunctionCode::READ_INPUT_REGISTERS,
                                  first_register_address, num_registers_to_read);
}

types::serial_comm_hub_requests::StatusCodeEnum serial_communication_hubImpl::handle_modbus_write_multiple_registers(
    int& target_device_id, int& first_register_address, types::serial_comm_hub_requests::VectorUint16& data_raw) {

    types::serial_comm_hub_requests::Result result;
    std::vector<uint16_t> data;
    append_array<uint16_t, int>(data, data_raw.data);

    result = perform_modbus_request(target_device_id, tiny_modbus::FunctionCode::WRITE_MULTIPLE_HOLDING_REGISTERS,
                                    first_register_address, data.size(), true, data);

    return result.status_code;
}

types::serial_comm_hub_requests::StatusCodeEnum
serial_communication_hubImpl::handle_modbus_write_single_register(int& target_device_id, int& register_address,
                                                                  int& data) {
    types::serial_comm_hub_requests::Result result;

    result = perform_modbus_request(target_device_id, tiny_modbus::FunctionCode::WRITE_SINGLE_HOLDING_REGISTER,
                                    register_address, 1, true, {static_cast<uint16_t>(data)});

    return result.status_code;
}

types::serial_comm_hub_requests::StatusCodeEnum
serial_communication_hubImpl::handle_modbus_write_single_coil(int& target_device_id, int& coil_address, bool& data) {
    types::serial_comm_hub_requests::Result result;

    result = perform_modbus_request(target_device_id, tiny_modbus::FunctionCode::WRITE_SINGLE_COIL, coil_address, 1,
                                    true, {static_cast<uint16_t>(data ? 0xFF00 : 0x0000)});

    return result.status_code;
}

types::serial_comm_hub_requests::ResultBool
serial_communication_hubImpl::handle_modbus_read_coils(int& target_device_id, int& first_coil_address,
                                                       int& num_coils_to_read) {
    const auto result = perform_modbus_request(target_device_id, tiny_modbus::FunctionCode::READ_COILS,
                                               first_coil_address, num_coils_to_read);

    return convert_read_coils_result(result, num_coils_to_read);
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
