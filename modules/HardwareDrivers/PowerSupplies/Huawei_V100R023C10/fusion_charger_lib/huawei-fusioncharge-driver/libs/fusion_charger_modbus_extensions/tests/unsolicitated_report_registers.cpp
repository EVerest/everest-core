// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <fusion_charger/modbus/extensions/unsolicitated_registers.hpp>

using namespace fusion_charger::modbus_extensions;

TEST(DataProviderHoldingUnsolicitated, basic_positive_test) {
    bool unsolicitated_report = true;

    DataProviderHoldingUnsolicitatedReportCallback<uint16_t> data_provider(
        0x1234, [&unsolicitated_report]() { return unsolicitated_report; });
    modbus::registers::data_providers::DataProviderHolding<uint16_t>& data_provider_base = data_provider;
    modbus::registers::complex_registers::ElementaryRegister<uint16_t> reg(
        0x1234, data_provider_base, modbus::registers::converters::ConverterABCD::instance());

    // check initial value
    auto val = reg.on_read();
    ASSERT_EQ(val[0], 0x12);
    ASSERT_EQ(val[1], 0x34);

    auto report = unsolicitated_report_helper(&reg);
    ASSERT_TRUE(report.has_value());
    ASSERT_EQ(report.value().size(), 2);
    ASSERT_EQ(report.value()[0], 0x12);
    ASSERT_EQ(report.value()[1], 0x34);

    unsolicitated_report = false;
    report = unsolicitated_report_helper(&reg);
    ASSERT_FALSE(report.has_value());

    unsolicitated_report = true;
    reg.on_write(0, {0x56, 0x78});

    val = reg.on_read();
    ASSERT_EQ(val[0], 0x56);
    ASSERT_EQ(val[1], 0x78);

    report = unsolicitated_report_helper(&reg);
    ASSERT_TRUE(report.has_value());
    ASSERT_EQ(report.value().size(), 2);
    ASSERT_EQ(report.value()[0], 0x56);
    ASSERT_EQ(report.value()[1], 0x78);
}

TEST(DataProviderUnsolicitatedEvent, basic_positive_test) {
    bool unsolicitated_report = true;

    DataProviderUnsolicitatedEvent<uint16_t> data_provider(0x1234);
    modbus::registers::data_providers::DataProviderHolding<uint16_t>& data_provider_base = data_provider;
    modbus::registers::complex_registers::ElementaryRegister<uint16_t> reg(
        0x1234, data_provider_base, modbus::registers::converters::ConverterABCD::instance());

    // read always works
    auto val = reg.on_read();
    ASSERT_EQ(val[0], 0x12);
    ASSERT_EQ(val[1], 0x34);

    // should not report yet
    auto report = unsolicitated_report_helper(&reg);
    ASSERT_FALSE(report.has_value());

    report = unsolicitated_report_helper(&reg);
    ASSERT_FALSE(report.has_value());

    // report
    data_provider.report(0x5678);

    // read should work
    val = reg.on_read();
    ASSERT_EQ(val[0], 0x56);
    ASSERT_EQ(val[1], 0x78);

    // should report once
    report = unsolicitated_report_helper(&reg);
    ASSERT_TRUE(report.has_value());
    ASSERT_EQ(report.value().size(), 2);
    ASSERT_EQ(report.value()[0], 0x56);
    ASSERT_EQ(report.value()[1], 0x78);

    // should not report again
    report = unsolicitated_report_helper(&reg);
    ASSERT_FALSE(report.has_value());
}
