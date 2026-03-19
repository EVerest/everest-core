// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V16_KNOWN_KEYS_HPP
#define OCPP_V16_KNOWN_KEYS_HPP

#include "ocpp/v16/types.hpp"
#include "ocpp/v2/ocpp_enums.hpp"
#include "ocpp/v2/ocpp_types.hpp"

#include <cstdint>
#include <optional>
#include <string_view>
#include <tuple>

namespace ocpp::v16::keys {

// clang-format off
// ============================================================================
// Standard OCPP 1.6 keys
// ============================================================================

// SupportedMeasurands is collected from the following valuesList elements
// - AlignedDataMeasurands
// - AlignedDataTxEndedMeasurands
// - SampledDataTxEndedMeasurands
// - SampledDataTxStartedMeasurands
// - SampledDataTxUpdatedMeasurands

#define MAPPING_STANDARD(mapping) \
    mapping(AllowOfflineTxForUnknownId, OfflineTxForUnknownIdEnabled, Actual) \
    mapping(AuthorizationCacheEnabled, AuthCacheCtrlrEnabled, Actual) \
    mapping(AuthorizeRemoteTxRequests, AuthorizeRemoteStart, Actual) \
    mapping(ClockAlignedDataInterval, AlignedDataInterval, Actual) \
    mapping(ConnectionTimeOut, EVConnectionTimeOut, Actual) \
    mapping(HeartbeatInterval, HeartbeatInterval, Actual) \
    mapping(LocalAuthorizeOffline, LocalAuthorizeOffline, Actual) \
    mapping(LocalPreAuthorize, LocalPreAuthorize, Actual) \
    mapping(MaxEnergyOnInvalidId, MaxEnergyOnInvalidId, Actual) \
    mapping(MeterValuesAlignedData, AlignedDataMeasurands, Actual) \
    mapping(MeterValuesAlignedDataMaxLength, AlignedDataMeasurands, MaxSet) \
    mapping(MeterValuesSampledData, SampledDataTxUpdatedMeasurands, Actual) \
    mapping(MeterValuesSampledDataMaxLength, SampledDataTxUpdatedMeasurands, MaxSet) \
    mapping(MeterValueSampleInterval, SampledDataTxUpdatedInterval, Actual) \
    mapping(ResetRetries, ResetRetries, Actual) \
    mapping(StopTransactionOnInvalidId, StopTxOnInvalidId, Actual) \
    mapping(StopTxnAlignedData, AlignedDataTxEndedMeasurands, Actual) \
    mapping(StopTxnAlignedDataMaxLength, AlignedDataTxEndedMeasurands, MaxSet) \
    mapping(StopTxnSampledData, SampledDataTxEndedMeasurands, Actual) \
    mapping(StopTxnSampledDataMaxLength, SampledDataTxEndedMeasurands, MaxSet) \
    mapping(TransactionMessageAttempts, MessageAttempts, Actual) \
    mapping(TransactionMessageRetryInterval, MessageAttemptInterval, Actual) \
    mapping(WebSocketPingInterval, WebSocketPingInterval, Actual) \
    mapping(LocalAuthListEnabled, LocalAuthListCtrlrEnabled, Actual) \
    mapping(ChargeProfileMaxStackLevel, ChargingProfileMaxStackLevel, Actual) \
    mapping(ChargingScheduleAllowedChargingRateUnit, ChargingScheduleChargingRateUnit, Actual) \
    mapping(ChargingScheduleMaxPeriods, PeriodsPerSchedule, Actual) \
    mapping(ConnectorSwitch3to1PhaseSupported, Phases3to1, Actual) \
    mapping(SupportedFileTransferProtocols, FileTransferProtocols, Actual)

// ============================================================================
// Internal configuration keys
// ============================================================================

#define MAPPING_INTERNAL(mapping) \
    mapping(ChargePointId, ChargePointId, Actual) \
    mapping(ChargeBoxSerialNumber, ChargeBoxSerialNumber, Actual) \
    mapping(ChargePointModel, ChargePointModel, Actual) \
    mapping(ChargePointSerialNumber, ChargePointSerialNumber, Actual) \
    mapping(ChargePointVendor, ChargePointVendor, Actual) \
    mapping(FirmwareVersion, FirmwareVersion, Actual) \
    mapping(ICCID, ICCID, Actual) \
    mapping(IFace, IFace, Actual) \
    mapping(IMSI, IMSI, Actual) \
    mapping(MeterSerialNumber, MeterSerialNumber, Actual) \
    mapping(MeterType, MeterType, Actual) \
    mapping(SupportedCiphers12, SupportedCiphers12, Actual) \
    mapping(SupportedCiphers13, SupportedCiphers13, Actual) \
    mapping(UseTPM, UseTPM, Actual) \
    mapping(UseTPMSeccLeafCertificate, UseTPMSeccLeafCertificate, Actual) \
    mapping(RetryBackoffRandomRange, RetryBackOffRandomRange, Actual) \
    mapping(RetryBackoffRepeatTimes, RetryBackOffRepeatTimes, Actual) \
    mapping(AuthorizeConnectorZeroOnConnectorOne,AuthorizeConnectorZeroOnConnectorOne, Actual) \
    mapping(LogMessages, LogMessages, Actual) \
    mapping(LogMessagesRaw, LogMessagesRaw, Actual) \
    mapping(LogMessagesFormat, LogMessagesFormat, Actual) \
    mapping(LogRotation, LogRotation, Actual) \
    mapping(LogRotationDateSuffix, LogRotationDateSuffix, Actual) \
    mapping(LogRotationMaximumFileSize, LogRotationMaximumFileSize, Actual) \
    mapping(LogRotationMaximumFileCount, LogRotationMaximumFileCount, Actual) \
    mapping(SupportedChargingProfilePurposeTypes,SupportedChargingProfilePurposeTypes, Actual) \
    mapping(IgnoredProfilePurposesOffline, IgnoredProfilePurposesOffline, Actual) \
    mapping(MaxCompositeScheduleDuration, MaxCompositeScheduleDuration, Actual) \
    mapping(CompositeScheduleDefaultLimitAmps, CompositeScheduleDefaultLimitAmps, Actual) \
    mapping(CompositeScheduleDefaultLimitWatts, CompositeScheduleDefaultLimitWatts, Actual) \
    mapping(CompositeScheduleDefaultNumberPhases, CompositeScheduleDefaultNumberPhases, Actual) \
    mapping(SupplyVoltage, SupplyVoltage, Actual) \
    mapping(WebsocketPingPayload, WebsocketPingPayload, Actual) \
    mapping(WebsocketPongTimeout, WebsocketPongTimeout, Actual) \
    mapping(UseSslDefaultVerifyPaths, UseSslDefaultVerifyPaths, Actual) \
    mapping(VerifyCsmsCommonName, VerifyCsmsCommonName, Actual) \
    mapping(VerifyCsmsAllowWildcards, VerifyCsmsAllowWildcards, Actual) \
    mapping(OcspRequestInterval, OcspRequestInterval, Actual) \
    mapping(SeccLeafSubjectCommonName, ISO15118CtrlrSeccId, Actual) \
    mapping(SeccLeafSubjectCountry, ISO15118CtrlrCountryName, Actual) \
    mapping(SeccLeafSubjectOrganization, ISO15118CtrlrOrganizationName, Actual) \
    mapping(QueueAllMessages, QueueAllMessages, Actual) \
    mapping(MessageTypesDiscardForQueueing, MessageTypesDiscardForQueueing, Actual) \
    mapping(MessageQueueSizeThreshold, MessageQueueSizeThreshold, Actual) \
    mapping(MaxMessageSize, MaxMessageSize, Actual) \
    mapping(TLSKeylogFile, TLSKeylogFile, Actual) \
    mapping(EnableTLSKeylog, EnableTLSKeylog, Actual) \
    mapping(NumberOfConnectors, NumberOfConnectors, Actual) \
    mapping(RetryBackoffWaitMinimum, RetryBackOffWaitMinimum, Actual)

// ============================================================================
// LocalAuthList Section
// ============================================================================

#define MAPPING_LOCAL_AUTH_LIST(mapping) \
    mapping(LocalAuthListMaxLength, LocalAuthListCtrlrEntries, Actual) \
    mapping(SendLocalListMaxLength, ItemsPerMessageSendLocalList, Actual)

// ============================================================================
// Smart Charging Section
// ============================================================================

#define MAPPING_SMART_CHARGING(mapping) \
    mapping(MaxChargingProfilesInstalled, EntriesChargingProfiles, Actual)

// ============================================================================
// Security Section
// ============================================================================

#define MAPPING_SECURITY(mapping) \
    mapping(AdditionalRootCertificateCheck, AdditionalRootCertificateCheck, Actual) \
    mapping(CertificateSignedMaxChainSize, MaxCertificateChainSize, Actual) \
    mapping(CpoName, OrganizationName, Actual) \
    mapping(CertSigningWaitMinimum, CertSigningWaitMinimum, Actual) \
    mapping(CertSigningRepeatTimes, CertSigningRepeatTimes, Actual) \
    mapping(CertificateStoreMaxLength, CertificateEntries, Actual)

// ============================================================================
// PnC Section
// ============================================================================

#define MAPPING_PNC(mapping) \
    mapping(ISO15118PnCEnabled, PnCEnabled, Actual) \
    mapping(CentralContractValidationAllowed, CentralContractValidationAllowed, Actual) \
    mapping(ContractValidationOffline, ContractValidationOffline, Actual)

// ============================================================================
// CostAndPrice Section
// ============================================================================

#define MAPPING_COST(mapping) \
    mapping(NumberOfDecimalsForCostValues, NumberOfDecimalsForCostValues, Actual) \
    mapping(TimeOffset, TimeOffset, Actual) \
    mapping(NextTimeOffsetTransitionDateTime, NextTimeOffsetTransitionDateTime, Actual) \
    mapping(TimeOffsetNextTransition, TimeOffsetNextTransition, Actual)

// ============================================================================
// Mavericks Section - OCPP 1.6 keys without direct OCPP 2.x equivalents
// ============================================================================

#define MAPPING_MISC(mapping) \
    mapping(BlinkRepeat, BlinkRepeat, Actual) \
    mapping(ConnectorPhaseRotation, ConnectorPhaseRotation, Actual) \
    mapping(ConnectorPhaseRotationMaxLength, ConnectorPhaseRotationMaxLength, Actual) \
    mapping(GetConfigurationMaxKeys, GetConfigurationMaxKeys, Actual) \
    mapping(LightIntensity, LightIntensity, Actual) \
    mapping(MinimumStatusDuration, MinimumStatusDuration, Actual) \
    mapping(StopTransactionOnEVSideDisconnect, StopTransactionOnEVSideDisconnect, Actual) \
    mapping(SupportedFeatureProfiles, SupportedFeatureProfiles, Actual) \
    mapping(SupportedFeatureProfilesMaxLength, SupportedFeatureProfilesMaxLength, Actual) \
    mapping(UnlockConnectorOnEVSideDisconnect, UnlockConnectorOnEVSideDisconnect, Actual) \
    mapping(ReserveConnectorZeroSupported, ReserveConnectorZeroSupported, Actual) \
    mapping(HostName, HostName, Actual) \
    mapping(AllowChargingProfileWithoutStartSchedule, AllowChargingProfileWithoutStartSchedule, Actual) \
    mapping(WaitForStopTransactionsOnResetTimeout, WaitForStopTransactionsOnResetTimeout, Actual) \
    mapping(StopTransactionIfUnlockNotSupported, StopTransactionIfUnlockNotSupported, Actual) \
    mapping(MeterPublicKeys, MeterPublicKeys, Actual) \
    mapping(DisableSecurityEventNotifications, DisableSecurityEventNotifications, Actual) \
    mapping(ISO15118CertificateManagementEnabled, ISO15118CertificateManagementEnabled, Actual) \
    mapping(CustomDisplayCostAndPrice, CustomDisplayCostAndPrice, Actual) \
    mapping(DefaultPrice, DefaultPrice, Actual) \
    mapping(DefaultPriceText, DefaultPriceText, Actual) \
    mapping(CustomIdleFeeAfterStop, CustomIdleFeeAfterStop, Actual) \
    mapping(SupportedLanguages, SupportedLanguages, Actual) \
    mapping(CustomMultiLanguageMessages, CustomMultiLanguageMessages, Actual) \
    mapping(Language, Language, Actual) \
    mapping(WaitForSetUserPriceTimeout, WaitForSetUserPriceTimeout, Actual)

// ============================================================================
// Mavericks Section - OCPP 1.6 keys where OCPP 2.x mapping is problematic
// ============================================================================

#define MAPPING_MISC_ADDITIONAL(mapping) \
    mapping(AuthorizationKey, AuthorizationKey16, Actual) \
    mapping(CentralSystemURI, CentralSystemURI16, Actual) \
    mapping(SecurityProfile, SecurityProfile16, Actual)

#define MAPPING_ALL(mapping) \
    MAPPING_MISC_ADDITIONAL(mapping) \
    MAPPING_MISC(mapping) \
    MAPPING_STANDARD(mapping) \
    MAPPING_INTERNAL(mapping) \
    MAPPING_LOCAL_AUTH_LIST(mapping) \
    MAPPING_SMART_CHARGING(mapping) \
    MAPPING_SECURITY(mapping) \
    MAPPING_PNC(mapping) \
    MAPPING_COST(mapping)

#define FOR_ALL_MAPPED_KEYS(key) \
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

// these have special handling
#define FOR_ALL_UNMAPPED_KEYS(key) \
    key(Internal, ConnectorEvseIds) \
    key(Internal, SupportedMeasurands)

#define FOR_ALL_KEYS(key) \
    FOR_ALL_MAPPED_KEYS(key) \
    FOR_ALL_UNMAPPED_KEYS(key)
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

using DeviceModel_CV = std::optional<std::tuple<ocpp::v2::Component, ocpp::v2::Variable, ocpp::v2::AttributeEnum>>;
DeviceModel_CV convert_v2(const std::string_view& str);
DeviceModel_CV convert_v2(valid_keys key);
std::optional<std::string> convert_v2(const ocpp::v2::Component& component, const ocpp::v2::Variable& variable,
                                      ocpp::v2::AttributeEnum attribute);

} // namespace ocpp::v16::keys

#endif // OCPP_V16_KNOWN_KEYS_HPP
