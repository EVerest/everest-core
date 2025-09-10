// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-registers/registers.hpp>

using namespace modbus::registers::complex_registers;
using namespace modbus::registers::data_providers;

TEST(Assumptions, elementary_register_with_enum_type_works) {
    enum class MyEnum : uint16_t {
        VALUE_1 = 0x1234,
        VALUE_2 = 0x5678,
    };

    DataProviderHolding<MyEnum> data_provider(MyEnum::VALUE_1);

    ElementaryRegister<MyEnum> reg(0x0000, data_provider, modbus::registers::converters::ConverterABCD::instance());

    EXPECT_EQ(data_provider.get_value(), MyEnum::VALUE_1);
    auto read = reg.on_read();
    EXPECT_EQ(read, std::vector<uint8_t>({0x12, 0x34}));
    reg.on_write(0, {0x56, 0x78});
    EXPECT_EQ(data_provider.get_value(), MyEnum::VALUE_2);
}
