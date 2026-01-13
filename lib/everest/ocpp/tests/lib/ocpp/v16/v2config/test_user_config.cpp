// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <optional>

#include "configuration_stub.hpp"

namespace {
using namespace ocpp::v16::stubs;

TEST_P(Configuration, setChargepointInformation) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getChargeBoxSerialNumber(), "cp001");
    EXPECT_EQ(get()->getChargePointSerialNumber(), std::nullopt);
    EXPECT_EQ(get()->getFirmwareVersion(), "0.1");

    EXPECT_EQ(get()->getChargePointVendor(), "Pionix");
    EXPECT_EQ(get()->getChargePointModel(), "Yeti");

    get()->setChargepointInformation("chargePointVendor", "chargePointModel", "chargePointSerialNumber",
                                     "chargeBoxSerialNumber", "firmwareVersion");
    EXPECT_EQ(get()->getChargePointVendor(), "chargePointVendor");
    EXPECT_EQ(get()->getChargePointModel(), "chargePointModel");
    EXPECT_EQ(get()->getChargePointSerialNumber(), "chargePointSerialNumber");
    EXPECT_EQ(get()->getChargeBoxSerialNumber(), "chargeBoxSerialNumber");
    EXPECT_EQ(get()->getFirmwareVersion(), "firmwareVersion");

    auto kv = get()->getChargeBoxSerialNumberKeyValue();
    EXPECT_EQ(kv.key, "ChargeBoxSerialNumber");
    EXPECT_EQ(kv.value, "chargeBoxSerialNumber");
    EXPECT_TRUE(kv.readonly);

    kv = get()->getFirmwareVersionKeyValue();
    EXPECT_EQ(kv.key, "FirmwareVersion");
    EXPECT_EQ(kv.value, "firmwareVersion");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, setChargepointMeterInformation) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getMeterSerialNumber(), std::nullopt);
    EXPECT_EQ(get()->getMeterType(), std::nullopt);

    get()->setChargepointMeterInformation("meterSerialNumber", "meterType");
    EXPECT_EQ(get()->getMeterSerialNumber(), "meterSerialNumber");
    EXPECT_EQ(get()->getMeterType(), "meterType");
}

TEST_P(Configuration, setChargepointModemInformation) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getICCID(), std::nullopt);
    EXPECT_EQ(get()->getIMSI(), std::nullopt);

    get()->setChargepointModemInformation("ICCID", "IMSI");
    EXPECT_EQ(get()->getICCID(), "ICCID");
    EXPECT_EQ(get()->getIMSI(), "IMSI");
}

} // namespace
