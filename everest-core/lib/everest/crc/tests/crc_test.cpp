// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest/crc/crc.hpp>
#include <gtest/gtest.h>

TEST(CRC16_XModem, KnownVector) {
    std::vector<std::uint8_t> data{'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    EXPECT_EQ(calculate_xModem_crc16(data), 0x31C3);
}

TEST(CRC16_XModem, EmptyInput) {
    std::vector<std::uint8_t> data{};
    EXPECT_EQ(calculate_xModem_crc16(data), 0x0000);
}

TEST(CRC16_XModem, MultipleBytes) {
    std::vector<std::uint8_t> data{'E', 'V', 'E', 'R', 'E', 'S', 'T'};
    EXPECT_EQ(calculate_xModem_crc16(data), 0x4492);
}
