// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <optional>

#include "configuration_stub.hpp"
#include "ocpp/v16/types.hpp"

namespace {
using namespace ocpp::v16::stubs;

TEST_P(Configuration, ConnectorPhaseRotation) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getConnectorPhaseRotation(), "0.RST,1.RST");
    auto kv = get()->getConnectorPhaseRotationKeyValue();
    EXPECT_EQ(kv.key, "ConnectorPhaseRotation");
    EXPECT_EQ(kv.value, "0.RST,1.RST");
    EXPECT_FALSE(kv.readonly);

    get()->setConnectorPhaseRotation("0.TRS,1.TRS");
    EXPECT_EQ(get()->getConnectorPhaseRotation(), "0.TRS,1.TRS");
    kv = get()->getConnectorPhaseRotationKeyValue();
    EXPECT_EQ(kv.key, "ConnectorPhaseRotation");
    EXPECT_EQ(kv.value, "0.TRS,1.TRS");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, MeterValuesAlignedData) {
    using Measurand = ocpp::v16::Measurand;
    using Phase = ocpp::v16::Phase;

    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getMeterValuesAlignedData(), "Energy.Active.Import.Register");
    auto kv = get()->getMeterValuesAlignedDataKeyValue();
    EXPECT_EQ(kv.key, "MeterValuesAlignedData");
    EXPECT_EQ(kv.value, "Energy.Active.Import.Register");
    EXPECT_FALSE(kv.readonly);

    auto vec = get()->getMeterValuesAlignedDataVector();
    ASSERT_EQ(vec.size(), 4);
    EXPECT_EQ(vec[0].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[0].phase, std::nullopt);
    EXPECT_EQ(vec[1].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[1].phase, Phase::L1);
    EXPECT_EQ(vec[2].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[2].phase, Phase::L2);
    EXPECT_EQ(vec[3].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[3].phase, Phase::L3);

    get()->setMeterValuesAlignedData("Current.Import");
    EXPECT_EQ(get()->getMeterValuesAlignedData(), "Current.Import");
    kv = get()->getMeterValuesAlignedDataKeyValue();
    EXPECT_EQ(kv.key, "MeterValuesAlignedData");
    EXPECT_EQ(kv.value, "Current.Import");
    EXPECT_FALSE(kv.readonly);

    vec = get()->getMeterValuesAlignedDataVector();
    ASSERT_EQ(vec.size(), 5);
    EXPECT_EQ(vec[0].measurand, Measurand::Current_Import);
    EXPECT_EQ(vec[0].phase, std::nullopt);
    EXPECT_EQ(vec[1].measurand, Measurand::Current_Import);
    EXPECT_EQ(vec[1].phase, Phase::L1);
    EXPECT_EQ(vec[2].measurand, Measurand::Current_Import);
    EXPECT_EQ(vec[2].phase, Phase::L2);
    EXPECT_EQ(vec[3].measurand, Measurand::Current_Import);
    EXPECT_EQ(vec[3].phase, Phase::L3);
    EXPECT_EQ(vec[4].measurand, Measurand::Current_Import);
    EXPECT_EQ(vec[4].phase, Phase::N);
}

