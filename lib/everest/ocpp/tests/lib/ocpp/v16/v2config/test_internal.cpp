// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <cstdint>
#include <gtest/gtest.h>
#include <optional>
#include <string>

#include "configuration_stub.hpp"
#include "ocpp/v16/ocpp_enums.hpp"
#include "ocpp/v16/types.hpp"

namespace {
using namespace ocpp::v16::stubs;

// clang-format off
#define FOR_ALL_SupportedMessageTypesSending(apply) \
    apply(Authorize) \
    apply(BootNotification) \
    apply(CancelReservationResponse) \
    apply(CertificateSignedResponse) \
    apply(ChangeAvailabilityResponse) \
    apply(ChangeConfigurationResponse) \
    apply(ClearCacheResponse) \
    apply(ClearChargingProfileResponse) \
    apply(DataTransfer) \
    apply(DataTransferResponse) \
    apply(DeleteCertificateResponse) \
    apply(DiagnosticsStatusNotification) \
    apply(ExtendedTriggerMessageResponse) \
    apply(FirmwareStatusNotification) \
    apply(GetCompositeScheduleResponse) \
    apply(GetConfigurationResponse) \
    apply(GetDiagnosticsResponse) \
    apply(GetInstalledCertificateIdsResponse) \
    apply(GetLocalListVersionResponse) \
    apply(GetLogResponse) \
    apply(Heartbeat) \
    apply(InstallCertificateResponse) \
    apply(LogStatusNotification) \
    apply(MeterValues) \
    apply(RemoteStartTransactionResponse) \
    apply(RemoteStopTransactionResponse) \
    apply(ReserveNowResponse) \
    apply(ResetResponse) \
    apply(SecurityEventNotification) \
    apply(SendLocalListResponse) \
    apply(SetChargingProfileResponse) \
    apply(SignCertificate) \
    apply(SignedFirmwareStatusNotification) \
    apply(SignedUpdateFirmwareResponse) \
    apply(StartTransaction) \
    apply(StatusNotification) \
    apply(StopTransaction) \
    apply(TriggerMessageResponse) \
    apply(UnlockConnectorResponse) \
    apply(UpdateFirmwareResponse)

#define FOR_ALL_UnsupportedMessageTypesSending(apply) \
    apply(AuthorizeResponse) \
    apply(BootNotificationResponse) \
    apply(CancelReservation) \
    apply(CertificateSigned) \
    apply(ChangeAvailability) \
    apply(ChangeConfiguration) \
    apply(ClearCache) \
    apply(ClearChargingProfile) \
    apply(DeleteCertificate) \
    apply(DiagnosticsStatusNotificationResponse) \
    apply(ExtendedTriggerMessage) \
    apply(FirmwareStatusNotificationResponse) \
    apply(GetCompositeSchedule) \
    apply(GetConfiguration) \
    apply(GetDiagnostics) \
    apply(GetInstalledCertificateIds) \
    apply(GetLocalListVersion) \
    apply(GetLog) \
    apply(HeartbeatResponse) \
    apply(InstallCertificate) \
    apply(LogStatusNotificationResponse) \
    apply(MeterValuesResponse) \
    apply(RemoteStartTransaction) \
    apply(RemoteStopTransaction) \
    apply(ReserveNow) \
    apply(Reset) \
    apply(SecurityEventNotificationResponse) \
    apply(SendLocalList) \
    apply(SetChargingProfile) \
    apply(SignCertificateResponse) \
    apply(SignedFirmwareStatusNotificationResponse) \
    apply(SignedUpdateFirmware) \
    apply(StartTransactionResponse) \
    apply(StatusNotificationResponse) \
    apply(StopTransactionResponse) \
    apply(TriggerMessage) \
    apply(UnlockConnector) \
    apply(UpdateFirmware) \
    apply(InternalError)

#define FOR_ALL_SupportedMessageTypesReceiveing(apply) \
    apply(AuthorizeResponse) \
    apply(BootNotificationResponse) \
    apply(CancelReservation) \
    apply(CertificateSigned) \
    apply(ChangeAvailability) \
    apply(ChangeConfiguration) \
    apply(ClearCache) \
    apply(ClearChargingProfile) \
    apply(DataTransfer) \
    apply(DataTransferResponse) \
    apply(DeleteCertificate) \
    apply(DiagnosticsStatusNotificationResponse) \
    apply(ExtendedTriggerMessage) \
    apply(FirmwareStatusNotificationResponse) \
    apply(GetCompositeSchedule) \
    apply(GetConfiguration) \
    apply(GetDiagnostics) \
    apply(GetInstalledCertificateIds) \
    apply(GetLocalListVersion) \
    apply(GetLog) \
    apply(HeartbeatResponse) \
    apply(InstallCertificate) \
    apply(LogStatusNotificationResponse) \
    apply(MeterValuesResponse) \
    apply(RemoteStartTransaction) \
    apply(RemoteStopTransaction) \
    apply(ReserveNow) \
    apply(Reset) \
    apply(SecurityEventNotificationResponse) \
    apply(SendLocalList) \
    apply(SetChargingProfile) \
    apply(SignCertificateResponse) \
    apply(SignedFirmwareStatusNotificationResponse) \
    apply(SignedUpdateFirmware) \
    apply(StartTransactionResponse) \
    apply(StatusNotificationResponse) \
    apply(StopTransactionResponse) \
    apply(TriggerMessage) \
    apply(UnlockConnector) \
    apply(UpdateFirmware)

#define FOR_ALL_UnsupportedMessageTypesReceiveing(apply) \
    apply(Authorize) \
    apply(BootNotification) \
    apply(CancelReservationResponse) \
    apply(CertificateSignedResponse) \
    apply(ChangeAvailabilityResponse) \
    apply(ChangeConfigurationResponse) \
    apply(ClearCacheResponse) \
    apply(ClearChargingProfileResponse) \
    apply(DeleteCertificateResponse) \
    apply(DiagnosticsStatusNotification) \
    apply(ExtendedTriggerMessageResponse) \
    apply(FirmwareStatusNotification) \
    apply(GetCompositeScheduleResponse) \
    apply(GetConfigurationResponse) \
    apply(GetDiagnosticsResponse) \
    apply(GetInstalledCertificateIdsResponse) \
    apply(GetLocalListVersionResponse) \
    apply(GetLogResponse) \
    apply(Heartbeat) \
    apply(InstallCertificateResponse) \
    apply(LogStatusNotification) \
    apply(MeterValues) \
    apply(RemoteStartTransactionResponse) \
    apply(RemoteStopTransactionResponse) \
    apply(ReserveNowResponse) \
    apply(ResetResponse) \
    apply(SecurityEventNotification) \
    apply(SendLocalListResponse) \
    apply(SetChargingProfileResponse) \
    apply(SignCertificate) \
    apply(SignedFirmwareStatusNotification) \
    apply(SignedUpdateFirmwareResponse) \
    apply(StartTransaction) \
    apply(StatusNotification) \
    apply(StopTransaction) \
    apply(TriggerMessageResponse) \
    apply(UnlockConnectorResponse) \
    apply(UpdateFirmwareResponse) \
    apply(InternalError)
// clang-format on

bool in_set(const std::set<ocpp::v16::MessageType>& valid, ocpp::v16::MessageType value) {
    const auto res = valid.find(value);
    return res != valid.end();
}

TEST_P(Configuration, CentralSystemURI) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getCentralSystemURI(), "127.0.0.1:8180/steve/websocket/CentralSystemService/");
    get()->setCentralSystemURI("CentralSystemURIvalue");
    EXPECT_EQ(get()->getCentralSystemURI(), "CentralSystemURIvalue");
    auto kv = get()->getCentralSystemURIKeyValue();
    EXPECT_EQ(kv.key, "CentralSystemURI");
    EXPECT_EQ(kv.value, "CentralSystemURIvalue");
    // TODO(james-ctc): schema lists this as readonly yet setters exist
    // it is possible that the schema is in error
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, ChargePointId) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getChargePointId(), "cp001");
    auto kv = get()->getChargePointIdKeyValue();
    EXPECT_EQ(kv.key, "ChargePointId");
    EXPECT_EQ(kv.value, "cp001");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, SupportedCiphers12) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getSupportedCiphers12(),
              "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:AES128-GCM-SHA256:AES256-GCM-SHA384");
    auto kv = get()->getSupportedCiphers12KeyValue();
    EXPECT_EQ(kv.key, "SupportedCiphers12");
    EXPECT_EQ(kv.value,
              "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:AES128-GCM-SHA256:AES256-GCM-SHA384");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, SupportedCiphers13) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getSupportedCiphers13(), "TLS_AES_256_GCM_SHA384:TLS_AES_128_GCM_SHA256");
    auto kv = get()->getSupportedCiphers13KeyValue();
    EXPECT_EQ(kv.key, "SupportedCiphers13");
    EXPECT_EQ(kv.value, "TLS_AES_256_GCM_SHA384:TLS_AES_128_GCM_SHA256");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, SupportedMeasurands) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getSupportedMeasurands(),
              "Energy.Active.Import.Register,Energy.Active.Export.Register,Power.Active.Import,Voltage,Current.Import,"
              "Frequency,Current.Offered,Power.Offered,SoC,Temperature");
    auto kv = get()->getSupportedMeasurandsKeyValue();
    EXPECT_EQ(kv.key, "SupportedMeasurands");
    EXPECT_EQ(kv.value, "Energy.Active.Import.Register,Energy.Active.Export.Register,Power.Active.Import,Voltage,"
                        "Current.Import,Frequency,Current.Offered,Power.Offered,SoC,Temperature");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, TLSKeylogFile) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getTLSKeylogFile(), "/tmp/ocpp_tls_keylog.txt");
}

