// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_SERVER__GENERIC_SERVER_HPP
#define MODBUS_SERVER__GENERIC_SERVER_HPP

#include <exception>
#include <modbus-server/frames.hpp>

namespace modbus_server {

class ApplicationServerError : public std::exception {
private:
    uint8_t exception_code;
    std::vector<uint8_t> other_data; // generally not used but is theoretically possible

public:
    ApplicationServerError(pdu::PDUExceptionCode exception_code) :
        exception_code(static_cast<uint8_t>(exception_code)){};

    ApplicationServerError(uint8_t exception_code, const std::vector<uint8_t>& other_data = {}) :
        exception_code(exception_code), other_data(other_data){};

    const char* what() const noexcept override {
        return "ApplicationServerError";
    }

    pdu::GenericPDU to_pdu(uint8_t original_function_code) const {
        std::vector<uint8_t> data;
        data.push_back(exception_code);
        data.insert(data.end(), other_data.begin(), other_data.end());
        return pdu::GenericPDU(0x80 | original_function_code, data);
    }
};

}; // namespace modbus_server

#endif