TEST_P(Configuration, MeterValuesSampledData) {
    using Measurand = ocpp::v16::Measurand;
    using Phase = ocpp::v16::Phase;

    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getMeterValuesSampledData(), "Energy.Active.Import.Register");
    auto kv = get()->getMeterValuesSampledDataKeyValue();
    EXPECT_EQ(kv.key, "MeterValuesSampledData");
    EXPECT_EQ(kv.value, "Energy.Active.Import.Register");
    EXPECT_FALSE(kv.readonly);

    auto vec = get()->getMeterValuesSampledDataVector();
    ASSERT_EQ(vec.size(), 4);
    EXPECT_EQ(vec[0].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[0].phase, std::nullopt);
    EXPECT_EQ(vec[1].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[1].phase, Phase::L1);
    EXPECT_EQ(vec[2].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[2].phase, Phase::L2);
    EXPECT_EQ(vec[3].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[3].phase, Phase::L3);

    get()->setMeterValuesSampledData("Energy.Active.Import.Register,Energy.Active.Export.Register");
    EXPECT_EQ(get()->getMeterValuesSampledData(), "Energy.Active.Import.Register,Energy.Active.Export.Register");
    kv = get()->getMeterValuesSampledDataKeyValue();
    EXPECT_EQ(kv.key, "MeterValuesSampledData");
    EXPECT_EQ(kv.value, "Energy.Active.Import.Register,Energy.Active.Export.Register");
    EXPECT_FALSE(kv.readonly);

    vec = get()->getMeterValuesSampledDataVector();
    ASSERT_EQ(vec.size(), 8);
    EXPECT_EQ(vec[0].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[0].phase, std::nullopt);
    EXPECT_EQ(vec[1].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[1].phase, Phase::L1);
    EXPECT_EQ(vec[2].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[2].phase, Phase::L2);
    EXPECT_EQ(vec[3].measurand, Measurand::Energy_Active_Import_Register);
    EXPECT_EQ(vec[3].phase, Phase::L3);
    EXPECT_EQ(vec[4].measurand, Measurand::Energy_Active_Export_Register);
    EXPECT_EQ(vec[4].phase, std::nullopt);
    EXPECT_EQ(vec[5].measurand, Measurand::Energy_Active_Export_Register);
    EXPECT_EQ(vec[5].phase, Phase::L1);
    EXPECT_EQ(vec[6].measurand, Measurand::Energy_Active_Export_Register);
    EXPECT_EQ(vec[6].phase, Phase::L2);
    EXPECT_EQ(vec[7].measurand, Measurand::Energy_Active_Export_Register);
    EXPECT_EQ(vec[7].phase, Phase::L3);
}

