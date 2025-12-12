// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-server/frames/write_single_register.hpp>

using namespace modbus_server::pdu;

// Assert that WriteSingleRegisterRequest and WriteSingleRegisterResponse are
// derived from WriteSingleRegister
static_assert(std::is_base_of<WriteSingleRegister, WriteSingleRegisterRequest>::value);
static_assert(std::is_base_of<WriteSingleRegister, WriteSingleRegisterResponse>::value);

TEST(WriteSingleRegister, to_generic_works_correctly) {
    WriteSingleRegisterRequest request;

    request.register_address = 0xdead;
    request.register_value = 0xbeef;

    GenericPDU generic = request.to_generic();

    std::vector<std::uint8_t> expected = {0xde, 0xad, 0xbe, 0xef};
    ASSERT_EQ(generic.function_code, 0x06);
    ASSERT_EQ(generic.data, expected);
}

TEST(WriteSingleRegister, from_generic_works_correctly) {
    std::vector<std::uint8_t> data = {0x06, 0xde, 0xad, 0xbe, 0xef};

    GenericPDU generic(data);
    WriteSingleRegisterRequest request;

    ASSERT_NO_THROW(request.from_generic(generic));
    ASSERT_EQ(request.register_address, 0xdead);
    ASSERT_EQ(request.register_value, 0xbeef);
}
