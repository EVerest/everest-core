// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-registers/data_provider.hpp>
#include <modbus-registers/registers.hpp>

using namespace modbus::registers::converters;
using namespace modbus::registers::complex_registers;
using namespace modbus::registers::data_providers;

TEST(MemoryRegister, read_normal) {
    DataProviderMemoryHolding<4> provider;
    MemoryRegister<4> reg(0, provider, ConverterIdentity::instance());

    std::uint8_t val[4] = {0xde, 0xad, 0xbe, 0xef};
    provider.update_value(val);

    auto read_val = reg.on_read();
    EXPECT_EQ(read_val.size(), 4);
    EXPECT_EQ(read_val[0], 0xde);
    EXPECT_EQ(read_val[1], 0xad);
    EXPECT_EQ(read_val[2], 0xbe);
    EXPECT_EQ(read_val[3], 0xef);
}

TEST(MemoryRegister, write_normal) {
    DataProviderMemoryHolding<4> provider;
    MemoryRegister<4> reg(0, provider, ConverterIdentity::instance());

    reg.on_write(0, {0xde, 0xad, 0xbe, 0xef});
    auto val = provider.get_value();
    EXPECT_EQ(val[0], 0xde);
    EXPECT_EQ(val[1], 0xad);
    EXPECT_EQ(val[2], 0xbe);
    EXPECT_EQ(val[3], 0xef);
}

TEST(MemoryRegister, write_callback) {
    DataProviderMemoryHolding<4> provider;
    MemoryRegister<4> reg(0, provider, ConverterIdentity::instance());

    std::uint8_t write_count = 0;
    provider.add_write_callback([&write_count](const std::uint8_t* val) { write_count++; });

    ASSERT_EQ(write_count, 0);

    reg.on_write(0, {0xde, 0xad, 0xbe, 0xef});

    EXPECT_EQ(write_count, 1);
}

TEST(MemoryRegister, vector_constructors) {
    DataProviderMemoryHolding<4> provider({0xde, 0xad, 0xbe, 0xef});
    MemoryRegister<4> reg(0, provider, ConverterIdentity::instance());

    auto read_val = reg.on_read();
    EXPECT_EQ(read_val.size(), 4);
    EXPECT_EQ(read_val[0], 0xde);
    EXPECT_EQ(read_val[1], 0xad);
    EXPECT_EQ(read_val[2], 0xbe);
    EXPECT_EQ(read_val[3], 0xef);
}