TEST_P(Configuration, StopTxnAlignedData) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getStopTxnAlignedData(), "Energy.Active.Import.Register");
    auto kv = get()->getStopTxnAlignedDataKeyValue();
    EXPECT_EQ(kv.key, "StopTxnAlignedData");
    EXPECT_EQ(kv.value, "Energy.Active.Import.Register");
    EXPECT_FALSE(kv.readonly);

    get()->setStopTxnAlignedData("Voltage");
    EXPECT_EQ(get()->getStopTxnAlignedData(), "Voltage");
    kv = get()->getStopTxnAlignedDataKeyValue();
    EXPECT_EQ(kv.key, "StopTxnAlignedData");
    EXPECT_EQ(kv.value, "Voltage");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, StopTxnSampledData) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getStopTxnSampledData(), "Energy.Active.Import.Register");
    auto kv = get()->getStopTxnSampledDataKeyValue();
    EXPECT_EQ(kv.key, "StopTxnSampledData");
    EXPECT_EQ(kv.value, "Energy.Active.Import.Register");
    EXPECT_FALSE(kv.readonly);

    get()->setStopTxnSampledData("Frequency,Current.Offered");
    EXPECT_EQ(get()->getStopTxnSampledData(), "Frequency,Current.Offered");
    kv = get()->getStopTxnSampledDataKeyValue();
    EXPECT_EQ(kv.key, "StopTxnSampledData");
    EXPECT_EQ(kv.value, "Frequency,Current.Offered");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, SupportedFeatureProfiles) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getSupportedFeatureProfiles(),
              "Core,FirmwareManagement,RemoteTrigger,Reservation,LocalAuthListManagement,SmartCharging");
    auto kv = get()->getSupportedFeatureProfilesKeyValue();
    EXPECT_EQ(kv.key, "SupportedFeatureProfiles");
    EXPECT_EQ(kv.value, "Core,FirmwareManagement,RemoteTrigger,Reservation,LocalAuthListManagement,SmartCharging");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, AuthorizeRemoteTxRequests) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getAuthorizeRemoteTxRequests(), false);
    auto kv = get()->getAuthorizeRemoteTxRequestsKeyValue();
    EXPECT_EQ(kv.key, "AuthorizeRemoteTxRequests");
    EXPECT_EQ(kv.value, "false");
    EXPECT_FALSE(kv.readonly);

    get()->setAuthorizeRemoteTxRequests(true);
    EXPECT_EQ(get()->getAuthorizeRemoteTxRequests(), true);
    kv = get()->getAuthorizeRemoteTxRequestsKeyValue();
    EXPECT_EQ(kv.key, "AuthorizeRemoteTxRequests");
    EXPECT_EQ(kv.value, "true");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, LocalAuthorizeOffline) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getLocalAuthorizeOffline(), false);
    auto kv = get()->getLocalAuthorizeOfflineKeyValue();
    EXPECT_EQ(kv.key, "LocalAuthorizeOffline");
    EXPECT_EQ(kv.value, "false");
    EXPECT_FALSE(kv.readonly);

    get()->setLocalAuthorizeOffline(true);
    EXPECT_EQ(get()->getLocalAuthorizeOffline(), true);
    kv = get()->getLocalAuthorizeOfflineKeyValue();
    EXPECT_EQ(kv.key, "LocalAuthorizeOffline");
    EXPECT_EQ(kv.value, "true");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, LocalPreAuthorize) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getLocalPreAuthorize(), false);
    auto kv = get()->getLocalPreAuthorizeKeyValue();
    EXPECT_EQ(kv.key, "LocalPreAuthorize");
    EXPECT_EQ(kv.value, "false");
    EXPECT_FALSE(kv.readonly);

    get()->setLocalPreAuthorize(true);
    EXPECT_EQ(get()->getLocalPreAuthorize(), true);
    kv = get()->getLocalPreAuthorizeKeyValue();
    EXPECT_EQ(kv.key, "LocalPreAuthorize");
    EXPECT_EQ(kv.value, "true");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, StopTransactionOnInvalidId) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getStopTransactionOnInvalidId(), true);
    auto kv = get()->getStopTransactionOnInvalidIdKeyValue();
    EXPECT_EQ(kv.key, "StopTransactionOnInvalidId");
    EXPECT_EQ(kv.value, "true");
    EXPECT_FALSE(kv.readonly);

    get()->setStopTransactionOnInvalidId(false);
    EXPECT_EQ(get()->getStopTransactionOnInvalidId(), false);
    kv = get()->getStopTransactionOnInvalidIdKeyValue();
    EXPECT_EQ(kv.key, "StopTransactionOnInvalidId");
    EXPECT_EQ(kv.value, "false");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, UnlockConnectorOnEVSideDisconnect) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getUnlockConnectorOnEVSideDisconnect(), true);
    auto kv = get()->getUnlockConnectorOnEVSideDisconnectKeyValue();
    EXPECT_EQ(kv.key, "UnlockConnectorOnEVSideDisconnect");
    EXPECT_EQ(kv.value, "true");
    EXPECT_FALSE(kv.readonly);

    get()->setUnlockConnectorOnEVSideDisconnect(false);
    EXPECT_EQ(get()->getUnlockConnectorOnEVSideDisconnect(), false);
    kv = get()->getUnlockConnectorOnEVSideDisconnectKeyValue();
    EXPECT_EQ(kv.key, "UnlockConnectorOnEVSideDisconnect");
    EXPECT_EQ(kv.value, "false");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, ClockAlignedDataInterval) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getClockAlignedDataInterval(), 900);
    auto kv = get()->getClockAlignedDataIntervalKeyValue();
    EXPECT_EQ(kv.key, "ClockAlignedDataInterval");
    EXPECT_EQ(kv.value, "900");
    EXPECT_FALSE(kv.readonly);

    get()->setClockAlignedDataInterval(5200);
    EXPECT_EQ(get()->getClockAlignedDataInterval(), 5200);
    kv = get()->getClockAlignedDataIntervalKeyValue();
    EXPECT_EQ(kv.key, "ClockAlignedDataInterval");
    EXPECT_EQ(kv.value, "5200");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, ConnectionTimeOut) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getConnectionTimeOut(), 10);
    auto kv = get()->getConnectionTimeOutKeyValue();
    EXPECT_EQ(kv.key, "ConnectionTimeOut");
    EXPECT_EQ(kv.value, "10");
    EXPECT_FALSE(kv.readonly);

    get()->setConnectionTimeOut(60);
    EXPECT_EQ(get()->getConnectionTimeOut(), 60);
    kv = get()->getConnectionTimeOutKeyValue();
    EXPECT_EQ(kv.key, "ConnectionTimeOut");
    EXPECT_EQ(kv.value, "60");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, GetConfigurationMaxKeys) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getGetConfigurationMaxKeys(), 100);
    auto kv = get()->getGetConfigurationMaxKeysKeyValue();
    EXPECT_EQ(kv.key, "GetConfigurationMaxKeys");
    EXPECT_EQ(kv.value, "100");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, HeartbeatInterval) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getHeartbeatInterval(), 86400);
    auto kv = get()->getHeartbeatIntervalKeyValue();
    EXPECT_EQ(kv.key, "HeartbeatInterval");
    EXPECT_EQ(kv.value, "86400");
    EXPECT_FALSE(kv.readonly);

    get()->setHeartbeatInterval(70);
    EXPECT_EQ(get()->getHeartbeatInterval(), 70);
    kv = get()->getHeartbeatIntervalKeyValue();
    EXPECT_EQ(kv.key, "HeartbeatInterval");
    EXPECT_EQ(kv.value, "70");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, MeterValueSampleInterval) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getMeterValueSampleInterval(), 0);
    auto kv = get()->getMeterValueSampleIntervalKeyValue();
    EXPECT_EQ(kv.key, "MeterValueSampleInterval");
    EXPECT_EQ(kv.value, "0");
    EXPECT_FALSE(kv.readonly);

    get()->setMeterValueSampleInterval(125);
    EXPECT_EQ(get()->getMeterValueSampleInterval(), 125);
    kv = get()->getMeterValueSampleIntervalKeyValue();
    EXPECT_EQ(kv.key, "MeterValueSampleInterval");
    EXPECT_EQ(kv.value, "125");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, NumberOfConnectors) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getNumberOfConnectors(), 1);
    auto kv = get()->getNumberOfConnectorsKeyValue();
    EXPECT_EQ(kv.key, "NumberOfConnectors");
    EXPECT_EQ(kv.value, "1");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, ResetRetries) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getResetRetries(), 1);
    auto kv = get()->getResetRetriesKeyValue();
    EXPECT_EQ(kv.key, "ResetRetries");
    EXPECT_EQ(kv.value, "1");
    EXPECT_FALSE(kv.readonly);

    get()->setResetRetries(7);
    EXPECT_EQ(get()->getResetRetries(), 7);
    kv = get()->getResetRetriesKeyValue();
    EXPECT_EQ(kv.key, "ResetRetries");
    EXPECT_EQ(kv.value, "7");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, TransactionMessageAttempts) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getTransactionMessageAttempts(), 1);
    auto kv = get()->getTransactionMessageAttemptsKeyValue();
    EXPECT_EQ(kv.key, "TransactionMessageAttempts");
    EXPECT_EQ(kv.value, "1");
    EXPECT_FALSE(kv.readonly);

    get()->setTransactionMessageAttempts(4);
    EXPECT_EQ(get()->getTransactionMessageAttempts(), 4);
    kv = get()->getTransactionMessageAttemptsKeyValue();
    EXPECT_EQ(kv.key, "TransactionMessageAttempts");
    EXPECT_EQ(kv.value, "4");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, TransactionMessageRetryInterval) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getTransactionMessageRetryInterval(), 10);
    auto kv = get()->getTransactionMessageRetryIntervalKeyValue();
    EXPECT_EQ(kv.key, "TransactionMessageRetryInterval");
    EXPECT_EQ(kv.value, "10");
    EXPECT_FALSE(kv.readonly);

    get()->setTransactionMessageRetryInterval(1250);
    EXPECT_EQ(get()->getTransactionMessageRetryInterval(), 1250);
    kv = get()->getTransactionMessageRetryIntervalKeyValue();
    EXPECT_EQ(kv.key, "TransactionMessageRetryInterval");
    EXPECT_EQ(kv.value, "1250");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, AllowOfflineTxForUnknownId) {
    ASSERT_NE(get(), nullptr);
    // No initial value set - hence set() doesn't work

    EXPECT_FALSE(get()->getAllowOfflineTxForUnknownId().has_value());
    auto kv = get()->getAllowOfflineTxForUnknownIdKeyValue();
    ASSERT_FALSE(kv.has_value());

    // set only works when there is a value configured
    get()->setAllowOfflineTxForUnknownId(true);
    EXPECT_FALSE(get()->getAllowOfflineTxForUnknownId().has_value());
    kv = get()->getAllowOfflineTxForUnknownIdKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, AuthorizationCacheEnabled) {
    ASSERT_NE(get(), nullptr);
    // No initial value set - hence set() doesn't work

    EXPECT_FALSE(get()->getAuthorizationCacheEnabled().has_value());
    auto kv = get()->getAuthorizationCacheEnabledKeyValue();
    ASSERT_FALSE(kv.has_value());

    // set only works when there is a value configured
    get()->setAuthorizationCacheEnabled(true);
    EXPECT_FALSE(get()->getAuthorizationCacheEnabled().has_value());
    kv = get()->getAuthorizationCacheEnabledKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, ReserveConnectorZeroSupported) {
    ASSERT_NE(get(), nullptr);
    EXPECT_FALSE(get()->getReserveConnectorZeroSupported().has_value());
    auto kv = get()->getReserveConnectorZeroSupportedKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, StopTransactionOnEVSideDisconnect) {
    ASSERT_NE(get(), nullptr);

    EXPECT_TRUE(get()->getStopTransactionOnEVSideDisconnect().has_value());
    EXPECT_TRUE(get()->getStopTransactionOnEVSideDisconnect().value());
    auto kv = get()->getStopTransactionOnEVSideDisconnectKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "StopTransactionOnEVSideDisconnect");
    EXPECT_EQ(kv.value().value, "true");
    EXPECT_TRUE(kv.value().readonly);
}

