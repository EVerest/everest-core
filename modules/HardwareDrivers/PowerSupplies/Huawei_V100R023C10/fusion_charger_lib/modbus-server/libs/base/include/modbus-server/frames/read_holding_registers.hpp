// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_SERVER__FRAMES__READ_HOLDING_REGISTERS_HPP
#define MODBUS_SERVER__FRAMES__READ_HOLDING_REGISTERS_HPP

#include "../frames.hpp"

namespace modbus_server {
namespace pdu {

struct ReadHoldingRegistersRequest : public SpecificPDU {
    std::uint16_t register_start;
    std::uint16_t register_count;

    ReadHoldingRegistersRequest() = default;
    ~ReadHoldingRegistersRequest() override = default;

    void from_generic(const GenericPDU& generic) override;
    GenericPDU to_generic() const override;
};

struct ReadHoldingRegistersResponse : public SpecificPDU {
    std::uint16_t register_count;
    std::vector<std::uint8_t> register_data;

    ReadHoldingRegistersResponse(const ReadHoldingRegistersRequest& req, std::vector<std::uint8_t> data) :
        register_count(req.register_count), register_data(data) {
    }
    ReadHoldingRegistersResponse() = default;
    ~ReadHoldingRegistersResponse() override = default;

    void from_generic(const GenericPDU& generic) override;
    GenericPDU to_generic() const override;

    /**
     * @brief Get big-endian register data, as an alternative to \c register_data
     *
     * @return std::vector<std::uint16_t> the register data from big-endian format
     * (converted to system endianness)
     */
    std::vector<std::uint16_t> get_register_data() const;
};

} // namespace pdu
} // namespace modbus_server

#endif
