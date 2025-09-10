// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <modbus-registers/converter.hpp>
#include <modbus-registers/data_provider.hpp>
#include <modbus-registers/registers.hpp>

using namespace modbus::registers;

TEST(DataProviderCallbacks, uint32_combined_works) {
    uint32_t value = 0;
    data_providers::DataProviderCallbacks<uint32_t> provider([&value]() { return value; },
                                                             [&value](uint32_t new_value) { value = new_value; });

    uint8_t sys_buffer[4] = {0, 0, 0, 0};
    uint8_t net_buffer[4] = {0, 0, 0, 0};
    provider.on_read(sys_buffer, 4);

    converters::ConverterABCD::instance().sys_to_net(sys_buffer, net_buffer, 4);
    EXPECT_EQ(net_buffer[0], 0);
    EXPECT_EQ(net_buffer[1], 0);
    EXPECT_EQ(net_buffer[2], 0);
    EXPECT_EQ(net_buffer[3], 0);

    net_buffer[0] = 0x12;
    net_buffer[1] = 0x34;
    net_buffer[2] = 0x56;
    net_buffer[3] = 0x78;
    converters::ConverterABCD::instance().net_to_sys(net_buffer, sys_buffer, 4);
    provider.on_write(sys_buffer, 4);
    EXPECT_EQ(value, 0x12345678);
}

TEST(DataProviderCallbacks, uint32_elemetary_register_rw) {
    uint32_t callback_call_count = 0;
    data_providers::DataProviderCallbacks<uint32_t> provider([]() { return 0xdeadbeef; },
                                                             [&callback_call_count](uint32_t new_value) {
                                                                 callback_call_count++;
                                                                 EXPECT_EQ(new_value, 0x12345678);
                                                             });

    complex_registers::ElementaryRegister<uint32_t> reg(0, provider, converters::ConverterABCD::instance());

    reg.on_write(0, {0x12, 0x34, 0x56, 0x78});
    EXPECT_EQ(callback_call_count, 1);

    std::vector<uint8_t> buffer = reg.on_read();
    EXPECT_EQ(buffer.size(), 4);
    EXPECT_EQ(buffer[0], 0xde);
    EXPECT_EQ(buffer[1], 0xad);
    EXPECT_EQ(buffer[2], 0xbe);
    EXPECT_EQ(buffer[3], 0xef);

    EXPECT_EQ(callback_call_count, 1); // should not have changed
}
