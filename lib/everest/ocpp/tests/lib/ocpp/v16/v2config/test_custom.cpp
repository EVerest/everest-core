// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <optional>

#include "configuration_stub.hpp"

namespace {
using namespace ocpp::v16::stubs;

// expected values extracted from the JSON configuration files
// also see memory_storage.cpp
const std::map<std::string, std::string> expected_key_value = {
    {"AuthorizeConnectorZeroOnConnectorOne", "true"},
    {"CentralSystemURI", "127.0.0.1:8180/steve/websocket/CentralSystemService/"},
    {"ChargeBoxSerialNumber", "cp001"},
    {"ChargePointId", "cp001"},
    {"ChargePointModel", "Yeti"},
    {"ChargePointVendor", "Pionix"},
    {"CompositeScheduleDefaultLimitAmps", "48"},
    {"CompositeScheduleDefaultLimitWatts", "33120"},
    {"CompositeScheduleDefaultNumberPhases", "3"},
    {"FirmwareVersion", "0.1"},
    {"LogMessages", "true"},
    {"LogMessagesFormat", ""},
    {"LogMessagesRaw", "false"},
    {"MaxCompositeScheduleDuration", "31536000"},
    {"MaxMessageSize", "65000"},
    {"OcspRequestInterval", "604800"},
    {"RetryBackoffRandomRange", "10"},
    {"RetryBackoffRepeatTimes", "3"},
    {"RetryBackoffWaitMinimum", "3"},
    {"StopTransactionIfUnlockNotSupported", "false"},
    {"SupplyVoltage", "230"},
    {"SupportedChargingProfilePurposeTypes", "ChargePointMaxProfile,TxDefaultProfile,TxProfile"},
    {"SupportedCiphers12",
     "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:AES128-GCM-SHA256:AES256-GCM-SHA384"},
    {"SupportedCiphers13", "TLS_AES_256_GCM_SHA384:TLS_AES_128_GCM_SHA256"},
    {"SupportedMeasurands", "Energy.Active.Import.Register,Energy.Active.Export.Register,Power.Active.Import,Voltage,"
                            "Current.Import,Frequency,Current.Offered,Power.Offered,SoC,Temperature"},
    {"UseSslDefaultVerifyPaths", "true"},
    {"VerifyCsmsAllowWildcards", "false"},
    {"VerifyCsmsCommonName", "true"},
    {"WaitForStopTransactionsOnResetTimeout", "60"},
    {"WebsocketPingPayload", "hello there"},
    {"WebsocketPongTimeout", "5"},
    {"AuthorizeRemoteTxRequests", "false"},
    {"ClockAlignedDataInterval", "900"},
    {"ConnectionTimeOut", "10"},
    {"ConnectorPhaseRotation", "0.RST,1.RST"},
    {"GetConfigurationMaxKeys", "100"},
    {"HeartbeatInterval", "86400"},
    {"LocalAuthorizeOffline", "false"},
    {"LocalPreAuthorize", "false"},
    {"MeterValueSampleInterval", "0"},
    {"MeterValuesAlignedData", "Energy.Active.Import.Register"},
    {"MeterValuesSampledData", "Energy.Active.Import.Register"},
    {"NumberOfConnectors", "1"},
    {"ResetRetries", "1"},
    {"StopTransactionOnEVSideDisconnect", "true"},
    {"StopTransactionOnInvalidId", "true"},
    {"StopTxnAlignedData", "Energy.Active.Import.Register"},
    {"StopTxnSampledData", "Energy.Active.Import.Register"},
    {"SupportedFeatureProfiles",
     "Core,FirmwareManagement,RemoteTrigger,Reservation,LocalAuthListManagement,SmartCharging"},
    {"TransactionMessageAttempts", "1"},
    {"TransactionMessageRetryInterval", "10"},
    {"UnlockConnectorOnEVSideDisconnect", "true"},
    {"CustomDisplayCostAndPrice", "false"},
    {"SupportedFileTransferProtocols", "FTP"},
    {"LocalAuthListEnabled", "true"},
    {"LocalAuthListMaxLength", "42"},
    {"SendLocalListMaxLength", "42"},
    {"ChargeProfileMaxStackLevel", "42"},
    {"ChargingScheduleAllowedChargingRateUnit", "Current"},
    {"ChargingScheduleMaxPeriods", "42"},
    {"MaxChargingProfilesInstalled", "42"},
    {"DisableSecurityEventNotifications", "false"},
    {"SecurityProfile", "0"},
    {"ContractValidationOffline", "true"},
    {"ISO15118CertificateManagementEnabled", "true"},
    {"ISO15118PnCEnabled", "true"},
    {"UseTPM", "false"},
    {"UseTPMSeccLeafCertificate", "false"},
    {"LogRotationMaximumFileCount", "0"},
    {"LogRotationMaximumFileSize", "0"},
    {"TLSKeylogFile", "/tmp/ocpp_tls_keylog.txt"},
    {"LogRotation", "false"},
    {"LogRotationDateSuffix", "false"},
    {"EnableTLSKeylog", "false"},
};

TEST_P(Configuration, CustomKey) {
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;
    ASSERT_NE(get(), nullptr);

    const std::string key{"GTCustom"};
    const std::string value{"GTCustomValue"};

    EXPECT_FALSE(get()->getCustomKeyValue(key).has_value());
    EXPECT_EQ(get()->setCustomKey(key, value, false), ConfigurationStatus::Rejected);

    // no point in testing setCustomKey(key, value, true)
    // since force==true still requires that the key exists
    // in only allows read-only keys to be changed
}

TEST_P(Configuration, Get) {
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;
    ASSERT_NE(get(), nullptr);

    // non-existent key
    EXPECT_FALSE(get()->get("DoesNotExist").has_value());

    // read-only key
    auto kv = get()->get("ChargePointModel");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ChargePointModel");
    EXPECT_EQ(kv.value().value, "Yeti");
    EXPECT_TRUE(kv.value().readonly);

    // read-write key

    kv = get()->get("ClockAlignedDataInterval");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ClockAlignedDataInterval");
    EXPECT_EQ(kv.value().value, "900");
    EXPECT_FALSE(kv.value().readonly);

    // check key exists and has a value
    EXPECT_EQ(get()->getTLSKeylogFile(), "/tmp/ocpp_tls_keylog.txt");

    // check it is available via this call (read-only)
    kv = get()->get("TLSKeylogFile");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "TLSKeylogFile");
    EXPECT_EQ(kv.value().value, "/tmp/ocpp_tls_keylog.txt");
    EXPECT_TRUE(kv.value().readonly);

