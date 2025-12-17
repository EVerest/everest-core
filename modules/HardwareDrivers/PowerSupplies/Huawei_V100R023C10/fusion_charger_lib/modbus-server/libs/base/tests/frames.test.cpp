// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-server/frames.hpp>

using namespace modbus_server::pdu;

TEST(GenericPDU, constructor_splits_data_correctly) {
    std::vector<std::uint8_t> data = {0x03, 0x00, 0x01};

    modbus_server::pdu::GenericPDU request(data);

    ASSERT_EQ(request.function_code, 0x03);
    ASSERT_EQ(request.data.size(), 2);
    ASSERT_EQ(request.data[0], 0x00);
    ASSERT_EQ(request.data[1], 0x01);
}

TEST(GenericPDU, to_vector_works) {
    GenericPDU pdu(0xab, {0xcd, 0xef});

    std::vector<std::uint8_t> expected = {0xab, 0xcd, 0xef};
    ASSERT_EQ(pdu.to_vector(), expected);
}

TEST(GenericPDU, to_string_works) {
    GenericPDU pdu(0xab, {0xcd, 0xef});

    std::string expected = "GenericPDU(fn_code: 171, data: [205, 239])";
    ASSERT_EQ(pdu.to_string(), expected);
}