TEST_P(Configuration, WebsocketPingPayload) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getWebsocketPingPayload(), "hello there");
    auto kv = get()->getWebsocketPingPayloadKeyValue();
    EXPECT_EQ(kv.key, "WebsocketPingPayload");
    EXPECT_EQ(kv.value, "hello there");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, ChargePointModel) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getChargePointModel(), "Yeti");
    auto kv = get()->getChargePointModelKeyValue();
    EXPECT_EQ(kv.key, "ChargePointModel");
    EXPECT_EQ(kv.value, "Yeti");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, ChargePointVendor) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getChargePointVendor(), "Pionix");
    auto kv = get()->getChargePointVendorKeyValue();
    EXPECT_EQ(kv.key, "ChargePointVendor");
    EXPECT_EQ(kv.value, "Pionix");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, AuthorizeConnectorZeroOnConnectorOne) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_TRUE(get()->getAuthorizeConnectorZeroOnConnectorOne());
    auto kv = get()->getAuthorizeConnectorZeroOnConnectorOneKeyValue();
    EXPECT_EQ(kv.key, "AuthorizeConnectorZeroOnConnectorOne");
    EXPECT_EQ(kv.value, "true");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, LogMessages) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_TRUE(get()->getLogMessages());
    auto kv = get()->getLogMessagesKeyValue();
    EXPECT_EQ(kv.key, "LogMessages");
    EXPECT_EQ(kv.value, "true");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, LogMessagesRaw) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_FALSE(get()->getLogMessagesRaw());
    auto kv = get()->getLogMessagesRawKeyValue();
    EXPECT_EQ(kv.key, "LogMessagesRaw");
    EXPECT_EQ(kv.value, "false");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, LogRotationDateSuffix) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_FALSE(get()->getLogRotationDateSuffix());
    auto kv = get()->getLogRotationDateSuffixKeyValue();
    EXPECT_EQ(kv.key, "LogRotationDateSuffix");
    EXPECT_EQ(kv.value, "false");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, LogRotation) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_FALSE(get()->getLogRotation());
    auto kv = get()->getLogRotationKeyValue();
    EXPECT_EQ(kv.key, "LogRotation");
    EXPECT_EQ(kv.value, "false");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, LogRotationMaximumFileCount) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getLogRotationMaximumFileCount(), 0);
    auto kv = get()->getLogRotationMaximumFileCountKeyValue();
    EXPECT_EQ(kv.key, "LogRotationMaximumFileCount");
    EXPECT_EQ(kv.value, "0");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, LogRotationMaximumFileSize) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getLogRotationMaximumFileSize(), 0);
    auto kv = get()->getLogRotationMaximumFileSizeKeyValue();
    EXPECT_EQ(kv.key, "LogRotationMaximumFileSize");
    EXPECT_EQ(kv.value, "0");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, MaxCompositeScheduleDuration) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getMaxCompositeScheduleDuration(), 31536000);
    auto kv = get()->getMaxCompositeScheduleDurationKeyValue();
    EXPECT_EQ(kv.key, "MaxCompositeScheduleDuration");
    EXPECT_EQ(kv.value, "31536000");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, MaxMessageSize) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getMaxMessageSize(), 65000);
    auto kv = get()->getMaxMessageSizeKeyValue();
    EXPECT_EQ(kv.key, "MaxMessageSize");
    EXPECT_EQ(kv.value, "65000");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, OcspRequestInterval) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getOcspRequestInterval(), 604800);
    auto kv = get()->getOcspRequestIntervalKeyValue();
    EXPECT_EQ(kv.key, "OcspRequestInterval");
    EXPECT_EQ(kv.value, "604800");
    EXPECT_FALSE(kv.readonly);

    get()->setOcspRequestInterval(86500);
    EXPECT_EQ(get()->getOcspRequestInterval(), 86500);
    kv = get()->getOcspRequestIntervalKeyValue();
    EXPECT_EQ(kv.key, "OcspRequestInterval");
    EXPECT_EQ(kv.value, "86500");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, RetryBackoffRandomRange) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getRetryBackoffRandomRange(), 10);
    auto kv = get()->getRetryBackoffRandomRangeKeyValue();
    EXPECT_EQ(kv.key, "RetryBackoffRandomRange");
    EXPECT_EQ(kv.value, "10");
    EXPECT_FALSE(kv.readonly);

    get()->setRetryBackoffRandomRange(3600);
    EXPECT_EQ(get()->getRetryBackoffRandomRange(), 3600);
    kv = get()->getRetryBackoffRandomRangeKeyValue();
    EXPECT_EQ(kv.key, "RetryBackoffRandomRange");
    EXPECT_EQ(kv.value, "3600");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, RetryBackoffRepeatTimes) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getRetryBackoffRepeatTimes(), 3);
    auto kv = get()->getRetryBackoffRepeatTimesKeyValue();
    EXPECT_EQ(kv.key, "RetryBackoffRepeatTimes");
    EXPECT_EQ(kv.value, "3");
    EXPECT_FALSE(kv.readonly);

    get()->setRetryBackoffRepeatTimes(10);
    EXPECT_EQ(get()->getRetryBackoffRepeatTimes(), 10);
    kv = get()->getRetryBackoffRepeatTimesKeyValue();
    EXPECT_EQ(kv.key, "RetryBackoffRepeatTimes");
    EXPECT_EQ(kv.value, "10");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, RetryBackoffWaitMinimum) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getRetryBackoffWaitMinimum(), 3);
    auto kv = get()->getRetryBackoffWaitMinimumKeyValue();
    EXPECT_EQ(kv.key, "RetryBackoffWaitMinimum");
    EXPECT_EQ(kv.value, "3");
    EXPECT_FALSE(kv.readonly);

    get()->setRetryBackoffWaitMinimum(15);
    EXPECT_EQ(get()->getRetryBackoffWaitMinimum(), 15);
    kv = get()->getRetryBackoffWaitMinimumKeyValue();
    EXPECT_EQ(kv.key, "RetryBackoffWaitMinimum");
    EXPECT_EQ(kv.value, "15");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, StopTransactionIfUnlockNotSupported) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_FALSE(get()->getStopTransactionIfUnlockNotSupported());
    auto kv = get()->getStopTransactionIfUnlockNotSupportedKeyValue();
    EXPECT_EQ(kv.key, "StopTransactionIfUnlockNotSupported");
    EXPECT_EQ(kv.value, "false");
    EXPECT_FALSE(kv.readonly);

    get()->setStopTransactionIfUnlockNotSupported(true);
    EXPECT_EQ(get()->getStopTransactionIfUnlockNotSupported(), true);
    kv = get()->getStopTransactionIfUnlockNotSupportedKeyValue();
    EXPECT_EQ(kv.key, "StopTransactionIfUnlockNotSupported");
    EXPECT_EQ(kv.value, "true");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, UseSslDefaultVerifyPaths) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_TRUE(get()->getUseSslDefaultVerifyPaths());
    auto kv = get()->getUseSslDefaultVerifyPathsKeyValue();
    EXPECT_EQ(kv.key, "UseSslDefaultVerifyPaths");
    EXPECT_EQ(kv.value, "true");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, VerifyCsmsAllowWildcards) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_FALSE(get()->getVerifyCsmsAllowWildcards());
    auto kv = get()->getVerifyCsmsAllowWildcardsKeyValue();
    EXPECT_EQ(kv.key, "VerifyCsmsAllowWildcards");
    EXPECT_EQ(kv.value, "false");
    EXPECT_TRUE(kv.readonly);

    get()->setVerifyCsmsAllowWildcards(true);
    EXPECT_EQ(get()->getVerifyCsmsAllowWildcards(), true);
    kv = get()->getVerifyCsmsAllowWildcardsKeyValue();
    EXPECT_EQ(kv.key, "VerifyCsmsAllowWildcards");
    EXPECT_EQ(kv.value, "true");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, VerifyCsmsCommonName) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_TRUE(get()->getVerifyCsmsCommonName());
    auto kv = get()->getVerifyCsmsCommonNameKeyValue();
    EXPECT_EQ(kv.key, "VerifyCsmsCommonName");
    EXPECT_EQ(kv.value, "true");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, WaitForStopTransactionsOnResetTimeout) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getWaitForStopTransactionsOnResetTimeout(), 60);
    auto kv = get()->getWaitForStopTransactionsOnResetTimeoutKeyValue();
    EXPECT_EQ(kv.key, "WaitForStopTransactionsOnResetTimeout");
    EXPECT_EQ(kv.value, "60");
    EXPECT_FALSE(kv.readonly);

    get()->setWaitForStopTransactionsOnResetTimeout(12);
    EXPECT_EQ(get()->getWaitForStopTransactionsOnResetTimeout(), 12);
    kv = get()->getWaitForStopTransactionsOnResetTimeoutKeyValue();
    EXPECT_EQ(kv.key, "WaitForStopTransactionsOnResetTimeout");
    EXPECT_EQ(kv.value, "12");
    EXPECT_FALSE(kv.readonly);
}

