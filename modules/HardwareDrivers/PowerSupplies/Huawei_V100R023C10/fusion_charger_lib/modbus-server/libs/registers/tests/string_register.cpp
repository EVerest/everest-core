// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-registers/data_provider.hpp>
#include <modbus-registers/registers.hpp>

using namespace modbus::registers::converters;
using namespace modbus::registers::complex_registers;
using namespace modbus::registers::data_providers;

TEST(StringRegister, register_start_register_size) {
    ConverterIdentity converter;
    DataProviderStringHolding<8> provider;
    StringRegister<8> reg(0xbeef, provider, converter);

    EXPECT_EQ(reg.get_start_address(), 0xbeef);
    EXPECT_EQ(reg.get_size(), 4);
}

TEST(StringRegister, write_basic) {
    ConverterIdentity converter;
    DataProviderStringHolding<8> provider;
    StringRegister<8> reg(0, provider, converter);

    reg.on_write(0, {0x54, 0x65, 0x6C, 0x65, 0});
    EXPECT_STREQ(provider.get_value(), "Tele");
}

TEST(StringRegister, write_too_long) {
    ConverterIdentity converter;
    DataProviderStringHolding<4> provider;
    StringRegister<4> reg(0, provider, converter);

    reg.on_write(0, {0x54, 0x65, 0x6C, 0x65, 0x54, 0x65, 0x6C, 0x65});
    EXPECT_STREQ(provider.get_value(), "Tele");
}

TEST(StringRegister, write_exact_with_termination) {
    ConverterIdentity converter;
    DataProviderStringHolding<4> provider;
    StringRegister<4> reg(0, provider, converter);

    reg.on_write(0, {0x54, 0x65, 0x6C, 0x65, 0});
    EXPECT_STREQ(provider.get_value(), "Tele");
}

// todo: is this wanted (auto-termination)?
TEST(StringRegister, write_exact_without_termination) {
    ConverterIdentity converter;
    DataProviderStringHolding<4> provider;
    StringRegister<4> reg(0, provider, converter);

    reg.on_write(0, {0x54, 0x65, 0x6C, 0x65});
    EXPECT_STREQ(provider.get_value(), "Tele");
}

TEST(StringRegister, offset_write_in_range) {
    ConverterIdentity converter;
    DataProviderStringHolding<8> provider;
    StringRegister<8> reg(0, provider, converter);

    reg.on_write(0, {0x54, 0x65, 0x6C, 0x65, 0});
    EXPECT_STREQ(provider.get_value(), "Tele");
    // note: the offset is in registers (2 bytes)
    reg.on_write(2, {0x54, 0x65, 0x6C, 0x65, 0});
    EXPECT_STREQ(provider.get_value(), "TeleTele");
}

TEST(StringRegister, offset_write_too_long) {
    ConverterIdentity converter;
    DataProviderStringHolding<10> provider;
    StringRegister<10> reg(0, provider, converter);

    reg.on_write(0, {0x54, 0x65, 0x6C, 0x65, 0});
    EXPECT_STREQ(provider.get_value(), "Tele");
    // note: the offset is in registers (2 bytes)
    reg.on_write(2, {0x54, 0x65, 0x6C, 0x65, 0x74, 0x75, 0x62, 0x62, 0x79});
    EXPECT_STREQ(provider.get_value(), "TeleTeletu");
}

TEST(StringRegister, offset_write_exact) {
    ConverterIdentity converter;
    DataProviderStringHolding<8> provider;
    StringRegister<8> reg(0, provider, converter);

    reg.on_write(0, {0x54, 0x65, 0x6C, 0x65});
    EXPECT_STREQ(provider.get_value(), "Tele");
    // note: the offset is in registers (2 bytes)
    reg.on_write(2, {0x54, 0x65, 0x6C, 0x65});
    EXPECT_STREQ(provider.get_value(), "TeleTele");
}

TEST(StringRegister, read_basic) {
    ConverterIdentity converter;
    DataProviderStringHolding<8> provider;
    StringRegister<8> reg(0, provider, converter);

    provider.update_value("Hello");
    auto data = reg.on_read();
    EXPECT_EQ(data.size(), 8);
    EXPECT_EQ(data[0], 0x48);
    EXPECT_EQ(data[1], 0x65);
    EXPECT_EQ(data[2], 0x6C);
    EXPECT_EQ(data[3], 0x6C);
    EXPECT_EQ(data[4], 0x6F);
    EXPECT_EQ(data[5], 0);
    EXPECT_EQ(data[6], 0);
    EXPECT_EQ(data[7], 0);
}

TEST(StringRegister, read_too_long_has_no_termination) {
    ConverterIdentity converter;
    DataProviderStringHolding<8> provider;
    StringRegister<8> reg(0, provider, converter);

    provider.update_value("Hello World");
    auto data = reg.on_read();
    EXPECT_EQ(data.size(), 8);
    EXPECT_EQ(data[0], 0x48);
    EXPECT_EQ(data[1], 0x65);
    EXPECT_EQ(data[2], 0x6C);
    EXPECT_EQ(data[3], 0x6C);
    EXPECT_EQ(data[4], 0x6F);
    EXPECT_EQ(data[5], 0x20);
    EXPECT_EQ(data[6], 0x57);
    EXPECT_EQ(data[7], 0x6F);
}
