// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-registers/data_provider.hpp>
#include <modbus-registers/registers.hpp>

using namespace modbus::registers::converters;
using namespace modbus::registers::complex_registers;
using namespace modbus::registers::data_providers;

// u32_ABCD tests
TEST(ElementaryRegister, register_start_register_size) {
    ConverterIdentity converter;
    DataProviderHolding<std::uint32_t> provider(0);
    ElementaryRegister<std::uint32_t> reg(0xc0de, provider, converter);

    EXPECT_EQ(reg.get_start_address(), 0xc0de);
    EXPECT_EQ(reg.get_size(), 2);

    DataProviderHolding<std::uint64_t> provider2(0);
    ElementaryRegister<std::uint64_t> reg2(0xdead, provider2, converter);

    EXPECT_EQ(reg2.get_start_address(), 0xdead);
    EXPECT_EQ(reg2.get_size(), 4);
}

TEST(ElementaryRegister, u32_ABCD_write_exact) {
    DataProviderHolding<std::uint32_t> provider(0);
    ElementaryRegister<std::uint32_t> reg(0, provider, ConverterABCD::instance());

    reg.on_write(0, {0x12, 0x34, 0x56, 0x78});
    EXPECT_EQ(provider.get_value(), 0x12345678);
}

TEST(ElementaryRegister, u32_ABCD_write_too_long) {
    DataProviderHolding<std::uint32_t> provider(0);
    ElementaryRegister<std::uint32_t> reg(0, provider, ConverterABCD::instance());

    reg.on_write(0, {0x12, 0x34, 0x56, 0x78, 0x12});
    EXPECT_EQ(provider.get_value(), 0x12345678);
}

TEST(ElementaryRegister, u32_ABCD_write_too_short) {
    DataProviderHolding<std::uint32_t> provider(0);
    ElementaryRegister<std::uint32_t> reg(0, provider, ConverterABCD::instance());

    reg.on_write(0, {
                        0x12,
                        0x34,
                    });
    EXPECT_EQ(provider.get_value(), 0x12340000);
}

TEST(ElementaryRegister, u32_ABCD_write_offset_basic) {
    DataProviderHolding<std::uint32_t> provider(0);
    ElementaryRegister<std::uint32_t> reg(0, provider, ConverterABCD::instance());

    reg.on_write(0, {0x12, 0x34, 0x56, 0x78});
    EXPECT_EQ(provider.get_value(), 0x12345678);
    // note: the offset is in registers (2 bytes)
    reg.on_write(1, {0xde, 0xad});
    EXPECT_EQ(provider.get_value(), 0x1234dead);
}

TEST(ElementaryRegister, u32_ABCD_write_offset_too_long) {
    DataProviderHolding<std::uint32_t> provider(0);
    ElementaryRegister<std::uint32_t> reg(0, provider, ConverterABCD::instance());

    reg.on_write(0, {0x12, 0x34, 0x56, 0x78});
    EXPECT_EQ(provider.get_value(), 0x12345678);
    // note: the offset is in registers (2 bytes)
    reg.on_write(1, {0xde, 0xad, 0xbe, 0xef});
    EXPECT_EQ(provider.get_value(), 0x1234dead);
}

// float_ABCD

TEST(ElementaryRegister, float_ABCD_write_exact) {
    DataProviderHolding<float> provider(0);
    ElementaryRegister<float> reg(0, provider, ConverterABCD::instance());

    reg.on_write(0, {0x41, 0x48, 0x00, 0x00});
    EXPECT_FLOAT_EQ(provider.get_value(), 12.5);
    reg.on_write(0, {0x40, 0x49, 0x06, 0x25});
    EXPECT_FLOAT_EQ(provider.get_value(), 3.141);
}

TEST(ElementaryRegister, float_ABCD_write_too_long) {
    DataProviderHolding<float> provider(0);
    ElementaryRegister<float> reg(0, provider, ConverterABCD::instance());

    reg.on_write(0, {0x41, 0x48, 0x00, 0x00, 0x41});
    EXPECT_FLOAT_EQ(provider.get_value(), 12.5);
}

// i64_ABCD tests

TEST(ElementaryRegister, i64_ABCD_write_exact) {
    DataProviderHolding<std::int64_t> provider(0);
    ElementaryRegister<std::int64_t> reg(0, provider, ConverterABCD::instance());

    reg.on_write(0, {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});
    EXPECT_EQ(provider.get_value(), 0x123456789abcdef0);
}

TEST(ElementaryRegister, i64_ABCD_2_complement_write_exact) {
    DataProviderHolding<std::int64_t> provider(0);
    ElementaryRegister<std::int64_t> reg(0, provider, ConverterABCD::instance());

    reg.on_write(0, {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});
    EXPECT_EQ(provider.get_value(), 0x123456789abcdef0);
    reg.on_write(0, {0xed, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21, 0x10});
    EXPECT_EQ(provider.get_value(), -0x123456789abcdef0);
}

// todo: tests for other converters
