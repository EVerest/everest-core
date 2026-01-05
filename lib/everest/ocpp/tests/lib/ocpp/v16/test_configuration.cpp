
// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <fstream>
#include <memory>

#include <gtest/gtest.h>

#include <ocpp/v16/charge_point_configuration.hpp>

namespace {
using namespace ocpp::v16;

struct ConfigurationTester : public testing::Test {
    std::unique_ptr<ChargePointConfiguration> config;

    void SetUp() override {
        std::ifstream ifs(CONFIG_FILE_LOCATION_V16);
        const std::string config_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        config = std::make_unique<ChargePointConfiguration>(config_file, CONFIG_DIR_V16, USER_CONFIG_FILE_LOCATION_V16);
    }
};

TEST_F(ConfigurationTester, SetUnknown) {
    auto get_result = config->get("HeartBeatInterval");
    EXPECT_TRUE(get_result.has_value());
    get_result = config->get("DoesNotExist");
    EXPECT_FALSE(get_result.has_value());

    auto set_result = config->set("HeartBeatInterval", "352");
    EXPECT_EQ(set_result, ConfigurationStatus::Accepted);
    set_result = config->set("DoesNotExist", "never-set");
    EXPECT_FALSE(set_result.has_value()); // std::nullopt indicates key not known
}

TEST_F(ConfigurationTester, BrokenChain) {
    // set() has a chain of if .. else if ..
    // test that there isn't a missing else
    // IgnoredProfilePurposesOffline is the fist key

    // actually returns rejected rather than accepted
    // this is fine since the error case would be std::nullopt
    auto set_result = config->set("IgnoredProfilePurposesOffline", "TxProfile");
    EXPECT_TRUE(set_result.has_value());
}

} // namespace