    // custom key (none defined)
}

TEST_P(Configuration, Set) {
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;
    ASSERT_NE(get(), nullptr);

    // non-existent key
    EXPECT_FALSE(get()->get("DoesNotExist").has_value());
    EXPECT_EQ(get()->set("DoesNotExist", "ToThisValue"), std::nullopt);

    // read-only key
    auto kv = get()->get("ChargePointModel");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ChargePointModel");
    EXPECT_EQ(kv.value().value, "Yeti");
    EXPECT_TRUE(kv.value().readonly);

    EXPECT_EQ(get()->set("ChargePointModel", "ToThisValue"), std::nullopt);
    kv = get()->get("ChargePointModel");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ChargePointModel");
    EXPECT_EQ(kv.value().value, "Yeti");
    EXPECT_TRUE(kv.value().readonly);

    // some other examples
    EXPECT_EQ(get()->set("ChargePointSerialNumber", "<won't be set>"), std::nullopt);
    EXPECT_EQ(get()->set("ICCID", "<won't be set>"), std::nullopt);
    EXPECT_EQ(get()->set("ConnectorPhaseRotationMaxLength", "<won't be set>"), std::nullopt);
    EXPECT_EQ(get()->set("NumberOfConnectors", "<won't be set>"), std::nullopt);
    EXPECT_EQ(get()->set("MeterType", "<won't be set>"), std::nullopt);
    EXPECT_EQ(get()->set("UseSslDefaultVerifyPaths", "<won't be set>"), std::nullopt);
    EXPECT_EQ(get()->set("CertificateStoreMaxLength", "<won't be set>"), std::nullopt);

    // read-write key
    kv = get()->get("ClockAlignedDataInterval");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ClockAlignedDataInterval");
    EXPECT_EQ(kv.value().value, "900");
    EXPECT_FALSE(kv.value().readonly);

    EXPECT_EQ(get()->set("ClockAlignedDataInterval", "1201"), ConfigurationStatus::Accepted);
    kv = get()->get("ClockAlignedDataInterval");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ClockAlignedDataInterval");
    EXPECT_EQ(kv.value().value, "1201");
    EXPECT_FALSE(kv.value().readonly);

    // check if read-only key exists and has a value
    EXPECT_EQ(get()->getTLSKeylogFile(), "/tmp/ocpp_tls_keylog.txt");
    kv = get()->get("TLSKeylogFile");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "TLSKeylogFile");
    EXPECT_EQ(kv.value().value, "/tmp/ocpp_tls_keylog.txt");
    EXPECT_TRUE(kv.value().readonly);
    EXPECT_EQ(get()->set("TLSKeylogFile", "1201"), std::nullopt);
    EXPECT_EQ(get()->getTLSKeylogFile(), "/tmp/ocpp_tls_keylog.txt");

    // custom key (none defined)
}