TEST_P(Configuration, ConnectorPhaseRotationMaxLength) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getConnectorPhaseRotationMaxLength(), std::nullopt);
    auto kv = get()->getConnectorPhaseRotationMaxLengthKeyValue();
    EXPECT_EQ(kv, std::nullopt);
}

TEST_P(Configuration, LightIntensity) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getLightIntensity(), std::nullopt);
    auto kv = get()->getLightIntensityKeyValue();
    EXPECT_EQ(kv, std::nullopt);

    // set only works when there is a value configured
    get()->setLightIntensity(777);
    EXPECT_EQ(get()->getLightIntensity(), std::nullopt);
    kv = get()->getLightIntensityKeyValue();
    EXPECT_EQ(kv, std::nullopt);
}

TEST_P(Configuration, MaxEnergyOnInvalidId) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getMaxEnergyOnInvalidId(), std::nullopt);
    auto kv = get()->getMaxEnergyOnInvalidIdKeyValue();
    EXPECT_EQ(kv, std::nullopt);

    // set only works when there is a value configured
    get()->setMaxEnergyOnInvalidId(770);
    EXPECT_EQ(get()->getMaxEnergyOnInvalidId(), std::nullopt);
    kv = get()->getMaxEnergyOnInvalidIdKeyValue();
    EXPECT_EQ(kv, std::nullopt);
}

