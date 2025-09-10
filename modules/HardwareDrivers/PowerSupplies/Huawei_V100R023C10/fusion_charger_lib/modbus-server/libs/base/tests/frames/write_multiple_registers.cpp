// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-server/frames/write_multiple_registers.hpp>

using namespace modbus_server::pdu;

TEST(WriteMultipleRegistersRequest, to_generic_works_correctly) {
    WriteMultipleRegistersRequest request;

    request.register_start = 0xdead;
    request.register_count = 0x0002;
    request.register_data = {0xbe, 0xef, 0xca, 0xfe};

    GenericPDU generic = request.to_generic();

    std::vector<uint8_t> expected = {0xde, 0xad, 0x00, 0x02, 0x04, 0xbe, 0xef, 0xca, 0xfe};
    ASSERT_EQ(generic.function_code, 0x10);
    ASSERT_EQ(generic.data, expected);
}

TEST(WriteMultipleRegistersRequest, from_generic_works_correctly) {
    std::vector<uint8_t> data = {0x10, 0xde, 0xad, 0x00, 0x02, 0x04, 0xbe, 0xef, 0xca, 0xfe};

    GenericPDU generic(data);
    WriteMultipleRegistersRequest request;

    ASSERT_NO_THROW(request.from_generic(generic));
    ASSERT_EQ(request.register_start, 0xdead);
    ASSERT_EQ(request.register_count, 0x0002);
    ASSERT_EQ(request.register_data, std::vector<uint8_t>({0xbe, 0xef, 0xca, 0xfe}));
}

TEST(WriteMultipleRegistersResponse, to_generic_works_correctly) {
    WriteMultipleRegistersResponse response;

    response.register_start = 0xdead;
    response.register_count = 0x0002;

    GenericPDU generic = response.to_generic();

    std::vector<uint8_t> expected = {0xde, 0xad, 0x00, 0x02};
    ASSERT_EQ(generic.function_code, 0x10);
    ASSERT_EQ(generic.data, expected);
}

TEST(WriteMultipleRegistersResponse, from_generic_works_correctly) {
    std::vector<uint8_t> data = {0x10, 0xde, 0xad, 0x00, 0x02};

    GenericPDU generic(data);
    WriteMultipleRegistersResponse response;

    ASSERT_NO_THROW(response.from_generic(generic));
    ASSERT_EQ(response.register_start, 0xdead);
    ASSERT_EQ(response.register_count, 0x0002);
}

// todo: more tests
