// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-server/frames.hpp>

using namespace modbus_server::pdu;

TEST(GenericPDU, to_vector_edge_case_1_works) {
    GenericPDU pdu(0xab, {});

    std::vector<uint8_t> expected = {0xab};
    ASSERT_EQ(pdu.to_vector(), expected);
}

TEST(GenericPDU, to_vector_edge_case_2_works) {
    std::vector<uint8_t> data = {};

    ASSERT_ANY_THROW(GenericPDU a(data));
}
