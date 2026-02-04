// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include "configuration_stub.hpp"

/*
    "DefaultPrice":
    {
        "priceText": "This is the price",
        "priceTextOffline": "Show this price text when offline!",
        "chargingPrice":
        {
            "kWhPrice": 3.14,
            "hourPrice": 0.42
        }
    },
    "DefaultPriceText":
    {
        "priceTexts":
        [
            {
                "priceText": "This is the price",
                "priceTextOffline": "Show this price text when offline!",
                "language": "en"
            },
            {
                "priceText": "Dit is de prijs",
                "priceTextOffline": "Laat dit zien wanneer de charging station offline is!",
                "language": "nl"
            },
            {
                "priceText": "Dette er prisen",
                "priceTextOffline": "Vis denne pristeksten når du er frakoblet",
                "language": "nb_NO"
            }
        ]
    },
*/

namespace {
using namespace ocpp::v16::stubs;

TEST_P(Configuration, CustomDisplayCostAndPriceEnabled) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getCustomDisplayCostAndPriceEnabled());
    auto kv = get()->getCustomDisplayCostAndPriceEnabledKeyValue();
    EXPECT_EQ(kv.key, "CustomDisplayCostAndPrice");
    EXPECT_EQ(kv.value, "false");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, DefaultTariffMessage) {
    ASSERT_NE(get(), nullptr);
    auto msg = get()->getDefaultTariffMessage(false);
    EXPECT_FALSE(msg.ocpp_transaction_id.has_value());
    EXPECT_FALSE(msg.identifier_id.has_value());
    EXPECT_FALSE(msg.identifier_type.has_value());
    EXPECT_TRUE(msg.message.empty());

    msg = get()->getDefaultTariffMessage(true);
    EXPECT_FALSE(msg.ocpp_transaction_id.has_value());
    EXPECT_FALSE(msg.identifier_id.has_value());
    EXPECT_FALSE(msg.identifier_type.has_value());
    EXPECT_TRUE(msg.message.empty());
}

TEST_P(Configuration, DefaultPrice) {
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;

    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getDefaultPrice().has_value());
    auto kv = get()->getDefaultPriceKeyValue();
    ASSERT_FALSE(kv);

    const char* minimal = R"({"priceText":"Default"})";

    auto status = get()->setDefaultPrice(minimal);
    EXPECT_EQ(status, ConfigurationStatus::Accepted);
    auto json_result = json::parse(get()->getDefaultPrice().value_or(""));
    auto result = json_result.dump();
    EXPECT_EQ(result, minimal);

    kv = get()->getDefaultPriceKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "DefaultPrice");
    json_result = json::parse(std::string{kv.value().value.value()});
    result = json_result.dump();
    EXPECT_EQ(result, minimal);
    EXPECT_FALSE(kv.value().readonly);
}

TEST_P(Configuration, DefaultPriceText) {
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;

    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getDefaultPriceText("en").has_value());
    auto kv = get()->getDefaultPriceTextKeyValue("en");
    EXPECT_EQ(kv.key, "DefaultPriceText,en");
    EXPECT_EQ(kv.value, "");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, DisplayTimeOffset) {
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;

    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getDisplayTimeOffset().has_value());
    auto kv = get()->getDisplayTimeOffsetKeyValue();
    ASSERT_FALSE(kv);

    auto status = get()->setDisplayTimeOffset("1:30");
    EXPECT_EQ(status, ConfigurationStatus::Accepted);
    EXPECT_EQ(get()->getDisplayTimeOffset(), "1:30");
    kv = get()->getDisplayTimeOffsetKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "TimeOffset");
    EXPECT_EQ(kv.value().value, "1:30");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_P(Configuration, Language) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getLanguage().has_value());
    auto kv = get()->getLanguageKeyValue();
    ASSERT_FALSE(kv);

    get()->setLanguage("de");
    EXPECT_EQ(get()->getLanguage(), "de");
    kv = get()->getLanguageKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "Language");
    EXPECT_EQ(kv.value().value, "de");
    EXPECT_TRUE(kv.value().readonly);
}

