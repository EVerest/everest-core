// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "transport.hpp"

const int MAX_REGISTER_PER_MESSAGE = 125;

namespace transport {

transport::DataVector SerialCommHubTransport::fetch(int address, int register_count) {

    transport::DataVector response;
    response.reserve(register_count * 2); // this is a uint 8 vector

    int remaining_register_to_read{register_count};
    int read_address{address - m_base_address};

    while (remaining_register_to_read > 0) {
        std::size_t register_to_read = remaining_register_to_read > MAX_REGISTER_PER_MESSAGE
                                           ? MAX_REGISTER_PER_MESSAGE
                                           : remaining_register_to_read;

        types::serial_comm_hub_requests::Result serial_com_hub_result =
            m_serial_hub.call_modbus_read_input_registers(m_device_id, read_address, register_to_read);

        if (not serial_com_hub_result.value.has_value())
            throw std::runtime_error("no result from serial com hub!");

        // make sure that returned vector is a int32 vector
        static_assert(
            std::is_same_v<int32_t, decltype(types::serial_comm_hub_requests::Result::value)::value_type::value_type>);

        union {
            int32_t val_32;
            struct {
                uint8_t v3;
                uint8_t v2;
                uint8_t v1;
                uint8_t v0;
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
}

} // namespace transport