TEST_P(Configuration, WebsocketPongTimeout) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getWebsocketPongTimeout(), 5);
    auto kv = get()->getWebsocketPongTimeoutKeyValue();
    EXPECT_EQ(kv.key, "WebsocketPongTimeout");
    EXPECT_EQ(kv.value, "5");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, InternalBooleans) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files
    EXPECT_EQ(get()->getNumberOfConnectors(), 1); // needed by getAuthorizeConnectorZeroOnConnectorOne

    EXPECT_FALSE(get()->getEnableTLSKeylog());
    EXPECT_FALSE(get()->getUseTPM());
    EXPECT_FALSE(get()->getUseTPMSeccLeafCertificate());
}

TEST_P(Configuration, LogMessagesFormat) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    const auto res = get()->getLogMessagesFormat();
    EXPECT_TRUE(res.empty());
    auto kv = get()->getLogMessagesFormatKeyValue();
    EXPECT_EQ(kv.key, "LogMessagesFormat");
    EXPECT_EQ(kv.value, "");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, IgnoredProfilePurposesOffline) {
    using ChargingProfilePurposeType = ocpp::v16::ChargingProfilePurposeType;
    ASSERT_NE(get(), nullptr);
    // No initial value set - hence set() doesn't work

    auto res = get()->getIgnoredProfilePurposesOffline();
    EXPECT_TRUE(res.empty());
    auto kv = get()->getIgnoredProfilePurposesOfflineKeyValue();
    ASSERT_FALSE(kv.has_value());

    // set only works when there is a value configured
    get()->setIgnoredProfilePurposesOffline("TxDefaultProfile");
    res = get()->getIgnoredProfilePurposesOffline();
    EXPECT_TRUE(res.empty());
    kv = get()->getIgnoredProfilePurposesOfflineKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, SupportedChargingProfilePurposeTypes) {
    using ChargingProfilePurposeType = ocpp::v16::ChargingProfilePurposeType;
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    const auto res = get()->getSupportedChargingProfilePurposeTypes();
    ASSERT_EQ(res.size(), 3);
    EXPECT_EQ(res[0], ChargingProfilePurposeType::ChargePointMaxProfile);
    EXPECT_EQ(res[1], ChargingProfilePurposeType::TxDefaultProfile);
    EXPECT_EQ(res[2], ChargingProfilePurposeType::TxProfile);

    auto kv = get()->getSupportedChargingProfilePurposeTypesKeyValue();
    EXPECT_EQ(kv.key, "SupportedChargingProfilePurposeTypes");
    EXPECT_EQ(kv.value, "ChargePointMaxProfile,TxDefaultProfile,TxProfile");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, AllowChargingProfileWithoutStartSchedule) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    EXPECT_TRUE(get()->getAllowChargingProfileWithoutStartSchedule().has_value());
    EXPECT_EQ(get()->getAllowChargingProfileWithoutStartSchedule().value(), true);
    auto kv = get()->getAllowChargingProfileWithoutStartScheduleKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "AllowChargingProfileWithoutStartSchedule");
    EXPECT_EQ(kv.value().value, "true");
    EXPECT_FALSE(kv.value().readonly);

    get()->setAllowChargingProfileWithoutStartSchedule(false);
    EXPECT_TRUE(get()->getAllowChargingProfileWithoutStartSchedule().has_value());
    EXPECT_EQ(get()->getAllowChargingProfileWithoutStartSchedule().value(), false);
    kv = get()->getAllowChargingProfileWithoutStartScheduleKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "AllowChargingProfileWithoutStartSchedule");
    EXPECT_EQ(kv.value().value, "false");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_P(Configuration, CompositeScheduleDefaultLimitAmps) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    EXPECT_TRUE(get()->getCompositeScheduleDefaultLimitAmps().has_value());
    EXPECT_EQ(get()->getCompositeScheduleDefaultLimitAmps(), 48);
    auto kv = get()->getCompositeScheduleDefaultLimitAmpsKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CompositeScheduleDefaultLimitAmps");
    EXPECT_EQ(kv.value().value, "48");
    EXPECT_FALSE(kv.value().readonly);

    get()->setCompositeScheduleDefaultLimitAmps(32);
    EXPECT_TRUE(get()->getCompositeScheduleDefaultLimitAmps().has_value());
    EXPECT_EQ(get()->getCompositeScheduleDefaultLimitAmps(), 32);
    kv = get()->getCompositeScheduleDefaultLimitAmpsKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CompositeScheduleDefaultLimitAmps");
    EXPECT_EQ(kv.value().value, "32");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_P(Configuration, CompositeScheduleDefaultLimitWatts) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    EXPECT_TRUE(get()->getCompositeScheduleDefaultLimitWatts().has_value());
    EXPECT_EQ(get()->getCompositeScheduleDefaultLimitWatts(), 33120);
    auto kv = get()->getCompositeScheduleDefaultLimitWattsKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CompositeScheduleDefaultLimitWatts");
    EXPECT_EQ(kv.value().value, "33120");
    EXPECT_FALSE(kv.value().readonly);

    get()->setCompositeScheduleDefaultLimitWatts(34000);
    EXPECT_EQ(get()->getCompositeScheduleDefaultLimitWatts(), 34000);
    kv = get()->getCompositeScheduleDefaultLimitWattsKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CompositeScheduleDefaultLimitWatts");
    EXPECT_EQ(kv.value().value, "34000");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_P(Configuration, CompositeScheduleDefaultNumberPhases) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    EXPECT_TRUE(get()->getCompositeScheduleDefaultNumberPhases().has_value());
    EXPECT_EQ(get()->getCompositeScheduleDefaultNumberPhases(), 3);
    auto kv = get()->getCompositeScheduleDefaultNumberPhasesKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CompositeScheduleDefaultNumberPhases");
    EXPECT_EQ(kv.value().value, "3");
    EXPECT_FALSE(kv.value().readonly);

    get()->setCompositeScheduleDefaultNumberPhases(1);
    EXPECT_TRUE(get()->getCompositeScheduleDefaultNumberPhases().has_value());
    EXPECT_EQ(get()->getCompositeScheduleDefaultNumberPhases(), 1);
    kv = get()->getCompositeScheduleDefaultNumberPhasesKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "CompositeScheduleDefaultNumberPhases");
    EXPECT_EQ(kv.value().value, "1");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_P(Configuration, ConnectorEvseIds) {
    ASSERT_NE(get(), nullptr);
    // No initial value set - hence set() doesn't work

    EXPECT_FALSE(get()->getConnectorEvseIds().has_value());
    auto kv = get()->getConnectorEvseIdsKeyValue();
    ASSERT_FALSE(kv.has_value());

    // set only works when there is a value configured
    get()->setConnectorEvseIds("01234567");
    EXPECT_FALSE(get()->getConnectorEvseIds().has_value());
    kv = get()->getConnectorEvseIdsKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, HostName) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    EXPECT_FALSE(get()->getHostName().has_value());
    auto kv = get()->getHostNameKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, IFace) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    EXPECT_FALSE(get()->getIFace().has_value());
    auto kv = get()->getIFaceKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, MessageTypesDiscardForQueueing) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    EXPECT_FALSE(get()->getMessageTypesDiscardForQueueing().has_value());
    auto kv = get()->getMessageTypesDiscardForQueueingKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, MessageQueueSizeThreshold) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    EXPECT_FALSE(get()->getMessageQueueSizeThreshold().has_value());
    auto kv = get()->getMessageQueueSizeThresholdKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, QueueAllMessages) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    EXPECT_FALSE(get()->getQueueAllMessages().has_value());
    auto kv = get()->getQueueAllMessagesKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, SeccLeafSubjectCommonName) {
    ASSERT_NE(get(), nullptr);
    // No initial value set - hence set() doesn't work

    EXPECT_FALSE(get()->getSeccLeafSubjectCommonName().has_value());
    auto kv = get()->getSeccLeafSubjectCommonNameKeyValue();
    ASSERT_FALSE(kv.has_value());

    // set only works when there is a value configured
    get()->setSeccLeafSubjectCommonName("0123456789AB");
    EXPECT_FALSE(get()->getSeccLeafSubjectCommonName().has_value());
    kv = get()->getSeccLeafSubjectCommonNameKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, SeccLeafSubjectCountry) {
    ASSERT_NE(get(), nullptr);
    // No initial value set - hence set() doesn't work

    EXPECT_FALSE(get()->getSeccLeafSubjectCountry().has_value());
    auto kv = get()->getSeccLeafSubjectCountryKeyValue();
    ASSERT_FALSE(kv.has_value());

    // set only works when there is a value configured
    get()->setSeccLeafSubjectCountry("UK");
    EXPECT_FALSE(get()->getSeccLeafSubjectCountry().has_value());
    kv = get()->getSeccLeafSubjectCountryKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, SeccLeafSubjectOrganization) {
    ASSERT_NE(get(), nullptr);
    // No initial value set - hence set() doesn't work

    EXPECT_FALSE(get()->getSeccLeafSubjectOrganization().has_value());
    auto kv = get()->getSeccLeafSubjectOrganizationKeyValue();
    ASSERT_FALSE(kv.has_value());

    // set only works when there is a value configured
    get()->setSeccLeafSubjectOrganization("0123456789AB_organisation");
    EXPECT_FALSE(get()->getSeccLeafSubjectOrganization().has_value());
    kv = get()->getSeccLeafSubjectOrganizationKeyValue();
    ASSERT_FALSE(kv.has_value());
}

