// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-registers/registry.hpp>

using namespace modbus::registers::complex_registers;
using namespace modbus::registers::registry;
using namespace modbus::registers::data_providers;
using namespace modbus::registers::converters;

TEST(Registry, write_cases) {
    DataProviderHolding<std::uint32_t> provider(0);
    DataProviderHolding<std::uint64_t> provider2(0);

    auto subregistry = std::make_shared<ComplexRegisterSubregistry>();
    subregistry->add(new ElementaryRegister<std::uint32_t>(0x0010, provider, ConverterABCD::instance()));
    subregistry->add(new ElementaryRegister<std::uint64_t>(0x0012, provider2, ConverterABCD::instance()));

    EXPECT_NO_THROW(subregistry->verify_internal_overlap());

    ComplexRegisterRegistry registry;
    registry.add(subregistry);
    EXPECT_NO_THROW(registry.verify_overlap());

    //  [  ][      ]
    //  |--|
    registry.on_write(0x0010, {0x12, 0x34, 0x56, 0x78});
    EXPECT_EQ(provider.get_value(), 0x12345678);

    // [  ]
    //   ||
    registry.on_write(0x0011, {0xde, 0xad});
    EXPECT_EQ(provider.get_value(), 0x1234dead);

    //   [  ]
    // |--|
    registry.on_write(0x000F, {0xbe, 0xef, 0xca, 0xfe});
    EXPECT_EQ(provider.get_value(), 0xcafedead);

    // [  ][      ]
    //     |--|
    registry.on_write(0x0012, {0x01, 0x02, 0x03, 0x04});
    EXPECT_EQ(provider2.get_value(), 0x0102030400000000);

    //   [  ][      ]
    // |------|
    registry.on_write(0x000F, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08});
    EXPECT_EQ(provider.get_value(), 0x03040506);
    EXPECT_EQ(provider2.get_value(), 0x0708030400000000);

    // [  ][      ]
    //     |------|
    registry.on_write(0x0012, {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08});
    EXPECT_EQ(provider.get_value(), 0x03040506);
    EXPECT_EQ(provider2.get_value(), 0x0102030405060708);
}

TEST(Registry, read_cases) {
    DataProviderHolding<std::uint32_t> provider(0x0010);
    DataProviderHolding<std::uint64_t> provider2(0x0012);

    auto subregistry = std::make_shared<ComplexRegisterSubregistry>();
    subregistry->add(new ElementaryRegister<std::uint32_t>(0x0010, provider, ConverterABCD::instance()));
    subregistry->add(new ElementaryRegister<std::uint64_t>(0x0012, provider2, ConverterABCD::instance()));

    EXPECT_NO_THROW(subregistry->verify_internal_overlap());

    ComplexRegisterRegistry registry;
    registry.add(subregistry);
    EXPECT_NO_THROW(registry.verify_overlap());

    provider.update_value(0x12345678);
    provider2.update_value(0x0102030405060708);

    auto data = registry.on_read(0x0010, 0);
    EXPECT_EQ(data.size(), 0);

    // [  ][    ]
    //     |--|
    data = registry.on_read(0x0012, 2);
    EXPECT_EQ(data.size(), 4);
    EXPECT_EQ(data[0], 0x01);
    EXPECT_EQ(data[1], 0x02);
    EXPECT_EQ(data[2], 0x03);
    EXPECT_EQ(data[3], 0x04);

    // [  ][      ]
    //     |------|
    data = registry.on_read(0x0012, 4);
    EXPECT_EQ(data.size(), 8);
    EXPECT_EQ(data[0], 0x01);
    EXPECT_EQ(data[1], 0x02);
    EXPECT_EQ(data[2], 0x03);
    EXPECT_EQ(data[3], 0x04);
    EXPECT_EQ(data[4], 0x05);
    EXPECT_EQ(data[5], 0x06);
    EXPECT_EQ(data[6], 0x07);
    EXPECT_EQ(data[7], 0x08);

    // [  ][      ]
    //   |--|
    data = registry.on_read(0x0011, 2);
    EXPECT_EQ(data.size(), 4);
    EXPECT_EQ(data[0], 0x56);
    EXPECT_EQ(data[1], 0x78);
    EXPECT_EQ(data[2], 0x01);
    EXPECT_EQ(data[3], 0x02);

    // [  ][      ]
    // |--|
    data = registry.on_read(0x0010, 2);
    EXPECT_EQ(data.size(), 4);
    EXPECT_EQ(data[0], 0x12);
    EXPECT_EQ(data[1], 0x34);
    EXPECT_EQ(data[2], 0x56);
    EXPECT_EQ(data[3], 0x78);

    // [  ][      ]
    // |----------|
    data = registry.on_read(0x0010, 6);
    EXPECT_EQ(data.size(), 12);
    EXPECT_EQ(data[0], 0x12);
    EXPECT_EQ(data[1], 0x34);
    EXPECT_EQ(data[2], 0x56);
    EXPECT_EQ(data[3], 0x78);
    EXPECT_EQ(data[4], 0x01);
    EXPECT_EQ(data[5], 0x02);
    EXPECT_EQ(data[6], 0x03);
    EXPECT_EQ(data[7], 0x04);
    EXPECT_EQ(data[8], 0x05);
    EXPECT_EQ(data[9], 0x06);
    EXPECT_EQ(data[10], 0x07);
    EXPECT_EQ(data[11], 0x08);
}