TEST_P(Configuration, MeterValuesAlignedDataMaxLength) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getMeterValuesAlignedDataMaxLength(), std::nullopt);
    auto kv = get()->getMeterValuesAlignedDataMaxLengthKeyValue();
    EXPECT_EQ(kv, std::nullopt);
}

TEST_P(Configuration, MeterValuesSampledDataMaxLength) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getMeterValuesSampledDataMaxLength(), std::nullopt);
    auto kv = get()->getMeterValuesSampledDataMaxLengthKeyValue();
    EXPECT_EQ(kv, std::nullopt);
}

TEST_P(Configuration, MinimumStatusDuration) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getMinimumStatusDuration(), std::nullopt);
    auto kv = get()->getMinimumStatusDurationKeyValue();
    EXPECT_EQ(kv, std::nullopt);

    // set only works when there is a value configured
    get()->setMinimumStatusDuration(760);
    EXPECT_EQ(get()->getMinimumStatusDuration(), std::nullopt);
    kv = get()->getMinimumStatusDurationKeyValue();
    EXPECT_EQ(kv, std::nullopt);
}

TEST_P(Configuration, StopTxnAlignedDataMaxLength) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getStopTxnAlignedDataMaxLength(), std::nullopt);
    auto kv = get()->getStopTxnAlignedDataMaxLengthKeyValue();
    EXPECT_EQ(kv, std::nullopt);
}

TEST_P(Configuration, StopTxnSampledDataMaxLength) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getStopTxnSampledDataMaxLength(), std::nullopt);
    auto kv = get()->getStopTxnSampledDataMaxLengthKeyValue();
    EXPECT_EQ(kv, std::nullopt);
}

TEST_P(Configuration, SupportedFeatureProfilesMaxLength) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getSupportedFeatureProfilesMaxLength(), std::nullopt);
    auto kv = get()->getSupportedFeatureProfilesMaxLengthKeyValue();
    EXPECT_EQ(kv, std::nullopt);
}

