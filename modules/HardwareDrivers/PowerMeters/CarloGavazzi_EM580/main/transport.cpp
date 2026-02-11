// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "transport.hpp"
#include <string>

// Modbus protocol limits:
// - Read Input Registers (0x04): typically max 125 registers per request
// - Write Multiple Registers (0x10): max 123 registers per request (PDU size
// limit)
constexpr std::uint16_t MAX_READ_REGISTERS_PER_MESSAGE = 125;
constexpr std::uint16_t MAX_WRITE_REGISTERS_PER_MESSAGE = 123;

namespace transport {

transport::DataVector SerialCommHubTransport::fetch(std::int32_t address, std::uint16_t register_count) {
    return retry_with_config([this, address, register_count]() {
        transport::DataVector response;
        response.reserve(static_cast<std::size_t>(register_count) * 2U); // this is a uint8_t vector

        std::uint16_t remaining_register_to_read{register_count};
        std::int32_t read_address{address - m_base_address};

        while (remaining_register_to_read > 0) {
            const std::uint16_t register_to_read = remaining_register_to_read > MAX_READ_REGISTERS_PER_MESSAGE
                                                       ? MAX_READ_REGISTERS_PER_MESSAGE
                                                       : remaining_register_to_read;

            types::serial_comm_hub_requests::Result serial_com_hub_result =
                m_serial_hub.call_modbus_read_input_registers(static_cast<int>(m_device_id),
                                                              static_cast<int>(read_address), register_to_read);

            // Check for communication errors
            if (serial_com_hub_result.status_code == types::serial_comm_hub_requests::StatusCodeEnum::Timeout) {
                throw transport::ModbusTimeoutException("Modbus read timeout: Packet receive timeout");
            } else if (serial_com_hub_result.status_code != types::serial_comm_hub_requests::StatusCodeEnum::Success) {
                std::string error_msg =
                    "Modbus read failed with status: " +
                    types::serial_comm_hub_requests::status_code_enum_to_string(serial_com_hub_result.status_code);
                throw std::runtime_error(error_msg);
            }

            if (not serial_com_hub_result.value.has_value())
                throw std::runtime_error("no result from serial com hub!");

            // make sure that returned vector is a int32 vector
            static_assert(
                std::is_same_v<std::int32_t,
                               decltype(types::serial_comm_hub_requests::Result::value)::value_type::value_type>);

            union {
                std::int32_t val_32;
                struct {
                    std::uint8_t v3;
                    std::uint8_t v2;
                    std::uint8_t v1;
                    std::uint8_t v0;
                } val_8;
            } swapit;

            static_assert(sizeof(swapit.val_32) == sizeof(swapit.val_8));

            transport::DataVector tmp{};

            for (auto item : serial_com_hub_result.value.value()) {
                swapit.val_32 = item;
                tmp.push_back(swapit.val_8.v2);
                tmp.push_back(swapit.val_8.v3);
            }

            response.insert(response.end(), tmp.begin(), tmp.end());

            read_address += register_to_read;
            remaining_register_to_read -= register_to_read;
        }

        return response;
    });
}

void SerialCommHubTransport::write_multiple_registers(std::int32_t address, const std::vector<std::uint16_t>& data) {
    retry_with_config_void([this, address, &data]() {
        std::int32_t write_address = address - m_base_address;
        std::size_t offset = 0;
        while (offset < data.size()) {
            const std::size_t remaining = data.size() - offset;
            const std::size_t chunk_size = remaining > static_cast<std::size_t>(MAX_WRITE_REGISTERS_PER_MESSAGE)
                                               ? static_cast<std::size_t>(MAX_WRITE_REGISTERS_PER_MESSAGE)
                                               : remaining;

            types::serial_comm_hub_requests::VectorUint16 data_raw;
            data_raw.data.reserve(chunk_size);
            for (std::size_t i = 0; i < chunk_size; ++i) {
                data_raw.data.push_back(data[offset + i]);
            }

            types::serial_comm_hub_requests::StatusCodeEnum status = m_serial_hub.call_modbus_write_multiple_registers(
                static_cast<int>(m_device_id), static_cast<int>(write_address + static_cast<std::int32_t>(offset)),
                data_raw);

            if (status == types::serial_comm_hub_requests::StatusCodeEnum::Timeout) {
                throw transport::ModbusTimeoutException("Modbus write timeout: Packet receive timeout");
            } else if (status != types::serial_comm_hub_requests::StatusCodeEnum::Success) {
                std::string error_msg = "Failed to write Modbus registers: " +
                                        types::serial_comm_hub_requests::status_code_enum_to_string(status);
                throw std::runtime_error(error_msg);
            }

            offset += chunk_size;
        }
    });
}

} // namespace transport