TEST_P(Configuration, MultiLanguageSupportedLanguages) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getMultiLanguageSupportedLanguages().has_value());
    auto kv = get()->getMultiLanguageSupportedLanguagesKeyValue();
    ASSERT_FALSE(kv);
}

TEST_P(Configuration, NextTimeOffsetTransitionDateTime) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getNextTimeOffsetTransitionDateTime().has_value());
    auto kv = get()->getNextTimeOffsetTransitionDateTimeKeyValue();
    ASSERT_FALSE(kv);

    get()->setNextTimeOffsetTransitionDateTime("2200-01-01T12:00:00");
    EXPECT_EQ(get()->getNextTimeOffsetTransitionDateTime(), "2200-01-01T12:00:00");
    kv = get()->getNextTimeOffsetTransitionDateTimeKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "NextTimeOffsetTransitionDateTime");
    EXPECT_EQ(kv.value().value, "2200-01-01T12:00:00");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_P(Configuration, TimeOffsetNextTransition) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getTimeOffsetNextTransition().has_value());
    auto kv = get()->getTimeOffsetNextTransitionKeyValue();
    ASSERT_FALSE(kv);

    get()->setTimeOffsetNextTransition("-01:15");
    EXPECT_EQ(get()->getTimeOffsetNextTransition(), "-01:15");
    kv = get()->getTimeOffsetNextTransitionKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "TimeOffsetNextTransition");
    EXPECT_EQ(kv.value().value, "-01:15");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_P(Configuration, CustomIdleFeeAfterStop) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getCustomIdleFeeAfterStop().has_value());
    auto kv = get()->getCustomIdleFeeAfterStopKeyValue();
    ASSERT_FALSE(kv);

    get()->setCustomIdleFeeAfterStop(false);
    EXPECT_TRUE(get()->getCustomIdleFeeAfterStop().has_value());
    EXPECT_FALSE(get()->getCustomIdleFeeAfterStop().value());
    kv = get()->getCustomIdleFeeAfterStopKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "CustomIdleFeeAfterStop");
    EXPECT_EQ(kv.value().value, "false");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_P(Configuration, CustomMultiLanguageMessagesEnabled) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getCustomMultiLanguageMessagesEnabled().has_value());
    auto kv = get()->getCustomMultiLanguageMessagesEnabledKeyValue();
    ASSERT_FALSE(kv);
}

TEST_P(Configuration, WaitForSetUserPriceTimeout) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getWaitForSetUserPriceTimeout().has_value());
    auto kv = get()->getWaitForSetUserPriceTimeoutKeyValue();
    ASSERT_FALSE(kv);

    get()->setWaitForSetUserPriceTimeout(3602);
    EXPECT_FALSE(get()->getWaitForSetUserPriceTimeout().has_value());
    kv = get()->getWaitForSetUserPriceTimeoutKeyValue();
    ASSERT_FALSE(kv);
}

TEST_P(Configuration, PriceNumberOfDecimalsForCostValues) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getPriceNumberOfDecimalsForCostValues().has_value());
    auto kv = get()->getPriceNumberOfDecimalsForCostValuesKeyValue();
    ASSERT_FALSE(kv);
}

TEST_P(ConfigurationFull, BadPriceText) {
    ASSERT_NE(get(), nullptr);
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;

    // PriceText value is JSON encoded - check that malformed and invalid
    // messages are correctly handled
    const char* valid = R"({"priceText":"default"})";
    const char* invalid = R"({"priceText":null,"priceTextOffline":null,"chargingPrice":null})";

    auto set_result = get()->set("DefaultPrice", valid);
    EXPECT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), ConfigurationStatus::Accepted);

    auto get_result = get()->getDefaultPrice();
    ASSERT_TRUE(get_result.has_value());
    auto get_json = json::parse(get_result.value());
    EXPECT_EQ(get_json["priceText"], "default");

    set_result = get()->set("DefaultPrice", invalid);
    EXPECT_TRUE(set_result.has_value());
    EXPECT_EQ(set_result.value(), ConfigurationStatus::Rejected);

    auto get_result2 = get()->getDefaultPrice();
    ASSERT_TRUE(get_result2.has_value());
    EXPECT_EQ(get_result2, get_result);
    get_json = json::parse(get_result.value());
    EXPECT_EQ(get_json["priceText"], "default");

    auto set_result2 = get()->setDefaultPrice(invalid);
    EXPECT_EQ(set_result2, ConfigurationStatus::Rejected);

    get_result2 = get()->getDefaultPrice();
    ASSERT_TRUE(get_result2.has_value());
    EXPECT_EQ(get_result2, get_result);
    get_json = json::parse(get_result.value());
    EXPECT_EQ(get_json["priceText"], "default");
}

