// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_SERVER__FRAMES__WRITE_MULTIPLE_REGISTERS_HPP
#define MODBUS_SERVER__FRAMES__WRITE_MULTIPLE_REGISTERS_HPP

#include "../frames.hpp"

namespace modbus_server {
namespace pdu {

/**
 * @brief Write multiple registers in the Modbus server
 *
 */
struct WriteMultipleRegistersRequest : public SpecificPDU {
    std::uint16_t register_start;
    std::uint16_t register_count;
    std::vector<std::uint8_t> register_data;

    WriteMultipleRegistersRequest() = default;
    ~WriteMultipleRegistersRequest() override = default;

    void from_generic(const GenericPDU& generic) override;
    GenericPDU to_generic() const override;
};

/**
 * @brief The response to a \c WriteMultipleRegistersRequest
 *
 */
struct WriteMultipleRegistersResponse : public SpecificPDU {
    std::uint16_t register_start;
    std::uint16_t register_count;

    WriteMultipleRegistersResponse() = default;
    // constructor for server side
    WriteMultipleRegistersResponse(const WriteMultipleRegistersRequest& req) :
        register_start(req.register_start), register_count(req.register_count) {
    }
    WriteMultipleRegistersResponse(std::uint16_t register_start, std::uint16_t register_count) :
        register_start(register_start), register_count(register_count) {
    }
    ~WriteMultipleRegistersResponse() override = default;

    void from_generic(const GenericPDU& generic) override;
    GenericPDU to_generic() const override;
};

} // namespace pdu
} // namespace modbus_server

#endif
