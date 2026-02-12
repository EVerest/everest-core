// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include "configuration_stub.hpp"

namespace {
using namespace ocpp::v16::stubs;

TEST_P(Configuration, LocalAuthListEnabled) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_TRUE(get()->getLocalAuthListEnabled());
    auto kv = get()->getLocalAuthListEnabledKeyValue();
    EXPECT_EQ(kv.key, "LocalAuthListEnabled");
    EXPECT_EQ(kv.value, "true");
    EXPECT_FALSE(kv.readonly);

    get()->setLocalAuthListEnabled(false);
    EXPECT_FALSE(get()->getLocalAuthListEnabled());
    kv = get()->getLocalAuthListEnabledKeyValue();
    EXPECT_EQ(kv.key, "LocalAuthListEnabled");
    EXPECT_EQ(kv.value, "false");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, LocalAuthListMaxLength) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getLocalAuthListMaxLength(), 42);
    auto kv = get()->getLocalAuthListMaxLengthKeyValue();
    EXPECT_EQ(kv.key, "LocalAuthListMaxLength");
    EXPECT_EQ(kv.value, "42");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, SendLocalListMaxLength) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getSendLocalListMaxLength(), 42);
    auto kv = get()->getSendLocalListMaxLengthKeyValue();
    EXPECT_EQ(kv.key, "SendLocalListMaxLength");
    EXPECT_EQ(kv.value, "42");
    EXPECT_TRUE(kv.readonly);
}

// -----------------------------------------------------------------------------
// Oneoff tests where there are differences between implementations
// Note: TEST_F and not TEST_P

TEST_F(Configuration, GetLocalAuthListMaxLengthV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("LocalAuthListManagement", "LocalAuthListMaxLength", "101");

    EXPECT_EQ(v2_config->getLocalAuthListMaxLength(), 101);
    auto kv = v2_config->getLocalAuthListMaxLengthKeyValue();
    EXPECT_EQ(kv.key, "LocalAuthListMaxLength");
    EXPECT_EQ(kv.value, "101");
    EXPECT_TRUE(kv.readonly);
}

TEST_F(Configuration, GetSendLocalListMaxLengthV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("LocalAuthListManagement", "SendLocalListMaxLength", "102");

    EXPECT_EQ(v2_config->getSendLocalListMaxLength(), 102);
    auto kv = v2_config->getSendLocalListMaxLengthKeyValue();
    EXPECT_EQ(kv.key, "SendLocalListMaxLength");
    EXPECT_EQ(kv.value, "102");
    EXPECT_TRUE(kv.readonly);
}

} // namespace
