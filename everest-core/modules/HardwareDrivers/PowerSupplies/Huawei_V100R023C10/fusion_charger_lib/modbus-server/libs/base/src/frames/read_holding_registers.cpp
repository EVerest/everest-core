// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <modbus-server/frames/read_holding_registers.hpp>

using namespace modbus_server::pdu;

void ReadHoldingRegistersRequest::from_generic(const GenericPDU& generic) {
    if (generic.function_code != 0x03) {
        throw DecodingError(generic, "ReadHoldingRegisterRequest", "Invalid function code");
    }

    if (generic.data.size() != 4) {
        throw DecodingError(generic, "ReadHoldingRegisterRequest", "Invalid data size");
    }

    register_start = (generic.data[0] << 8) | generic.data[1];
    register_count = (generic.data[2] << 8) | generic.data[3];

    if (register_count == 0) {
        throw DecodingError(generic, "ReadHoldingRegisterRequest", "Register count cannot be zero");
    }

    if (register_count > 125) {
        throw DecodingError(generic, "ReadHoldingRegisterRequest", "Register count too big");
    }
}

GenericPDU ReadHoldingRegistersRequest::to_generic() const {
    GenericPDU generic;

    generic.function_code = 0x03;

    generic.data.push_back(register_start >> 8);
    generic.data.push_back(register_start & 0xFF);
    generic.data.push_back(register_count >> 8);
    generic.data.push_back(register_count & 0xFF);

    return generic;
}

void ReadHoldingRegistersResponse::from_generic(const GenericPDU& generic) {
    if (generic.function_code != 0x03) {
        throw DecodingError(generic, "ReadHoldingRegisterResponse", "Invalid function code");
    }

    if (generic.data.size() < 1) {
        throw DecodingError(generic, "ReadHoldingRegisterResponse", "Invalid data size");
    }

    register_count = generic.data[0] / 2;

    if (generic.data.size() != register_count * 2 + 1) {
        throw DecodingError(generic, "ReadHoldingRegisterResponse", "Invalid data size");
    }

    register_data = std::vector<uint8_t>(generic.data.begin() + 1, generic.data.end());
}

GenericPDU ReadHoldingRegistersResponse::to_generic() const {
    if (register_count > 125) {
        throw EncodingError("ReadHoldingRegistersResponse", "Register count too big");
    }

    if (register_data.size() != register_count * 2) {
        throw EncodingError("ReadHoldingRegistersResponse", "Data size (" + std::to_string(register_data.size()) +
                                                                ") does not match register count derived size (" +
                                                                std::to_string(register_count * 2) + ")");
    }

    GenericPDU generic;

    generic.function_code = 0x03;
    generic.data.push_back(register_count * 2);

    generic.data.insert(generic.data.end(), register_data.begin(), register_data.end());

    return generic;
}

std::vector<uint16_t> ReadHoldingRegistersResponse::get_register_data() const {
    std::vector<uint16_t> ret;
    for (int i = 0; i < register_data.size(); i += 2) {
        ret.push_back((register_data[i] << 8) | register_data[i + 1]);
    }
    return ret;
}
