// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <modbus-server/frames/write_single_register.hpp>

using namespace modbus_server::pdu;

void WriteSingleRegister::from_generic(const GenericPDU& generic) {
    if (generic.function_code != 0x06) {
        throw DecodingError(generic, "WriteSingleRegisterRequest", "Invalid function code");
    }

    if (generic.data.size() != 4) {
        throw DecodingError(generic, "WriteSingleRegisterRequest", "Invalid data size");
    }

    register_address = (generic.data[0] << 8) | generic.data[1];
    register_value = (generic.data[2] << 8) | generic.data[3];
}

GenericPDU WriteSingleRegister::to_generic() const {
    GenericPDU generic;

    generic.function_code = 0x06;

    generic.data.push_back(register_address >> 8);
    generic.data.push_back(register_address & 0xFF);
    generic.data.push_back(register_value >> 8);
    generic.data.push_back(register_value & 0xFF);

    return generic;
}

WriteSingleRegisterResponse::WriteSingleRegisterResponse(const WriteSingleRegisterRequest& req) :
    WriteSingleRegister(req) {
}
