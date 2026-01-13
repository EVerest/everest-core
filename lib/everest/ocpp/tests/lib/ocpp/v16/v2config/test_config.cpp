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

    EXPECT_EQ(CPCB::extract_connector_id(""), std::nullopt);
    EXPECT_EQ(CPCB::extract_connector_id("1234"), std::nullopt);
    EXPECT_EQ(CPCB::extract_connector_id("[1234"), std::nullopt);
    EXPECT_EQ(CPCB::extract_connector_id("1234]"), std::nullopt);
    EXPECT_EQ(CPCB::extract_connector_id("[1]"), std::nullopt);
    EXPECT_EQ(CPCB::extract_connector_id("A[]"), std::nullopt);
    EXPECT_EQ(CPCB::extract_connector_id("A[1.3]"), std::nullopt);
    EXPECT_EQ(CPCB::extract_connector_id("A[1]"), 1);
    EXPECT_EQ(CPCB::extract_connector_id("A[12]"), 12);
    EXPECT_EQ(CPCB::extract_connector_id("A[123]"), 123);
}

TEST(ConnectorID, Build) {
    using CPCB = ocpp::v16::ChargePointConfigurationBase;

    EXPECT_EQ(CPCB::MeterPublicKey_string(0), "MeterPublicKey[0]");
    EXPECT_EQ(CPCB::MeterPublicKey_string(1), "MeterPublicKey[1]");
    EXPECT_EQ(CPCB::MeterPublicKey_string(12), "MeterPublicKey[12]");
    EXPECT_EQ(CPCB::MeterPublicKey_string(123), "MeterPublicKey[123]");
}

} // namespace
