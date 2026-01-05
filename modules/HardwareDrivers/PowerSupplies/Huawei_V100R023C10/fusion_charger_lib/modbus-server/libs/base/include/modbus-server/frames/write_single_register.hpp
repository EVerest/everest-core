// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_SERVER__FRAMES__WRITE_SINGLE_REGISTER_HPP
#define MODBUS_SERVER__FRAMES__WRITE_SINGLE_REGISTER_HPP

#include "../frames.hpp"

namespace modbus_server {
namespace pdu {

/**
 * @brief Write a single register in the Modbus server
 * @note request and response are the same (format-wise)
 */
struct WriteSingleRegister : public SpecificPDU {
    std::uint16_t register_address;
    std::uint16_t register_value;

    WriteSingleRegister() = default;
    ~WriteSingleRegister() override = default;

    void from_generic(const GenericPDU& generic) override;
    GenericPDU to_generic() const override;
};

struct WriteSingleRegisterRequest : public WriteSingleRegister {
    WriteSingleRegisterRequest() = default;
};
struct WriteSingleRegisterResponse : public WriteSingleRegister {
    WriteSingleRegisterResponse() = default;
    WriteSingleRegisterResponse(const WriteSingleRegisterRequest& req);
};

} // namespace pdu
} // namespace modbus_server

#endif
