// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <optional>

#include "configuration_stub.hpp"
#include "ocpp/v16/types.hpp"

namespace {
using namespace ocpp::v16::stubs;

TEST_P(Configuration, ChargingScheduleAllowedChargingRateUnit) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getChargingScheduleAllowedChargingRateUnit(), "Current");
    auto kv = get()->getChargingScheduleAllowedChargingRateUnitKeyValue();
    EXPECT_EQ(kv.key, "ChargingScheduleAllowedChargingRateUnit");
    EXPECT_EQ(kv.value, "Current");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, ChargeProfileMaxStackLevel) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getChargeProfileMaxStackLevel(), 42);
    auto kv = get()->getChargeProfileMaxStackLevelKeyValue();
    EXPECT_EQ(kv.key, "ChargeProfileMaxStackLevel");
    EXPECT_EQ(kv.value, "42");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, ChargingScheduleMaxPeriods) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getChargingScheduleMaxPeriods(), 42);
    auto kv = get()->getChargingScheduleMaxPeriodsKeyValue();
    EXPECT_EQ(kv.key, "ChargingScheduleMaxPeriods");
    EXPECT_EQ(kv.value, "42");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, MaxChargingProfilesInstalled) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getMaxChargingProfilesInstalled(), 42);
    auto kv = get()->getMaxChargingProfilesInstalledKeyValue();
    EXPECT_EQ(kv.key, "MaxChargingProfilesInstalled");
    EXPECT_EQ(kv.value, "42");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, ConnectorSwitch3to1PhaseSupported) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getConnectorSwitch3to1PhaseSupported(), std::nullopt);
    auto kv = get()->getConnectorSwitch3to1PhaseSupportedKeyValue();
    ASSERT_FALSE(kv);
}

// -----------------------------------------------------------------------------
// Oneoff tests where there are differences between implementations
// Note: TEST_F and not TEST_P

TEST_F(Configuration, SetChargingScheduleAllowedChargingRateUnitV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("SmartCharging", "ChargingScheduleAllowedChargingRateUnit", "Watts");

    EXPECT_EQ(v2_config->getChargingScheduleAllowedChargingRateUnit(), "Watts");
    auto kv = v2_config->getChargingScheduleAllowedChargingRateUnitKeyValue();
    EXPECT_EQ(kv.key, "ChargingScheduleAllowedChargingRateUnit");
    EXPECT_EQ(kv.value, "Watts");
    EXPECT_TRUE(kv.readonly);
}

TEST_F(Configuration, SetChargeProfileMaxStackLevelV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("SmartCharging", "ChargeProfileMaxStackLevel", "11");

    EXPECT_EQ(v2_config->getChargeProfileMaxStackLevel(), 11);
    auto kv = v2_config->getChargeProfileMaxStackLevelKeyValue();
    EXPECT_EQ(kv.key, "ChargeProfileMaxStackLevel");
    EXPECT_EQ(kv.value, "11");
    EXPECT_TRUE(kv.readonly);
}

TEST_F(Configuration, SetChargingScheduleMaxPeriodsV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("SmartCharging", "ChargingScheduleMaxPeriods", "12");

    EXPECT_EQ(v2_config->getChargingScheduleMaxPeriods(), 12);
    auto kv = v2_config->getChargingScheduleMaxPeriodsKeyValue();
    EXPECT_EQ(kv.key, "ChargingScheduleMaxPeriods");
    EXPECT_EQ(kv.value, "12");
    EXPECT_TRUE(kv.readonly);
}

TEST_F(Configuration, SetMaxChargingProfilesInstalledV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("SmartCharging", "MaxChargingProfilesInstalled", "13");

    EXPECT_EQ(v2_config->getMaxChargingProfilesInstalled(), 13);
    auto kv = v2_config->getMaxChargingProfilesInstalledKeyValue();
    EXPECT_EQ(kv.key, "MaxChargingProfilesInstalled");
    EXPECT_EQ(kv.value, "13");
    EXPECT_TRUE(kv.readonly);
}

TEST_F(Configuration, SetConnectorSwitch3to1PhaseSupportedV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("SmartCharging", "ConnectorSwitch3to1PhaseSupported", "");

    EXPECT_FALSE(v2_config->getConnectorSwitch3to1PhaseSupported().has_value());
    auto kv = v2_config->getConnectorSwitch3to1PhaseSupportedKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ConnectorSwitch3to1PhaseSupported");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_TRUE(kv.value().readonly);

    device_model->set("SmartCharging", "ConnectorSwitch3to1PhaseSupported", "true");

    EXPECT_TRUE(v2_config->getConnectorSwitch3to1PhaseSupported().has_value());
    EXPECT_TRUE(v2_config->getConnectorSwitch3to1PhaseSupported().value());

    kv = v2_config->getConnectorSwitch3to1PhaseSupportedKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ConnectorSwitch3to1PhaseSupported");
    EXPECT_EQ(kv.value().value, "true");
    EXPECT_TRUE(kv.value().readonly);
}

} // namespace
