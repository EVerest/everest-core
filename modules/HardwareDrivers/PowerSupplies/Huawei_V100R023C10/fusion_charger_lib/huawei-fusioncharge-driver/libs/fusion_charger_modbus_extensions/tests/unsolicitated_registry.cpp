// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include <fusion_charger/modbus/extensions/unsolicitated_registry.hpp>
#include <memory>

using namespace fusion_charger::modbus_extensions;
using namespace modbus::registers;
using namespace modbus::registers::data_providers;

class TestSubregistry : public registry::ComplexRegisterSubregistry {
public:
    struct DataProviders {
        DataProvider<uint16_t>& reg_holding;
        DataProviderUnsolicitated<uint16_t>& reg_unsolicitated1;
        DataProviderUnsolicitated<uint16_t>& reg_unsolicitated2;
    };

    TestSubregistry(DataProviders providers) {
        // clang-format off
    this->add(new complex_registers::ElementaryRegister<uint16_t>(0x0000, providers.reg_holding,       converters::ConverterABCD::instance()));
    this->add(new complex_registers::ElementaryRegister<uint16_t>(0x0001, providers.reg_unsolicitated1, converters::ConverterABCD::instance()));
    this->add(new complex_registers::ElementaryRegister<uint16_t>(0x0002, providers.reg_unsolicitated2, converters::ConverterABCD::instance()));
        // clang-format on
    }
};

TEST(UnsolicitatedRegistry, basic_positive_test) {
    DataProviderHolding<uint16_t> reg_holding(0x1234);
    DataProviderUnsolicitatedEvent<uint16_t> reg_unsolicitated1(0x5678);
    DataProviderUnsolicitatedEvent<uint16_t> reg_unsolicitated2(0x9abc);
    UnsolicitatedRegistry registry;

    {
        TestSubregistry::DataProviders data_providers{reg_holding, reg_unsolicitated1, reg_unsolicitated2};
        registry.add(std::make_shared<TestSubregistry>(data_providers));
    }

    // nothing should report now
    auto report = registry.unsolicitated_report();
    ASSERT_FALSE(report.has_value());

    // if one wants to be reported, next report should include this
    reg_unsolicitated1.report(0x1234);
    report = registry.unsolicitated_report();
    ASSERT_TRUE(report.has_value());
    ASSERT_EQ(report->devices.size(), 1);
    ASSERT_EQ(report->devices[0].location, 0x0000);
    ASSERT_EQ(report->devices[0].segments.size(), 1);
    ASSERT_EQ(report->devices[0].segments[0].registers_start, 0x0001);
    ASSERT_EQ(report->devices[0].segments[0].registers_count, 0x0001);
    ASSERT_EQ(report->devices[0].segments[0].registers.size(), 2);
    ASSERT_EQ(report->devices[0].segments[0].registers[0], 0x12);
    ASSERT_EQ(report->devices[0].segments[0].registers[1], 0x34);

    // nothing should report now
    report = registry.unsolicitated_report();
    ASSERT_FALSE(report.has_value());

    // if both want to be reported, next report should include both, defragmented
    reg_unsolicitated1.report(0xdead);
    reg_unsolicitated2.report(0xbeef);
    report = registry.unsolicitated_report();
    ASSERT_TRUE(report.has_value());
    ASSERT_EQ(report->devices.size(), 1);
    ASSERT_EQ(report->devices[0].location, 0x0000);
    ASSERT_EQ(report->devices[0].segments.size(), 1);
    ASSERT_EQ(report->devices[0].segments[0].registers_start, 0x0001);
    ASSERT_EQ(report->devices[0].segments[0].registers_count, 0x0002);
    ASSERT_EQ(report->devices[0].segments[0].registers.size(), 4);
    ASSERT_EQ(report->devices[0].segments[0].registers[0], 0xde);
    ASSERT_EQ(report->devices[0].segments[0].registers[1], 0xad);
    ASSERT_EQ(report->devices[0].segments[0].registers[2], 0xbe);
    ASSERT_EQ(report->devices[0].segments[0].registers[3], 0xef);
}