TEST_P(Configuration, WebsocketPingInterval) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getWebsocketPingInterval(), std::nullopt);
    auto kv = get()->getWebsocketPingIntervalKeyValue();
    EXPECT_EQ(kv, std::nullopt);

    // set only works when there is a value configured
    get()->setWebsocketPingInterval(707);
    EXPECT_EQ(get()->getWebsocketPingInterval(), std::nullopt);
    kv = get()->getWebsocketPingIntervalKeyValue();
    EXPECT_EQ(kv, std::nullopt);
}

TEST_P(Configuration, SupportedFeatureProfilesSet) {
    using SupportedFeatureProfiles = ocpp::v16::SupportedFeatureProfiles;

    ASSERT_NE(get(), nullptr);
    const auto set = get()->getSupportedFeatureProfilesSet();

    const std::set<SupportedFeatureProfiles> expected = {SupportedFeatureProfiles::Internal,
                                                         SupportedFeatureProfiles::Core,
                                                         SupportedFeatureProfiles::CostAndPrice,
                                                         SupportedFeatureProfiles::FirmwareManagement,
                                                         SupportedFeatureProfiles::LocalAuthListManagement,
                                                         SupportedFeatureProfiles::Reservation,
                                                         SupportedFeatureProfiles::SmartCharging,
                                                         SupportedFeatureProfiles::RemoteTrigger,
                                                         SupportedFeatureProfiles::Security,
                                                         SupportedFeatureProfiles::PnC};

    EXPECT_EQ(set.size(), expected.size());
    EXPECT_EQ(set, expected);
}

// -----------------------------------------------------------------------------
// Oneoff tests where there are differences between implementations
// Note: TEST_F and not TEST_P

TEST_F(Configuration, SetAllowOfflineTxForUnknownIdV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "AllowOfflineTxForUnknownId", "");

    EXPECT_FALSE(v2_config->getAllowOfflineTxForUnknownId().has_value());
    auto kv = v2_config->getAllowOfflineTxForUnknownIdKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "AllowOfflineTxForUnknownId");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setAllowOfflineTxForUnknownId(true);
    EXPECT_TRUE(v2_config->getAllowOfflineTxForUnknownId().has_value());
    EXPECT_EQ(v2_config->getAllowOfflineTxForUnknownId().value(), true);
    kv = v2_config->getAllowOfflineTxForUnknownIdKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "AllowOfflineTxForUnknownId");
    EXPECT_EQ(kv.value().value, "true");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetAuthorizationCacheEnabledV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "AuthorizationCacheEnabled", "");

    EXPECT_FALSE(v2_config->getAuthorizationCacheEnabled().has_value());
    auto kv = v2_config->getAuthorizationCacheEnabledKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "AuthorizationCacheEnabled");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setAuthorizationCacheEnabled(true);
    EXPECT_TRUE(v2_config->getAuthorizationCacheEnabled().has_value());
    EXPECT_EQ(v2_config->getAuthorizationCacheEnabled().value(), true);
    kv = v2_config->getAuthorizationCacheEnabledKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "AuthorizationCacheEnabled");
    EXPECT_EQ(kv.value().value, "true");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetBlinkRepeatV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "BlinkRepeat", "");

    EXPECT_EQ(v2_config->getBlinkRepeat(), std::nullopt);
    auto kv = v2_config->getBlinkRepeatKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "BlinkRepeat");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setBlinkRepeat(31);
    EXPECT_EQ(v2_config->getBlinkRepeat(), 31);
    kv = v2_config->getBlinkRepeatKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "BlinkRepeat");
    EXPECT_EQ(kv.value().value, "31");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetConnectorPhaseRotationMaxLengthV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "ConnectorPhaseRotationMaxLength", "200");

    EXPECT_EQ(v2_config->getConnectorPhaseRotationMaxLength(), 200);
    auto kv = v2_config->getConnectorPhaseRotationMaxLengthKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "ConnectorPhaseRotationMaxLength");
    EXPECT_EQ(kv.value().value, "200");
    EXPECT_TRUE(kv.value().readonly);
}

