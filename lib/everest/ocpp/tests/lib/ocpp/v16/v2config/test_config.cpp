// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include "configuration_stub.hpp"
#include "ocpp/v16/known_keys.hpp"
#include "ocpp/v2/ocpp_types.hpp"
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

using namespace ocpp;

TEST(V2Mapping, V16ToV2) {
    using namespace ocpp::v16::keys;
    using namespace ocpp::v2;

    auto res = convert_v2(valid_keys::CpoName);
    ASSERT_TRUE(res);
    auto component = std::get<Component>(res.value());
    auto variable = std::get<Variable>(res.value());
    EXPECT_EQ(component.name, "SecurityCtrlr");
    EXPECT_EQ(variable.name, "OrganizationName");

    res = convert_v2("CpoName");
    ASSERT_TRUE(res);
    component = std::get<Component>(res.value());
    variable = std::get<Variable>(res.value());
    EXPECT_EQ(component.name, "SecurityCtrlr");
    EXPECT_EQ(variable.name, "OrganizationName");
}

TEST(V2Mapping, V2ToV16) {
    using namespace ocpp::v16::keys;
    using namespace ocpp::v2;

    Component comp;
    comp.name = "SecurityCtrlr";
    Variable var;
    var.name = "OrganizationName";
    auto res = convert_v2(comp, var);
    EXPECT_EQ(res, "CpoName");
}

} // namespace