TEST_P(Configuration, SupplyVoltage) {
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    EXPECT_TRUE(get()->getSupplyVoltage().has_value());
    EXPECT_EQ(get()->getSupplyVoltage(), 230);
    auto kv = get()->getSupplyVoltageKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "SupplyVoltage");
    EXPECT_EQ(kv.value().value, "230");
    EXPECT_FALSE(kv.value().readonly);

    get()->setSupplyVoltage(250);
    EXPECT_TRUE(get()->getSupplyVoltage().has_value());
    EXPECT_EQ(get()->getSupplyVoltage(), 250);
    kv = get()->getSupplyVoltageKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "SupplyVoltage");
    EXPECT_EQ(kv.value().value, "250");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_P(Configuration, AllMeterPublicKeys) {
    ASSERT_NE(get(), nullptr);
    EXPECT_EQ(get()->getAllMeterPublicKeyKeyValues(), std::nullopt);
}

TEST_P(Configuration, PublicKey) {
    ASSERT_NE(get(), nullptr);

    const auto max = get()->getNumberOfConnectors();
    for (std::uint8_t i = 0; i <= max; i++) {
        SCOPED_TRACE(std::to_string(i));
        EXPECT_EQ(get()->getPublicKeyKeyValue(i), std::nullopt);
    }

    for (std::uint8_t i = 1; i <= max; i++) {
        SCOPED_TRACE(std::to_string(i));
        auto key = std::string{"MeterPublicKey["} + std::to_string(i) + ']';
        auto value = std::string{"Public Key: "} + std::to_string(i);
        EXPECT_TRUE(get()->setMeterPublicKey(i, value));
        auto kv = get()->getPublicKeyKeyValue(i);
        ASSERT_TRUE(kv);
        EXPECT_EQ(kv.value().key, key.c_str());
        EXPECT_EQ(kv.value().value, value.c_str());
        EXPECT_TRUE(kv.value().readonly);
    }

    auto kvl = get()->getAllMeterPublicKeyKeyValues();
    ASSERT_TRUE(kvl);
    ASSERT_EQ(kvl.value().size(), 1);
    auto kv = kvl.value()[0];
    EXPECT_EQ(kv.key, "MeterPublicKey[1]");
    EXPECT_EQ(kv.value, "Public Key: 1");
    EXPECT_TRUE(kv.readonly);
}

