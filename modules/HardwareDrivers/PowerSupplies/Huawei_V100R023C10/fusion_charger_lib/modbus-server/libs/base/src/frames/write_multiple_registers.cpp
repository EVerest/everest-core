// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <modbus-server/frames/write_multiple_registers.hpp>

using namespace modbus_server::pdu;

void WriteMultipleRegistersRequest::from_generic(const GenericPDU& generic) {
    if (generic.function_code != 0x10) {
        throw DecodingError(generic, "WriteMultipleRegistersRequest", "Invalid function code");
    }

    if (generic.data.size() < 5) {
        throw DecodingError(generic, "WriteMultipleRegistersRequest", "Invalid data size");
    }

    register_start = (generic.data[0] << 8) | generic.data[1];
    register_count = (generic.data[2] << 8) | generic.data[3];
    std::uint8_t byte_count = generic.data[4];

    if (register_count > 0x007b) {
        throw DecodingError(generic, "WriteMultipleRegistersRequest",
                            "Register count too big, maximum allowed is 123 (0x007b)");
    }

    if (byte_count != register_count * 2) {
        throw DecodingError(generic, "WriteMultipleRegistersRequest", "Byte count does not match register count");
    }

    if (generic.data.size() != 5 + register_count * 2) {
        throw DecodingError(generic, "WriteMultipleRegistersRequest", "Invalid data size");
    }

    register_data = std::vector<std::uint8_t>(generic.data.begin() + 5, generic.data.end());
}

GenericPDU WriteMultipleRegistersRequest::to_generic() const {
    if (register_count > 0x007b) {
        throw EncodingError("WriteMultipleRegistersRequest", "Register count too big, maximum allowed is 123 (0x007b)");
    }

    if (register_data.size() != register_count * 2) {
        throw EncodingError("WriteMultipleRegistersRequest", "Byte count does not match register count");
    }

    GenericPDU generic;
    generic.function_code = 0x10;
    generic.data.push_back(register_start >> 8);
    generic.data.push_back(register_start & 0xFF);
    generic.data.push_back(register_count >> 8);
    generic.data.push_back(register_count & 0xFF);
    generic.data.push_back(register_count * 2);
    generic.data.insert(generic.data.end(), register_data.begin(), register_data.end());

    return generic;
}

void WriteMultipleRegistersResponse::from_generic(const GenericPDU& generic) {
    if (generic.function_code != 0x10) {
        throw DecodingError(generic, "WriteMultipleRegistersResponse", "Invalid function code");
    }

    if (generic.data.size() != 4) {
        throw DecodingError(generic, "WriteMultipleRegistersResponse", "Invalid data size");
    }

    register_start = (generic.data[0] << 8) | generic.data[1];
    register_count = (generic.data[2] << 8) | generic.data[3];
}

GenericPDU WriteMultipleRegistersResponse::to_generic() const {
    GenericPDU generic;

    generic.function_code = 0x10;

    generic.data.push_back(register_start >> 8);
    generic.data.push_back(register_start & 0xFF);
    generic.data.push_back(register_count >> 8);
    generic.data.push_back(register_count & 0xFF);

    return generic;
}
