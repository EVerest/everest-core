// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include "configuration_stub.hpp"

namespace {
using namespace ocpp::v16::stubs;

TEST_P(Configuration, ContractValidationOffline) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_TRUE(get()->getContractValidationOffline());
    auto kv = get()->getContractValidationOfflineKeyValue();
    EXPECT_EQ(kv.key, "ContractValidationOffline");
    EXPECT_EQ(kv.value, "true");
    EXPECT_FALSE(kv.readonly);

    get()->setContractValidationOffline(false);
    EXPECT_FALSE(get()->getContractValidationOffline());
    kv = get()->getContractValidationOfflineKeyValue();
    EXPECT_EQ(kv.key, "ContractValidationOffline");
    EXPECT_EQ(kv.value, "false");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, ISO15118CertificateManagementEnabled) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_TRUE(get()->getISO15118CertificateManagementEnabled());
    auto kv = get()->getISO15118CertificateManagementEnabledKeyValue();
    EXPECT_EQ(kv.key, "ISO15118CertificateManagementEnabled");
    EXPECT_EQ(kv.value, "true");
    EXPECT_FALSE(kv.readonly);

    get()->setISO15118CertificateManagementEnabled(false);
    EXPECT_FALSE(get()->getISO15118CertificateManagementEnabled());
    kv = get()->getISO15118CertificateManagementEnabledKeyValue();
    EXPECT_EQ(kv.key, "ISO15118CertificateManagementEnabled");
    EXPECT_EQ(kv.value, "false");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, ISO15118PnCEnabled) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_TRUE(get()->getISO15118PnCEnabled());
    auto kv = get()->getISO15118PnCEnabledKeyValue();
    EXPECT_EQ(kv.key, "ISO15118PnCEnabled");
    EXPECT_EQ(kv.value, "true");
    EXPECT_FALSE(kv.readonly);

    get()->setISO15118PnCEnabled(false);
    EXPECT_FALSE(get()->getISO15118PnCEnabled());
    kv = get()->getISO15118PnCEnabledKeyValue();
    EXPECT_EQ(kv.key, "ISO15118PnCEnabled");
    EXPECT_EQ(kv.value, "false");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, CentralContractValidationAllowed) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getCentralContractValidationAllowed().has_value());
    auto kv = get()->getCentralContractValidationAllowedKeyValue();
    ASSERT_FALSE(kv);

    // needs existing value for set to work
    get()->setCentralContractValidationAllowed(false);
    EXPECT_FALSE(get()->getCentralContractValidationAllowed().has_value());
    kv = get()->getCentralContractValidationAllowedKeyValue();
    ASSERT_FALSE(kv);
}

TEST_P(Configuration, CertSigningRepeatTimes) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getCertSigningRepeatTimes().has_value());
    auto kv = get()->getCertSigningRepeatTimesKeyValue();
    ASSERT_FALSE(kv);

    // needs existing value for set to work
    get()->setCertSigningRepeatTimes(99);
    EXPECT_FALSE(get()->getCertSigningRepeatTimes().has_value());
    kv = get()->getCertSigningRepeatTimesKeyValue();
    ASSERT_FALSE(kv);
}

TEST_P(Configuration, CertSigningWaitMinimum) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getCertSigningWaitMinimum().has_value());
    auto kv = get()->getCertSigningWaitMinimumKeyValue();
    ASSERT_FALSE(kv);

    // needs existing value for set to work
    get()->setCertSigningWaitMinimum(55);
    EXPECT_FALSE(get()->getCertSigningWaitMinimum().has_value());
    kv = get()->getCertSigningWaitMinimumKeyValue();
    ASSERT_FALSE(kv);
}

// -----------------------------------------------------------------------------
// Oneoff tests where there are differences between implementations
// Note: TEST_F and not TEST_P

TEST_F(Configuration, SetCentralContractValidationAllowedV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("PnC", "CentralContractValidationAllowed", "");

    EXPECT_FALSE(v2_config->getCentralContractValidationAllowed().has_value());
    auto kv = v2_config->getCentralContractValidationAllowedKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CentralContractValidationAllowed");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setCentralContractValidationAllowed(false);
    EXPECT_TRUE(v2_config->getCentralContractValidationAllowed().has_value());
    EXPECT_FALSE(v2_config->getCentralContractValidationAllowed().value());
    kv = v2_config->getCentralContractValidationAllowedKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CentralContractValidationAllowed");
    EXPECT_EQ(kv.value().value, "false");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetCertSigningRepeatTimesV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("PnC", "CertSigningRepeatTimes", "");

    EXPECT_FALSE(v2_config->getCertSigningRepeatTimes().has_value());
    auto kv = v2_config->getCertSigningRepeatTimesKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CertSigningRepeatTimes");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setCertSigningRepeatTimes(55);
    EXPECT_TRUE(v2_config->getCertSigningRepeatTimes().has_value());
    EXPECT_EQ(v2_config->getCertSigningRepeatTimes(), 55);
    kv = v2_config->getCertSigningRepeatTimesKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CertSigningRepeatTimes");
    EXPECT_EQ(kv.value().value, "55");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetCertSigningWaitMinimumV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("PnC", "CertSigningWaitMinimum", "");

    EXPECT_FALSE(v2_config->getCertSigningWaitMinimum().has_value());
    auto kv = v2_config->getCertSigningWaitMinimumKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CertSigningWaitMinimum");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setCertSigningWaitMinimum(54);
    EXPECT_TRUE(v2_config->getCertSigningWaitMinimum().has_value());
    EXPECT_EQ(v2_config->getCertSigningWaitMinimum(), 54);
    kv = v2_config->getCertSigningWaitMinimumKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CertSigningWaitMinimum");
    EXPECT_EQ(kv.value().value, "54");
    EXPECT_FALSE(kv.value().readonly);
}

} // namespace