TEST_P(Configuration, SupportedMessageTypesSending) {
    using MessageType = ocpp::v16::MessageType;
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    const auto res = get()->getSupportedMessageTypesSending();
    ASSERT_EQ(res.size(), 40);
    std::uint8_t count = 0;

#define VALUE(a)                                                                                                       \
    EXPECT_TRUE(in_set(res, ocpp::v16::MessageType::a));                                                               \
    count++;
    FOR_ALL_SupportedMessageTypesSending(VALUE);
    EXPECT_EQ(count, res.size());
#undef VALUE
#define VALUE(a) EXPECT_FALSE(in_set(res, ocpp::v16::MessageType::a));
    FOR_ALL_UnsupportedMessageTypesSending(VALUE);
#undef VALUE
}

TEST_P(Configuration, SupportedMessageTypesReceiving) {
    using MessageType = ocpp::v16::MessageType;
    ASSERT_NE(get(), nullptr);
    // initial values are from the JSON unit test config files

    const auto res = get()->getSupportedMessageTypesReceiving();
    ASSERT_EQ(res.size(), 40);
    std::uint8_t count = 0;

#define VALUE(a)                                                                                                       \
    EXPECT_TRUE(in_set(res, ocpp::v16::MessageType::a));                                                               \
    count++;
    FOR_ALL_SupportedMessageTypesReceiveing(VALUE);
    EXPECT_EQ(count, res.size());
#undef VALUE
#define VALUE(a) EXPECT_FALSE(in_set(res, ocpp::v16::MessageType::a));
    FOR_ALL_UnsupportedMessageTypesReceiveing(VALUE);
#undef VALUE
}

