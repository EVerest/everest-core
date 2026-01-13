// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V16_KNOWN_KEYS_HPP
#define OCPP_V16_KNOWN_KEYS_HPP

#include <cstdint>
#include <optional>
#include <string_view>

namespace ocpp::v16::keys {

// clang-format off
#define FOR_ALL_KEYS(key) \
key(Core, AllowOfflineTxForUnknownId) \
key(Core, AuthorizationCacheEnabled) \
key(Core, AuthorizeRemoteTxRequests) \
key(Core, BlinkRepeat) \
key(Core, ClockAlignedDataInterval) \
key(Core, ConnectionTimeOut) \
key(Core, ConnectorPhaseRotation) \
key(Core, ConnectorPhaseRotationMaxLength) \
key(Core, GetConfigurationMaxKeys) \
key(Core, HeartbeatInterval) \
key(Core, LightIntensity) \
key(Core, LocalAuthorizeOffline) \
key(Core, LocalPreAuthorize) \
key(Core, MaxEnergyOnInvalidId) \
key(Core, MeterValuesAlignedData) \
key(Core, MeterValuesAlignedDataMaxLength) \
key(Core, MeterValueSampleInterval) \
key(Core, MeterValuesSampledData) \
key(Core, MeterValuesSampledDataMaxLength) \
key(Core, MinimumStatusDuration) \
key(Core, NumberOfConnectors) \
key(Core, ResetRetries) \
key(Core, StopTransactionOnEVSideDisconnect) \
key(Core, StopTransactionOnInvalidId) \
key(Core, StopTxnAlignedData) \
key(Core, StopTxnAlignedDataMaxLength) \
key(Core, StopTxnSampledData) \
key(Core, StopTxnSampledDataMaxLength) \
key(Core, SupportedFeatureProfiles) \
key(Core, SupportedFeatureProfilesMaxLength) \
key(Core, TransactionMessageAttempts) \
key(Core, TransactionMessageRetryInterval) \
key(Core, UnlockConnectorOnEVSideDisconnect) \
key(Core, WebSocketPingInterval) \
key(CostAndPrice, CustomDisplayCostAndPrice) \
key(CostAndPrice, CustomIdleFeeAfterStop) \
key(CostAndPrice, CustomMultiLanguageMessages) \
key(CostAndPrice, DefaultPrice) \
key(CostAndPrice, DefaultPriceText) \
key(CostAndPrice, Language) \
key(CostAndPrice, NextTimeOffsetTransitionDateTime) \
key(CostAndPrice, NumberOfDecimalsForCostValues) \
key(CostAndPrice, SupportedLanguages) \
key(CostAndPrice, TimeOffset) \
key(CostAndPrice, TimeOffsetNextTransition) \
key(CostAndPrice, WaitForSetUserPriceTimeout) \
key(FirmwareManagement, SupportedFileTransferProtocols) \
key(Internal, AllowChargingProfileWithoutStartSchedule) \
key(Internal, AuthorizeConnectorZeroOnConnectorOne) \
key(Internal, CentralSystemURI) \
key(Internal, ChargeBoxSerialNumber) \
key(Internal, ChargePointId) \
key(Internal, ChargePointModel) \
key(Internal, ChargePointSerialNumber) \
key(Internal, ChargePointVendor) \
key(Internal, CompositeScheduleDefaultLimitAmps) \
key(Internal, CompositeScheduleDefaultLimitWatts) \
key(Internal, CompositeScheduleDefaultNumberPhases) \
key(Internal, ConnectorEvseIds) \
key(Internal, EnableTLSKeylog) \
key(Internal, FirmwareVersion) \
key(Internal, HostName) \
key(Internal, ICCID) \
key(Internal, IFace) \
key(Internal, IgnoredProfilePurposesOffline) \
key(Internal, IMSI) \
key(Internal, LogMessages) \
key(Internal, LogMessagesFormat) \
key(Internal, LogMessagesRaw) \
key(Internal, LogRotation) \
key(Internal, LogRotationDateSuffix) \
key(Internal, LogRotationMaximumFileCount) \
key(Internal, LogRotationMaximumFileSize) \
key(Internal, MaxCompositeScheduleDuration) \
key(Internal, MaxMessageSize) \
key(Internal, MessageQueueSizeThreshold) \
key(Internal, MessageTypesDiscardForQueueing) \
key(Internal, MeterPublicKeys) \
key(Internal, MeterSerialNumber) \
key(Internal, MeterType) \
key(Internal, OcspRequestInterval) \
key(Internal, QueueAllMessages) \
key(Internal, RetryBackoffRandomRange) \
key(Internal, RetryBackoffRepeatTimes) \
key(Internal, RetryBackoffWaitMinimum) \
key(Internal, SeccLeafSubjectCommonName) \
key(Internal, SeccLeafSubjectCountry) \
key(Internal, SeccLeafSubjectOrganization) \
key(Internal, StopTransactionIfUnlockNotSupported) \
key(Internal, SupplyVoltage) \
key(Internal, SupportedChargingProfilePurposeTypes) \
key(Internal, SupportedCiphers12) \
key(Internal, SupportedCiphers13) \
key(Internal, SupportedMeasurands) \
key(Internal, TLSKeylogFile) \
key(Internal, UseSslDefaultVerifyPaths) \
key(Internal, UseTPM) \
key(Internal, UseTPMSeccLeafCertificate) \
key(Internal, VerifyCsmsAllowWildcards) \
key(Internal, VerifyCsmsCommonName) \
key(Internal, WaitForStopTransactionsOnResetTimeout) \
key(Internal, WebsocketPingPayload) \
key(Internal, WebsocketPongTimeout) \
key(LocalAuthListManagement, LocalAuthListEnabled) \
key(LocalAuthListManagement, LocalAuthListMaxLength) \
key(LocalAuthListManagement, SendLocalListMaxLength) \
key(PnC, CentralContractValidationAllowed) \
key(PnC, CertSigningRepeatTimes) \
key(PnC, CertSigningWaitMinimum) \
key(PnC, ContractValidationOffline) \
key(PnC, ISO15118CertificateManagementEnabled) \
key(PnC, ISO15118PnCEnabled) \
key(Reservation, ReserveConnectorZeroSupported) \
key(Security, AdditionalRootCertificateCheck) \
key(Security, AuthorizationKey) \
key(Security, CertificateSignedMaxChainSize) \
key(Security, CertificateStoreMaxLength) \
key(Security, CpoName) \
key(Security, DisableSecurityEventNotifications) \
key(Security, SecurityProfile) \
key(SmartCharging, ChargeProfileMaxStackLevel) \
key(SmartCharging, ChargingScheduleAllowedChargingRateUnit) \
key(SmartCharging, ChargingScheduleMaxPeriods) \
key(SmartCharging, ConnectorSwitch3to1PhaseSupported) \
key(SmartCharging, MaxChargingProfilesInstalled)

// clang-format on

#define VALUE(a, b) b,

enum class valid_keys : std::uint8_t {
    FOR_ALL_KEYS(VALUE)
};

enum class sections : std::uint8_t {
    Core,
    CostAndPrice,
    FirmwareManagement,
    Internal,
    LocalAuthListManagement,
    PnC,
    Reservation,
    Security,
    SmartCharging,
    Custom,
};

#undef VALUE

std::optional<valid_keys> convert(const std::string_view& str);
std::string_view convert(valid_keys key);
sections to_section(valid_keys key);
std::string_view to_section_string_view(valid_keys key);
bool is_readonly(valid_keys key);
inline bool is_in_section(sections section, valid_keys key) {
    return to_section(key) == section;
}
bool is_hidden(valid_keys key);

} // namespace ocpp::v16::keys

#endif // OCPP_V16_KNOWN_KEYS_HPP
