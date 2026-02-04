// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include "configuration_stub.hpp"
#include <ocpp/v16/charge_point_configuration_base.hpp>
#include <optional>

namespace {
using namespace ocpp::v16::stubs;

// run tests against V16 JSON and V2 database
// gtest_filter: Config/Configuration.*
INSTANTIATE_TEST_SUITE_P(Config, Configuration, testing::Values("sql", "json"));

TEST(ConnectorID, Extract) {
    using CPCB = ocpp::v16::ChargePointConfigurationBase;

    EXPECT_EQ(CPCB::extractConnectorId(""), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorId("1234"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorId("[1234"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorId("1234]"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorId("[1]"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorId("A[]"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorId("A[1.3]"), std::nullopt);
    EXPECT_EQ(CPCB::extractConnectorId("A[1]"), 1);
    EXPECT_EQ(CPCB::extractConnectorId("A[12]"), 12);
    EXPECT_EQ(CPCB::extractConnectorId("A[123]"), 123);
}

TEST(ConnectorID, Build) {
    using CPCB = ocpp::v16::ChargePointConfigurationBase;

    EXPECT_EQ(CPCB::meterPublicKeyString(0), "MeterPublicKey[0]");
    EXPECT_EQ(CPCB::meterPublicKeyString(1), "MeterPublicKey[1]");
    EXPECT_EQ(CPCB::meterPublicKeyString(12), "MeterPublicKey[12]");
    EXPECT_EQ(CPCB::meterPublicKeyString(123), "MeterPublicKey[123]");
}

TEST(ConnectorID, PhaseRotation) {
    using CPCB = ocpp::v16::ChargePointConfigurationBase;
    const char* no_phase_rotation = "0.NotApplicable,1.Unknown,2.NotApplicable,3.Unknown";
    const char* valid_phase_rotation = "0.RST,1.RTS,2.SRT,3.STR,4.TRS,5.TSR";
    const char* valid_phase_rotation_extended = "8.RST,9.RTS,10.SRT,11.STR,12.TRS,13.TSR";

    EXPECT_TRUE(CPCB::isConnectorPhaseRotationValid(5, no_phase_rotation));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(1, no_phase_rotation));

    EXPECT_TRUE(CPCB::isConnectorPhaseRotationValid(5, valid_phase_rotation));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(3, valid_phase_rotation));

    EXPECT_TRUE(CPCB::isConnectorPhaseRotationValid(15, valid_phase_rotation_extended));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(11, valid_phase_rotation_extended));

    EXPECT_TRUE(CPCB::isConnectorPhaseRotationValid(5, ""));

    // error cases
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "123456"));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "abcdef"));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, ".abcd"));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "abcd."));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "1."));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "11."));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "111."));
    EXPECT_FALSE(CPCB::isConnectorPhaseRotationValid(5, "1a.RST"));
}

} // namespace
