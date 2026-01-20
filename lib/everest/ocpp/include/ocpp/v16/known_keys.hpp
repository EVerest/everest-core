// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V16_KNOWN_KEYS_HPP
#define OCPP_V16_KNOWN_KEYS_HPP

#include "ocpp/v16/types.hpp"
#include "ocpp/v2/ocpp_types.hpp"

#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>

namespace ocpp::v16::keys {

// clang-format off
// ============================================================================
// Standard OCPP 1.6 keys
// ============================================================================

#define MAPPING_STANDARD(mapping) \
    mapping(AllowOfflineTxForUnknownId, OfflineTxForUnknownIdEnabled) \
    mapping(AuthorizationCacheEnabled, AuthCacheCtrlrEnabled) \
    mapping(AuthorizeRemoteTxRequests, AuthorizeRemoteStart) \
    mapping(ClockAlignedDataInterval, AlignedDataInterval) \
    mapping(ConnectionTimeOut, EVConnectionTimeOut) \
    mapping(HeartbeatInterval, HeartbeatInterval) \
    mapping(LocalAuthorizeOffline, LocalAuthorizeOffline) \
    mapping(LocalPreAuthorize, LocalPreAuthorize) \
    mapping(MaxEnergyOnInvalidId, MaxEnergyOnInvalidId) \
    mapping(MeterValuesAlignedData, AlignedDataMeasurands) \
    mapping(MeterValuesSampledData, SampledDataTxUpdatedMeasurands) \
    mapping(MeterValueSampleInterval, SampledDataTxUpdatedInterval) \
    mapping(ResetRetries, ResetRetries) \
    mapping(StopTransactionOnInvalidId, StopTxOnInvalidId) \
    mapping(StopTxnAlignedData, AlignedDataTxEndedMeasurands) \
    mapping(StopTxnSampledData, SampledDataTxEndedMeasurands) \
    mapping(TransactionMessageAttempts, MessageAttempts) \
    mapping(TransactionMessageRetryInterval, MessageAttemptInterval) \
    mapping(WebSocketPingInterval, WebSocketPingInterval) \
    mapping(LocalAuthListEnabled, LocalAuthListCtrlrEnabled) \
    mapping(ChargeProfileMaxStackLevel, ChargingProfileMaxStackLevel) \
    mapping(ChargingScheduleAllowedChargingRateUnit, ChargingScheduleChargingRateUnit) \
    mapping(ChargingScheduleMaxPeriods, PeriodsPerSchedule) \
    mapping(ConnectorSwitch3to1PhaseSupported, Phases3to1) \
    mapping(SupportedFileTransferProtocols, FileTransferProtocols)

// ============================================================================
// Internal configuration keys
// ============================================================================

#define MAPPING_INTERNAL(mapping) \
    mapping(ChargePointId, ChargePointId) \
    mapping(ChargeBoxSerialNumber, ChargeBoxSerialNumber) \
    mapping(ChargePointModel, ChargePointModel) \
    mapping(ChargePointSerialNumber, ChargePointSerialNumber) \
    mapping(ChargePointVendor, ChargePointVendor) \
    mapping(FirmwareVersion, FirmwareVersion) \
    mapping(ICCID, ICCID) \
    mapping(IFace, IFace) \
    mapping(IMSI, IMSI) \
    mapping(MeterSerialNumber, MeterSerialNumber) \
    mapping(MeterType, MeterType) \
    mapping(SupportedCiphers12, SupportedCiphers12) \
    mapping(SupportedCiphers13, SupportedCiphers13) \
    mapping(UseTPM, UseTPM) \
    mapping(UseTPMSeccLeafCertificate, UseTPMSeccLeafCertificate) \
    mapping(RetryBackoffRandomRange, RetryBackOffRandomRange) \
    mapping(RetryBackoffRepeatTimes, RetryBackOffRepeatTimes) \
    mapping(AuthorizeConnectorZeroOnConnectorOne,AuthorizeConnectorZeroOnConnectorOne) \
    mapping(LogMessages, LogMessages) \
    mapping(LogMessagesRaw, LogMessagesRaw) \
    mapping(LogMessagesFormat, LogMessagesFormat) \
    mapping(LogRotation, LogRotation) \
    mapping(LogRotationDateSuffix, LogRotationDateSuffix) \
    mapping(LogRotationMaximumFileSize, LogRotationMaximumFileSize) \
    mapping(LogRotationMaximumFileCount, LogRotationMaximumFileCount) \
    mapping(SupportedChargingProfilePurposeTypes,SupportedChargingProfilePurposeTypes) \
    mapping(IgnoredProfilePurposesOffline, IgnoredProfilePurposesOffline) \
    mapping(MaxCompositeScheduleDuration, MaxCompositeScheduleDuration) \
    mapping(CompositeScheduleDefaultLimitAmps, CompositeScheduleDefaultLimitAmps) \
    mapping(CompositeScheduleDefaultLimitWatts, CompositeScheduleDefaultLimitWatts) \
    mapping(CompositeScheduleDefaultNumberPhases, CompositeScheduleDefaultNumberPhases) \
    mapping(SupplyVoltage, SupplyVoltage) \
    mapping(WebsocketPingPayload, WebsocketPingPayload) \
    mapping(WebsocketPongTimeout, WebsocketPongTimeout) \
    mapping(UseSslDefaultVerifyPaths, UseSslDefaultVerifyPaths) \
    mapping(VerifyCsmsCommonName, VerifyCsmsCommonName) \
    mapping(VerifyCsmsAllowWildcards, VerifyCsmsAllowWildcards) \
    mapping(OcspRequestInterval, OcspRequestInterval) \
    mapping(SeccLeafSubjectCommonName, ISO15118CtrlrSeccId) \
    mapping(SeccLeafSubjectCountry, ISO15118CtrlrCountryName) \
    mapping(SeccLeafSubjectOrganization, ISO15118CtrlrOrganizationName) \
    mapping(QueueAllMessages, QueueAllMessages) \
    mapping(MessageTypesDiscardForQueueing, MessageTypesDiscardForQueueing) \
    mapping(MessageQueueSizeThreshold, MessageQueueSizeThreshold) \
    mapping(MaxMessageSize, MaxMessageSize) \
    mapping(TLSKeylogFile, TLSKeylogFile) \
    mapping(EnableTLSKeylog, EnableTLSKeylog) \
    mapping(NumberOfConnectors, NumberOfConnectors)

// ============================================================================
// Security Section
// ============================================================================

#define MAPPING_SECURITY(mapping) \
    mapping(AdditionalRootCertificateCheck, AdditionalRootCertificateCheck) \
    mapping(AuthorizationKey, BasicAuthPassword) \
    mapping(CertificateSignedMaxChainSize, MaxCertificateChainSize) \
    mapping(CpoName, OrganizationName) \
    mapping(SecurityProfile, SecurityProfile) \
    mapping(CertSigningWaitMinimum, CertSigningWaitMinimum) \
    mapping(CertSigningRepeatTimes, CertSigningRepeatTimes)

// ============================================================================
// PnC Section
// ============================================================================

#define MAPPING_PNC(mapping) \
    mapping(ISO15118PnCEnabled, PnCEnabled) \
    mapping(CentralContractValidationAllowed, CentralContractValidationAllowed) \
    mapping(ContractValidationOffline, ContractValidationOffline)

// ============================================================================
// CostAndPrice Section
// ============================================================================

#define MAPPING_COST(mapping) \
    mapping(NumberOfDecimalsForCostValues, NumberOfDecimalsForCostValues) \
    mapping(TimeOffset, TimeOffset) \
    mapping(NextTimeOffsetTransitionDateTime, NextTimeOffsetTransitionDateTime) \
    mapping(TimeOffsetNextTransition, TimeOffsetNextTransition)

// ============================================================================
// Mavericks Section - OCPP 1.6 keys without direct OCPP 2.x equivalents
// ============================================================================

#define MAPPING_MISC(mapping) \
    mapping(BlinkRepeat, BlinkRepeat) \
    mapping(ConnectorPhaseRotationMaxLength, ConnectorPhaseRotationMaxLength) \
    mapping(GetConfigurationMaxKeys, GetConfigurationMaxKeys) \
    mapping(LightIntensity, LightIntensity) \
    mapping(MinimumStatusDuration, MinimumStatusDuration) \
    mapping(StopTransactionOnEVSideDisconnect, StopTransactionOnEVSideDisconnect) \
    mapping(SupportedFeatureProfiles, SupportedFeatureProfiles) \
    mapping(SupportedFeatureProfilesMaxLength, SupportedFeatureProfilesMaxLength) \
    mapping(UnlockConnectorOnEVSideDisconnect, UnlockConnectorOnEVSideDisconnect) \
    mapping(ReserveConnectorZeroSupported, ReserveConnectorZeroSupported) \
    mapping(HostName, HostName) \
    mapping(AllowChargingProfileWithoutStartSchedule, AllowChargingProfileWithoutStartSchedule) \
    mapping(WaitForStopTransactionsOnResetTimeout, WaitForStopTransactionsOnResetTimeout) \
    mapping(StopTransactionIfUnlockNotSupported, StopTransactionIfUnlockNotSupported) \
    mapping(MeterPublicKeys, MeterPublicKeys) \
    mapping(DisableSecurityEventNotifications, DisableSecurityEventNotifications) \
    mapping(ISO15118CertificateManagementEnabled, ISO15118CertificateManagementEnabled) \
    mapping(CustomDisplayCostAndPrice, CustomDisplayCostAndPrice) \
    mapping(DefaultPrice, DefaultPrice) \
    mapping(DefaultPriceText, DefaultPriceText) \
    mapping(CustomIdleFeeAfterStop, CustomIdleFeeAfterStop) \
    mapping(SupportedLanguages, SupportedLanguages) \
    mapping(CustomMultiLanguageMessages, CustomMultiLanguageMessages) \
    mapping(Language, Language) \
    mapping(WaitForSetUserPriceTimeout, WaitForSetUserPriceTimeout)

// TODO(james-ctc): check if there is a mapping to the V2 device model
#define MAPPING_MISC_ADDITIONAL(mapping) \
    mapping(AuthorizeRemoteTxRequests, AuthorizeRemoteTxRequests) \
    mapping(CentralSystemURI, CentralSystemURI) \
    mapping(CertificateStoreMaxLength, CertificateStoreMaxLength) \
    mapping(ChargeProfileMaxStackLevel, ChargeProfileMaxStackLevel) \
    mapping(ChargingScheduleAllowedChargingRateUnit, ChargingScheduleAllowedChargingRateUnit) \
    mapping(ChargingScheduleMaxPeriods, ChargingScheduleMaxPeriods) \
    mapping(ClockAlignedDataInterval, ClockAlignedDataInterval) \
    mapping(ConnectionTimeOut, ConnectionTimeOut) \
    mapping(ConnectorEvseIds, ConnectorEvseIds) \
    mapping(ConnectorPhaseRotation, ConnectorPhaseRotation) \
    mapping(ISO15118PnCEnabled, ISO15118PnCEnabled) \
    mapping(LocalAuthListEnabled, LocalAuthListEnabled) \
    mapping(LocalAuthListMaxLength, LocalAuthListMaxLength) \
    mapping(MaxChargingProfilesInstalled, MaxChargingProfilesInstalled) \
    mapping(MeterValueSampleInterval, MeterValueSampleInterval) \
    mapping(MeterValuesAlignedData, MeterValuesAlignedData) \
    mapping(MeterValuesAlignedDataMaxLength, MeterValuesAlignedDataMaxLength) \
    mapping(MeterValuesSampledData, MeterValuesSampledData) \
    mapping(MeterValuesSampledDataMaxLength, MeterValuesSampledDataMaxLength) \
    mapping(RetryBackoffRandomRange, RetryBackoffRandomRange) \
    mapping(RetryBackoffRepeatTimes, RetryBackoffRepeatTimes) \
    mapping(RetryBackoffWaitMinimum, RetryBackoffWaitMinimum) \
    mapping(SendLocalListMaxLength, SendLocalListMaxLength) \
    mapping(StopTransactionOnInvalidId, StopTransactionOnInvalidId) \
    mapping(StopTxnAlignedData, StopTxnAlignedData) \
    mapping(StopTxnAlignedDataMaxLength, StopTxnAlignedDataMaxLength) \
    mapping(StopTxnSampledData, StopTxnSampledData) \
    mapping(StopTxnSampledDataMaxLength, StopTxnSampledDataMaxLength) \
    mapping(SupportedFileTransferProtocols, SupportedFileTransferProtocols) \
    mapping(SupportedMeasurands, SupportedMeasurands) \
    mapping(TransactionMessageAttempts, TransactionMessageAttempts) \
    mapping(TransactionMessageRetryInterval, TransactionMessageRetryInterval)

#define MAPPING_ALL(mapping) \
    MAPPING_MISC_ADDITIONAL(mapping) \
    MAPPING_MISC(mapping) \
    MAPPING_STANDARD(mapping) \
    MAPPING_INTERNAL(mapping) \
    MAPPING_SECURITY(mapping) \
    MAPPING_PNC(mapping) \
    MAPPING_COST(mapping)

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
std::optional<ocpp::v16::SupportedFeatureProfiles> get_profile(valid_keys key);

using DeviceModel_CV = std::optional<std::pair<ocpp::v2::Component, ocpp::v2::Variable>>;
DeviceModel_CV convert_v2(const std::string_view& str);
DeviceModel_CV convert_v2(valid_keys key);
std::string convert_v2(const ocpp::v2::Component& component, const ocpp::v2::Variable& variable);

} // namespace ocpp::v16::keys

#endif // OCPP_V16_KNOWN_KEYS_HPP