TEST_P(ConfigurationFull, DefaultPriceTextEmptyArray) {
    ASSERT_NE(get(), nullptr);
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;
    const char* empty = R"([])";
    auto set_result = get()->set("DefaultPriceText,en", empty);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
    set_result = get()->setDefaultPriceText("DefaultPriceText,en", empty);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
}

TEST_P(ConfigurationFull, DefaultPriceTextEmptyObject) {
    ASSERT_NE(get(), nullptr);
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;
    const char* empty = R"({})";
    auto set_result = get()->set("DefaultPriceText,en", empty);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
    set_result = get()->setDefaultPriceText("DefaultPriceText,en", empty);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
}

TEST_P(ConfigurationFull, DefaultPriceInvalid) {
    ASSERT_NE(get(), nullptr);
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;
    const char* minimal = R"("priceText":[])";

    auto set_result = get()->set("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
    set_result = get()->setDefaultPriceText("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Rejected);
}

TEST_P(ConfigurationFull, DefaultPriceTextMinimal) {
    ASSERT_NE(get(), nullptr);
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;
    const char* minimal = R"({"priceText":"Default"})";

    auto set_result = get()->set("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Accepted);
    set_result = get()->setDefaultPriceText("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Accepted);
}

TEST_P(ConfigurationFull, DefaultPriceTextFull) {
    ASSERT_NE(get(), nullptr);
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;
    const char* minimal = R"({"priceText":"Default","priceTextOffline":"Offline"})";

    auto set_result = get()->set("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Accepted);
    set_result = get()->setDefaultPriceText("DefaultPriceText,en", minimal);
    EXPECT_EQ(set_result, ConfigurationStatus::Accepted);
}

// -----------------------------------------------------------------------------
// Oneoff tests where there are differences between implementations
// Note: TEST_F and not TEST_P

TEST_F(Configuration, GetMultiLanguageSupportedLanguagesV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("CostAndPrice", "SupportedLanguages", "en,de");

    EXPECT_TRUE(v2_config->getMultiLanguageSupportedLanguages().has_value());
    EXPECT_EQ(v2_config->getMultiLanguageSupportedLanguages().value(), "en,de");
    auto kv = v2_config->getMultiLanguageSupportedLanguagesKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "SupportedLanguages");
    EXPECT_EQ(kv.value().value, "en,de");
    EXPECT_TRUE(kv.value().readonly);
}

TEST_F(Configuration, GetCustomMultiLanguageMessagesEnabledV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("CostAndPrice", "CustomMultiLanguageMessages", "false");

    EXPECT_TRUE(v2_config->getCustomMultiLanguageMessagesEnabled().has_value());
    EXPECT_FALSE(v2_config->getCustomMultiLanguageMessagesEnabled().value());
    auto kv = v2_config->getCustomMultiLanguageMessagesEnabledKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CustomMultiLanguageMessages");
    EXPECT_EQ(kv.value().value, "false");
    EXPECT_TRUE(kv.value().readonly);
}

TEST_F(Configuration, SetWaitForSetUserPriceTimeoutV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("CostAndPrice", "WaitForSetUserPriceTimeout", "");

    EXPECT_FALSE(v2_config->getWaitForSetUserPriceTimeout().has_value());
    auto kv = v2_config->getWaitForSetUserPriceTimeoutKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "WaitForSetUserPriceTimeout");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setWaitForSetUserPriceTimeout(3001);
    EXPECT_TRUE(v2_config->getWaitForSetUserPriceTimeout().has_value());
    EXPECT_EQ(v2_config->getWaitForSetUserPriceTimeout().value(), 3001);
    kv = v2_config->getWaitForSetUserPriceTimeoutKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "WaitForSetUserPriceTimeout");
    EXPECT_EQ(kv.value().value, "3001");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, GetPriceNumberOfDecimalsForCostValuesV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("CostAndPrice", "NumberOfDecimalsForCostValues", "3");

    EXPECT_TRUE(v2_config->getPriceNumberOfDecimalsForCostValues().has_value());
    EXPECT_EQ(v2_config->getPriceNumberOfDecimalsForCostValues().value(), 3);
    auto kv = v2_config->getPriceNumberOfDecimalsForCostValuesKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "NumberOfDecimalsForCostValues");
    EXPECT_EQ(kv.value().value, "3");
    EXPECT_TRUE(kv.value().readonly);
}

TEST_F(Configuration, SetDefaultPriceTextV2) {
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("CostAndPrice", "NumberOfDecimalsForCostValues", "3");

    EXPECT_FALSE(v2_config->getDefaultPriceText("en").has_value());
    auto kv = v2_config->getDefaultPriceTextKeyValue("en");
    EXPECT_EQ(kv.key, "DefaultPriceText,en");
    EXPECT_EQ(kv.value, "");
    EXPECT_FALSE(kv.readonly);

    // needs config item for multi language support
    // getMultiLanguageSupportedLanguages()
    device_model->set("CostAndPrice", "SupportedLanguages", "en,de");

    const char* value_en = R"({
  "priceText": "This is the price",
  "priceTextOffline": "Show this price text when offline!"
})";
    const char* value_de = R"({
  "priceText": "Das ist der Preis",
  "priceTextOffline": "Diesen Preistext anzeigen, wenn Sie offline sind!"
})";

    auto status = v2_config->setDefaultPriceText("DefaultPriceText,en", value_en);
    EXPECT_EQ(status, ConfigurationStatus::Accepted);
    EXPECT_EQ(v2_config->getDefaultPriceText("en"), value_en);
    kv = v2_config->getDefaultPriceTextKeyValue("en");
    EXPECT_EQ(kv.key, "DefaultPriceText,en");
    ASSERT_TRUE(kv.value.has_value());
    EXPECT_EQ(kv.value.value(), value_en);
    EXPECT_FALSE(kv.readonly);

    status = v2_config->setDefaultPriceText("DefaultPriceText,de", value_de);
    EXPECT_EQ(status, ConfigurationStatus::Accepted);
    EXPECT_EQ(v2_config->getDefaultPriceText("de"), value_de);
    kv = v2_config->getDefaultPriceTextKeyValue("de");
    EXPECT_EQ(kv.key, "DefaultPriceText,de");
    ASSERT_TRUE(kv.value.has_value());
    EXPECT_EQ(kv.value.value(), value_de);
    EXPECT_FALSE(kv.readonly);

    auto list = v2_config->getAllDefaultPriceTextKeyValues();
    ASSERT_TRUE(list);
    ASSERT_EQ(list.value().size(), 2);
    const auto& lv = list.value();

    int en_index = 0;
    int de_index = 1;
    if (lv[0].key == "DefaultPriceText,de") {
        en_index = 1;
        de_index = 0;
    }

    EXPECT_EQ(lv[en_index].key, "DefaultPriceText,en");
    ASSERT_TRUE(lv[en_index].value.has_value());
    EXPECT_EQ(lv[en_index].value.value(), value_en);
    EXPECT_FALSE(lv[en_index].readonly);

    EXPECT_EQ(lv[de_index].key, "DefaultPriceText,de");
    ASSERT_TRUE(lv[de_index].value.has_value());
    EXPECT_EQ(lv[de_index].value.value(), value_de);
    EXPECT_FALSE(lv[de_index].readonly);
}
} // namespace
