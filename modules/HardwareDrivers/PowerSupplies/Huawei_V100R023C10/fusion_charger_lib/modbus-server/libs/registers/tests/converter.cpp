// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-registers/converter.hpp>

using namespace modbus::registers::converters;

TEST(ConverterABCD, NetToSys) {
    ConverterABCD converter;
    std::uint8_t in[] = {0x12, 0x34, 0x56, 0x78};
    std::uint8_t out[sizeof(in)];
    converter.net_to_sys(in, out, sizeof(in));
    EXPECT_EQ(out[0], 0x78);
    EXPECT_EQ(out[1], 0x56);
    EXPECT_EQ(out[2], 0x34);
    EXPECT_EQ(out[3], 0x12);
}

TEST(ConverterABCD, SysToNet) {
    ConverterABCD converter;
    std::uint8_t in[] = {0x78, 0x56, 0x34, 0x12};
    std::uint8_t out[sizeof(in)];
    converter.sys_to_net(in, out, sizeof(in));
    EXPECT_EQ(out[0], 0x12);
    EXPECT_EQ(out[1], 0x34);
    EXPECT_EQ(out[2], 0x56);
    EXPECT_EQ(out[3], 0x78);
}

TEST(ConverterIdentity, NetToSys) {
    ConverterIdentity converter;
    std::uint8_t in[] = {0x12, 0x34, 0x56, 0x78};
    std::uint8_t out[sizeof(in)];
    converter.net_to_sys(in, out, sizeof(in));
    EXPECT_EQ(out[0], 0x12);
    EXPECT_EQ(out[1], 0x34);
    EXPECT_EQ(out[2], 0x56);
    EXPECT_EQ(out[3], 0x78);
}

TEST(ConverterIdentity, SysToNet) {
    ConverterIdentity converter;
    std::uint8_t in[] = {0x12, 0x34, 0x56, 0x78};
    std::uint8_t out[sizeof(in)];
    converter.sys_to_net(in, out, sizeof(in));
    EXPECT_EQ(out[0], 0x12);
    EXPECT_EQ(out[1], 0x34);
    EXPECT_EQ(out[2], 0x56);
    EXPECT_EQ(out[3], 0x78);
}