// -----------------------------------------------------------------------------
// Oneoff tests where there are differences between implementations
// Note: TEST_F and not TEST_P

TEST_F(Configuration, SetIgnoredProfilePurposesOfflineV2) {
    using ChargingProfilePurposeType = ocpp::v16::ChargingProfilePurposeType;
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Internal", "IgnoredProfilePurposesOffline", "");

    auto res = v2_config->getIgnoredProfilePurposesOffline();
    EXPECT_TRUE(res.empty());
    auto kv = v2_config->getIgnoredProfilePurposesOfflineKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "IgnoredProfilePurposesOffline");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setIgnoredProfilePurposesOffline("TxDefaultProfile");
    res = v2_config->getIgnoredProfilePurposesOffline();
    ASSERT_EQ(res.size(), 1);
    EXPECT_EQ(res[0], ChargingProfilePurposeType::TxDefaultProfile);
    kv = v2_config->getIgnoredProfilePurposesOfflineKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "IgnoredProfilePurposesOffline");
    EXPECT_EQ(kv.value().value, "TxDefaultProfile");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetSeccLeafSubjectCommonNameV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Internal", "SeccLeafSubjectCommonName", "");

    EXPECT_TRUE(v2_config->getSeccLeafSubjectCommonName().has_value());
    EXPECT_EQ(v2_config->getSeccLeafSubjectCommonName(), "");
    auto kv = v2_config->getSeccLeafSubjectCommonNameKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "SeccLeafSubjectCommonName");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setSeccLeafSubjectCommonName("0123456789AB");
    EXPECT_TRUE(v2_config->getSeccLeafSubjectCommonName().has_value());
    EXPECT_EQ(v2_config->getSeccLeafSubjectCommonName(), "0123456789AB");
    kv = v2_config->getSeccLeafSubjectCommonNameKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "SeccLeafSubjectCommonName");
    EXPECT_EQ(kv.value().value, "0123456789AB");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetSeccLeafSubjectCountryV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Internal", "SeccLeafSubjectCountry", "");

    EXPECT_TRUE(v2_config->getSeccLeafSubjectCountry().has_value());
    auto kv = v2_config->getSeccLeafSubjectCountryKeyValue();
    EXPECT_EQ(v2_config->getSeccLeafSubjectCountry(), "");
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "SeccLeafSubjectCountry");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setSeccLeafSubjectCountry("UK");
    EXPECT_TRUE(v2_config->getSeccLeafSubjectCountry().has_value());
    EXPECT_EQ(v2_config->getSeccLeafSubjectCountry(), "UK");
    kv = v2_config->getSeccLeafSubjectCountryKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "SeccLeafSubjectCountry");
    EXPECT_EQ(kv.value().value, "UK");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetSeccLeafSubjectOrganizationV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Internal", "SeccLeafSubjectOrganization", "");

    EXPECT_TRUE(v2_config->getSeccLeafSubjectOrganization().has_value());
    EXPECT_EQ(v2_config->getSeccLeafSubjectOrganization(), "");
    auto kv = v2_config->getSeccLeafSubjectOrganizationKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "SeccLeafSubjectOrganization");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setSeccLeafSubjectOrganization("0123456789AB_organisation");
    EXPECT_TRUE(v2_config->getSeccLeafSubjectOrganization().has_value());
    EXPECT_EQ(v2_config->getSeccLeafSubjectOrganization(), "0123456789AB_organisation");
    kv = v2_config->getSeccLeafSubjectOrganizationKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "SeccLeafSubjectOrganization");
    EXPECT_EQ(kv.value().value, "0123456789AB_organisation");
    EXPECT_FALSE(kv.value().readonly);
}

TEST_F(Configuration, SetConnectorEvseIdsV2) {
    ASSERT_TRUE(device_model);
    // set an initial value
    device_model->set("Internal", "ConnectorEvseIds", "");

    EXPECT_TRUE(v2_config->getConnectorEvseIds().has_value());
    EXPECT_EQ(v2_config->getConnectorEvseIds(), "");
    auto kv = v2_config->getConnectorEvseIdsKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "ConnectorEvseIds");
    EXPECT_EQ(kv.value().value, "");
    EXPECT_FALSE(kv.value().readonly);

    v2_config->setConnectorEvseIds("01234567");
    EXPECT_TRUE(v2_config->getConnectorEvseIds().has_value());
    EXPECT_EQ(v2_config->getConnectorEvseIds(), "01234567");
    kv = v2_config->getConnectorEvseIdsKeyValue();
    ASSERT_TRUE(kv.has_value());
    EXPECT_EQ(kv.value().key, "ConnectorEvseIds");
    EXPECT_EQ(kv.value().value, "01234567");
    EXPECT_FALSE(kv.value().readonly);
}

} // namespace