TEST_P(Configuration, GetAllKeyValue) {
    ASSERT_NE(get(), nullptr);

    const auto values = get()->get_all_key_value();
    EXPECT_EQ(values.size(), expected_key_value.size());

    std::map<std::string, std::string> not_found;
    std::map<std::string, std::string> missing = expected_key_value;

    for (const auto& i : values) {
        if (const auto& search = expected_key_value.find(i.key); search == expected_key_value.end()) {
            not_found.insert({i.key, i.value.value_or("")});
        } else {
            missing.erase(i.key);
            std::string actual{i.value.value_or("")};
            SCOPED_TRACE("Name: " + std::string{i.key});
            EXPECT_EQ(search->second, actual);
        }
    }

    EXPECT_TRUE(not_found.empty());
    if (!not_found.empty()) {
        std::cout << "Not found:\n";
        for (const auto& i : not_found) {
            std::cout << "{\"" << i.first << "\",\"" << i.second << "\"},\n";
        }
    }

    EXPECT_TRUE(missing.empty());
    if (!missing.empty()) {
        std::cout << "Missing:\n";
        for (const auto& i : missing) {
            std::cout << "{\"" << i.first << "\",\"" << i.second << "\"},\n";
        }
    }
}

// -----------------------------------------------------------------------------
// Oneoff tests where there are differences between implementations
// Note: TEST_F and not TEST_P

TEST_F(Configuration, setCustomKeyV2) {
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;
    ASSERT_TRUE(device_model);

    const std::string key{"GTCustom"};
    const std::string value{"GTCustomValue"};

    // set an initial value
    device_model->set("Custom", key, "");

    EXPECT_TRUE(v2_config->getCustomKeyValue(key).has_value());
    auto kv = v2_config->getCustomKeyValue(key);
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, key.c_str());
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    EXPECT_EQ(v2_config->setCustomKey(key, value, false), ConfigurationStatus::Accepted);
    kv = v2_config->getCustomKeyValue(key);
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, key.c_str());
    EXPECT_EQ(kv.value().value, value.c_str());
    EXPECT_FALSE(kv.value().readonly);

    // TODO: test that force=true allows update of a read-only key
    // and force=false rejects update.
}

TEST_F(Configuration, SetV2) {
    using ConfigurationStatus = ocpp::v16::ConfigurationStatus;
    ASSERT_TRUE(device_model);

    // set an initial custom key value
    device_model->set("Custom", "ACustomKey", "");
    device_model->set("Custom", "ACustomRWKey", "");
    device_model->set_readonly("ACustomKey");

    // non-existent key
    EXPECT_FALSE(v2_config->get("DoesNotExist").has_value());
    EXPECT_EQ(v2_config->set("DoesNotExist", "ToThisValue"), std::nullopt);

    // read-only key
    auto kv = v2_config->get("ChargePointModel");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ChargePointModel");
    EXPECT_EQ(kv.value().value, "Yeti");
    EXPECT_TRUE(kv.value().readonly);

    EXPECT_EQ(v2_config->set("ChargePointModel", "ToThisValue"), std::nullopt);
    kv = v2_config->get("ChargePointModel");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ChargePointModel");
    EXPECT_EQ(kv.value().value, "Yeti");
    EXPECT_TRUE(kv.value().readonly);

    // read-write key
    kv = v2_config->get("ClockAlignedDataInterval");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ClockAlignedDataInterval");
    EXPECT_EQ(kv.value().value, "900");
    EXPECT_FALSE(kv.value().readonly);

    EXPECT_EQ(v2_config->set("ClockAlignedDataInterval", "1201"), ConfigurationStatus::Accepted);
    kv = v2_config->get("ClockAlignedDataInterval");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ClockAlignedDataInterval");
    EXPECT_EQ(kv.value().value, "1201");
    EXPECT_FALSE(kv.value().readonly);

    // custom key (read only)
    kv = v2_config->get("ACustomKey");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ACustomKey");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_TRUE(kv.value().readonly);
    EXPECT_EQ(v2_config->set("ACustomKey", "ToThisValueToo"), std::nullopt);
    kv = v2_config->get("ACustomKey");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ACustomKey");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_TRUE(kv.value().readonly);

    // custom key (read write)
    kv = v2_config->get("ACustomRWKey");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ACustomRWKey");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);
    EXPECT_EQ(v2_config->set("ACustomRWKey", "ToThisValueTooMore"), ConfigurationStatus::Accepted);
    kv = v2_config->get("ACustomRWKey");
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ACustomRWKey");
    EXPECT_EQ(kv.value().value, "ToThisValueTooMore");
    EXPECT_FALSE(kv.value().readonly);
}

} // namespace
