
// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <filesystem>
#include <fstream>
#include <memory>

#include <gtest/gtest.h>

#include <ocpp/common/schemas.hpp>
#include <ocpp/v16/charge_point_configuration.hpp>

namespace {
using namespace ocpp::v16;

struct ConfigurationTester : public testing::Test {
    std::unique_ptr<ChargePointConfiguration> config;

    void SetUp() override {
        fs::path cfg{CONFIG_DIR_V16};
        cfg /= "config-full.json";
        std::ifstream ifs(cfg);
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

TEST(PartialSchemaValidator, CostAndPrice) {
    fs::path schema_file = CONFIG_DIR_V16;
    schema_file /= "profile_schemas/CostAndPrice.json";
    std::ifstream ifs(schema_file);
    auto schema_json = json::parse(ifs);
    ocpp::Schemas schema(schema_json);

    const char* valid_str = R"({"priceText":"1"})";
    const char* invalid_str = R"({"priceText":null,"priceTextOffline":null,"chargingPrice":null})";

    auto valid_json = json::parse(valid_str);
    auto invalid_json = json::parse(invalid_str);

    auto validator = schema.get_validator();

    json to_test;
    to_test["CustomDisplayCostAndPrice"] = false;
    to_test["DefaultPrice"] = valid_json;
    EXPECT_NO_THROW(validator->validate(to_test));
    to_test["DefaultPrice"] = invalid_json;
    EXPECT_ANY_THROW(validator->validate(to_test));
}

TEST_F(ConfigurationTester, BadPriceText) {
    // PriceText value is JSON encoded - check that malformed and invalid
    // messages are correctly handled
    const char* valid = R"({"priceText":"default"})";
    const char* invalid = R"({"priceText":null,"priceTextOffline":null,"chargingPrice":null})";

    auto set_result = config->set("DefaultPrice", valid);
    EXPECT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), ConfigurationStatus::Accepted);

    auto get_result = config->getDefaultPrice();
    ASSERT_TRUE(get_result.has_value());
    auto get_json = json::parse(get_result.value());
    EXPECT_EQ(get_json["priceText"], "default");

    set_result = config->set("DefaultPrice", invalid);
    EXPECT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), ConfigurationStatus::Rejected);

    auto get_result2 = config->getDefaultPrice();
    ASSERT_TRUE(get_result2.has_value());
    EXPECT_EQ(get_result2, get_result);
    get_json = json::parse(get_result.value());
    EXPECT_EQ(get_json["priceText"], "default");

    auto set_result2 = config->setDefaultPrice(invalid);
    EXPECT_EQ(set_result2, ConfigurationStatus::Rejected);

    get_result2 = config->getDefaultPrice();
    ASSERT_TRUE(get_result2.has_value());
    EXPECT_EQ(get_result2, get_result);
    get_json = json::parse(get_result.value());
    EXPECT_EQ(get_json["priceText"], "default");
}

TEST_F(ConfigurationTester, DefaultPriceTextEmptyArray) {
    const char* empty = R"([])";
    auto set_result = config->set("DefaultPriceText,en", empty);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
    set_result = config->setDefaultPriceText("DefaultPriceText,en", empty);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
}

TEST_F(ConfigurationTester, DefaultPriceTextEmptyObject) {
    const char* empty = R"({})";
    auto set_result = config->set("DefaultPriceText,en", empty);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
    set_result = config->setDefaultPriceText("DefaultPriceText,en", empty);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
}

TEST_F(ConfigurationTester, DefaultPriceInvalid) {
    const char* minimal = R"("priceText":[])";

    auto set_result = config->set("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
    set_result = config->setDefaultPriceText("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
}

TEST_F(ConfigurationTester, DefaultPriceTextMinimal) {
    const char* minimal = R"({"priceText":"Default"})";

    auto set_result = config->set("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Accepted);
    set_result = config->setDefaultPriceText("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Accepted);
}

TEST_F(ConfigurationTester, DefaultPriceText) {
    const char* minimal = R"({"priceText":"Default","priceTextOffline":"Offline"})";

    auto set_result = config->set("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Accepted);
    set_result = config->setDefaultPriceText("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Accepted);
}

} // namespace
