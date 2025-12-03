// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <ocpp/v2/utils.hpp>

namespace ocpp {
namespace common {

class V2UtilsTest : public ::testing::Test {
protected:
    const std::string empty_input;
    const std::string short_input = "hello there";
    const std::string long_input =
        "hello there hello there hello there hello there hello there hello there hello there hello there "
        "hello there hello there hello there hello there hello there hello there hello there hello there "
        "hello there hello there hello there hello there hello there hello there hello there hello there "
        "hello there hello there hello there hello there hello there hello there hello there hello there";
    const ocpp::v2::IdToken valid_central_token = {"valid", ocpp::v2::IdTokenEnum::Central};
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(V2UtilsTest, test_valid_sha256) {
    ASSERT_EQ("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855", ocpp::v2::utils::sha256(empty_input));
    ASSERT_EQ("12998c017066eb0d2a70b94e6ed3192985855ce390f321bbdb832022888bd251", ocpp::v2::utils::sha256(short_input));
    ASSERT_EQ("34aa8868354dc8eb0e76fbc8b5f13259094bcdf5688c6a48a2bcb89ed863d441", ocpp::v2::utils::sha256(long_input));
}

TEST_F(V2UtilsTest, test_valid_generate_token_hash) {
    ocpp::v2::IdToken valid_iso14443_token = {"ABAD1DEA", ocpp::v2::IdTokenEnum::ISO14443};
    ocpp::v2::IdToken valid_iso15693_token = {"ABAD1DEA", ocpp::v2::IdTokenEnum::ISO15693};

    ASSERT_EQ("63f3202a9c2e08a033a861481c6259e7a70a2b6e243f91233ebf26f33859c113",
              ocpp::v2::utils::generate_token_hash(valid_central_token));
    ASSERT_EQ("1cc0ce8b95f44d43273c46a062af3d15a06e3d2170909b1fdebd634027aebef1",
              ocpp::v2::utils::generate_token_hash(valid_iso14443_token));
    ASSERT_NE(ocpp::v2::utils::generate_token_hash(valid_central_token),
              ocpp::v2::utils::generate_token_hash(valid_iso14443_token));
    ASSERT_NE(ocpp::v2::utils::generate_token_hash(valid_iso14443_token),
              ocpp::v2::utils::generate_token_hash(valid_iso15693_token));
}

TEST_F(V2UtilsTest, test_is_critical_security_event) {
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::FIRMWARE_UPDATED));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::SETTINGSYSTEMTIME));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::STARTUP_OF_THE_DEVICE));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::RESET_OR_REBOOT));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::SECURITYLOGWASCLEARED));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::MEMORYEXHAUSTION));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::TAMPERDETECTIONACTIVATED));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::INVALIDFIRMWARESIGNATURE));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::INVALIDFIRMWARESIGNINGCERTIFICATE));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::INVALIDCSMSCERTIFICATE));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::INVALIDCHARGINGSTATIONCERTIFICATE));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::INVALIDTLSVERSION));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::INVALIDTLSCIPHERSUITE));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::MAINTENANCELOGINACCEPTED));
    EXPECT_TRUE(ocpp::v2::utils::is_critical(ocpp::security_events::MAINTENANCELOGINFAILED));

    EXPECT_FALSE(ocpp::v2::utils::is_critical(ocpp::security_events::FAILEDTOAUTHENTICATEATCSMS));
    EXPECT_FALSE(ocpp::v2::utils::is_critical(ocpp::security_events::CSMSFAILEDTOAUTHENTICATE));
    EXPECT_FALSE(ocpp::v2::utils::is_critical(ocpp::security_events::RECONFIGURATIONOFSECURITYPARAMETERS));
    EXPECT_FALSE(ocpp::v2::utils::is_critical(ocpp::security_events::INVALIDMESSAGES));
    EXPECT_FALSE(ocpp::v2::utils::is_critical(ocpp::security_events::ATTEMPTEDREPLAYATTACKS));
}

} // namespace common
} // namespace ocpp