TEST_F(Configuration, SetLightIntensityV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "LightIntensity", "");

    EXPECT_EQ(v2_config->getLightIntensity(), std::nullopt);
    auto kv = v2_config->getLightIntensityKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "LightIntensity");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setLightIntensity(776);
    EXPECT_EQ(v2_config->getLightIntensity(), 776);
    kv = v2_config->getLightIntensityKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "LightIntensity");
    EXPECT_EQ(kv.value().value, "776");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetMaxEnergyOnInvalidIdV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "MaxEnergyOnInvalidId", "");

    EXPECT_EQ(v2_config->getMaxEnergyOnInvalidId(), std::nullopt);
    auto kv = v2_config->getMaxEnergyOnInvalidIdKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "MaxEnergyOnInvalidId");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setMaxEnergyOnInvalidId(770);
    EXPECT_EQ(v2_config->getMaxEnergyOnInvalidId(), 770);
    kv = v2_config->getMaxEnergyOnInvalidIdKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "MaxEnergyOnInvalidId");
    EXPECT_EQ(kv.value().value, "770");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetMeterValuesAlignedDataMaxLengthV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "MeterValuesAlignedDataMaxLength", "199");

    EXPECT_EQ(v2_config->getMeterValuesAlignedDataMaxLength(), 199);
    auto kv = v2_config->getMeterValuesAlignedDataMaxLengthKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "MeterValuesAlignedDataMaxLength");
    EXPECT_EQ(kv.value().value, "199");
    EXPECT_TRUE(kv.value().readonly);
}

TEST_F(Configuration, SetMeterValuesSampledDataMaxLengthV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "MeterValuesSampledDataMaxLength", "198");

    EXPECT_EQ(v2_config->getMeterValuesSampledDataMaxLength(), 198);
    auto kv = v2_config->getMeterValuesSampledDataMaxLengthKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "MeterValuesSampledDataMaxLength");
    EXPECT_EQ(kv.value().value, "198");
    EXPECT_TRUE(kv.value().readonly);
}

TEST_F(Configuration, SetMinimumStatusDurationV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "MinimumStatusDuration", "");

    EXPECT_EQ(v2_config->getMinimumStatusDuration(), std::nullopt);
    auto kv = v2_config->getMinimumStatusDurationKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "MinimumStatusDuration");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setMinimumStatusDuration(760);
    EXPECT_EQ(v2_config->getMinimumStatusDuration(), 760);
    kv = v2_config->getMinimumStatusDurationKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "MinimumStatusDuration");
    EXPECT_EQ(kv.value().value, "760");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetStopTxnAlignedDataMaxLengthV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "StopTxnAlignedDataMaxLength", "83");

    EXPECT_EQ(v2_config->getStopTxnAlignedDataMaxLength(), 83);
    auto kv = v2_config->getStopTxnAlignedDataMaxLengthKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "StopTxnAlignedDataMaxLength");
    EXPECT_EQ(kv.value().value, "83");
    EXPECT_TRUE(kv.value().readonly);
}

TEST_F(Configuration, SetStopTxnSampledDataMaxLengthV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "StopTxnSampledDataMaxLength", "84");

    EXPECT_EQ(v2_config->getStopTxnSampledDataMaxLength(), 84);
    auto kv = v2_config->getStopTxnSampledDataMaxLengthKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "StopTxnSampledDataMaxLength");
    EXPECT_EQ(kv.value().value, "84");
    EXPECT_TRUE(kv.value().readonly);
}

TEST_F(Configuration, SetSupportedFeatureProfilesMaxLengthV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "SupportedFeatureProfilesMaxLength", "85");

    EXPECT_EQ(v2_config->getSupportedFeatureProfilesMaxLength(), 85);
    auto kv = v2_config->getSupportedFeatureProfilesMaxLengthKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "SupportedFeatureProfilesMaxLength");
    EXPECT_EQ(kv.value().value, "85");
    EXPECT_TRUE(kv.value().readonly);
}

TEST_F(Configuration, SetWebsocketPingIntervalV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Core", "WebSocketPingInterval", "");

    EXPECT_EQ(v2_config->getWebsocketPingInterval(), std::nullopt);
    auto kv = v2_config->getWebsocketPingIntervalKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "WebsocketPingInterval");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setWebsocketPingInterval(707);
    EXPECT_EQ(v2_config->getWebsocketPingInterval(), 707);
    kv = v2_config->getWebsocketPingIntervalKeyValue();
    ASSERT_TRUE(kv);
    EXPECT_EQ(kv.value().key, "WebsocketPingInterval");
    EXPECT_EQ(kv.value().value, "707");
    EXPECT_FALSE(kv.value().readonly);
}

} // namespace
