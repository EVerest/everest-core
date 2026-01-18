// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include <ocpp/v2/ctrlr_component_variables.hpp>

namespace ocpp {
namespace v16 {

/// \brief Represents a mapping from OCPP 1.6 configuration key to OCPP 2.x ComponentVariable
struct V16ToV2Mapping {
    std::string v16_key;
    v2::ComponentVariable component_variable;
};

/// \brief Static mappings from OCPP 1.6 configuration keys to OCPP 2.x ComponentVariable
static const std::vector<V16ToV2Mapping> OCPP16_TO_V2_MAPPINGS = {
    // ============================================================================
    // Standard OCPP 1.6 keys
    // ============================================================================

    {"AllowOfflineTxForUnknownId", v2::ControllerComponentVariables::OfflineTxForUnknownIdEnabled},
    {"AuthorizationCacheEnabled", v2::ControllerComponentVariables::AuthCacheCtrlrEnabled},
    {"AuthorizeRemoteTxRequests", v2::ControllerComponentVariables::AuthorizeRemoteStart},
    {"ClockAlignedDataInterval", v2::ControllerComponentVariables::AlignedDataInterval},
    {"ConnectionTimeOut", v2::ControllerComponentVariables::EVConnectionTimeOut},
    {"HeartbeatInterval", v2::ControllerComponentVariables::HeartbeatInterval},
    {"LocalAuthorizeOffline", v2::ControllerComponentVariables::LocalAuthorizeOffline},
    {"LocalPreAuthorize", v2::ControllerComponentVariables::LocalPreAuthorize},
    {"MaxEnergyOnInvalidId", v2::ControllerComponentVariables::MaxEnergyOnInvalidId},
    {"MeterValuesAlignedData", v2::ControllerComponentVariables::AlignedDataMeasurands},
    {"MeterValuesSampledData", v2::ControllerComponentVariables::SampledDataTxUpdatedMeasurands},
    {"MeterValueSampleInterval", v2::ControllerComponentVariables::SampledDataTxUpdatedInterval},
    {"ResetRetries", v2::ControllerComponentVariables::ResetRetries},
    {"StopTransactionOnInvalidId", v2::ControllerComponentVariables::StopTxOnInvalidId},
    {"StopTxnAlignedData", v2::ControllerComponentVariables::AlignedDataTxEndedMeasurands},
    {"StopTxnSampledData", v2::ControllerComponentVariables::SampledDataTxEndedMeasurands},
    {"TransactionMessageAttempts", v2::ControllerComponentVariables::MessageAttempts},
    {"TransactionMessageRetryInterval", v2::ControllerComponentVariables::MessageAttemptInterval},
    {"WebSocketPingInterval", v2::ControllerComponentVariables::WebSocketPingInterval},
    {"LocalAuthListEnabled", v2::ControllerComponentVariables::LocalAuthListCtrlrEnabled},
    {"ChargeProfileMaxStackLevel", v2::ControllerComponentVariables::ChargingProfileMaxStackLevel},
    {"ChargingScheduleAllowedChargingRateUnit", v2::ControllerComponentVariables::ChargingScheduleChargingRateUnit},
    {"ChargingScheduleMaxPeriods", v2::ControllerComponentVariables::PeriodsPerSchedule},
    {"ConnectorSwitch3to1PhaseSupported", v2::ControllerComponentVariables::Phases3to1},
    {"SupportedFileTransferProtocols", v2::ControllerComponentVariables::FileTransferProtocols},

    // ============================================================================
    // Internal configuration keys
    // ============================================================================

    {"ChargePointId", v2::ControllerComponentVariables::ChargePointId},
    {"ChargeBoxSerialNumber", v2::ControllerComponentVariables::ChargeBoxSerialNumber},
    {"ChargePointModel", v2::ControllerComponentVariables::ChargePointModel},
    {"ChargePointSerialNumber", v2::ControllerComponentVariables::ChargePointSerialNumber},
    {"ChargePointVendor", v2::ControllerComponentVariables::ChargePointVendor},
    {"FirmwareVersion", v2::ControllerComponentVariables::FirmwareVersion},
    {"ICCID", v2::ControllerComponentVariables::ICCID},
    {"IFace", v2::ControllerComponentVariables::IFace},
    {"IMSI", v2::ControllerComponentVariables::IMSI},
    {"MeterSerialNumber", v2::ControllerComponentVariables::MeterSerialNumber},
    {"MeterType", v2::ControllerComponentVariables::MeterType},
    {"SupportedCiphers12", v2::ControllerComponentVariables::SupportedCiphers12},
    {"SupportedCiphers13", v2::ControllerComponentVariables::SupportedCiphers13},
    {"UseTPM", v2::ControllerComponentVariables::UseTPM},
    {"UseTPMSeccLeafCertificate", v2::ControllerComponentVariables::UseTPMSeccLeafCertificate},
    {"RetryBackoffRandomRange", v2::ControllerComponentVariables::RetryBackOffRandomRange},
    {"RetryBackoffRepeatTimes", v2::ControllerComponentVariables::RetryBackOffRepeatTimes},
    {"AuthorizeConnectorZeroOnConnectorOne", v2::ControllerComponentVariables::AuthorizeConnectorZeroOnConnectorOne},
    {"LogMessages", v2::ControllerComponentVariables::LogMessages},
    {"LogMessagesRaw", v2::ControllerComponentVariables::LogMessagesRaw},
    {"LogMessagesFormat", v2::ControllerComponentVariables::LogMessagesFormat},
    {"LogRotation", v2::ControllerComponentVariables::LogRotation},
    {"LogRotationDateSuffix", v2::ControllerComponentVariables::LogRotationDateSuffix},
    {"LogRotationMaximumFileSize", v2::ControllerComponentVariables::LogRotationMaximumFileSize},
    {"LogRotationMaximumFileCount", v2::ControllerComponentVariables::LogRotationMaximumFileCount},
    {"SupportedChargingProfilePurposeTypes", v2::ControllerComponentVariables::SupportedChargingProfilePurposeTypes},
    {"IgnoredProfilePurposesOffline", v2::ControllerComponentVariables::IgnoredProfilePurposesOffline},
    {"MaxCompositeScheduleDuration", v2::ControllerComponentVariables::MaxCompositeScheduleDuration},
    {"CompositeScheduleDefaultLimitAmps", v2::ControllerComponentVariables::CompositeScheduleDefaultLimitAmps},
    {"CompositeScheduleDefaultLimitWatts", v2::ControllerComponentVariables::CompositeScheduleDefaultLimitWatts},
    {"CompositeScheduleDefaultNumberPhases", v2::ControllerComponentVariables::CompositeScheduleDefaultNumberPhases},
    {"SupplyVoltage", v2::ControllerComponentVariables::SupplyVoltage},
    {"WebsocketPingPayload", v2::ControllerComponentVariables::WebsocketPingPayload},
    {"WebsocketPongTimeout", v2::ControllerComponentVariables::WebsocketPongTimeout},
    {"UseSslDefaultVerifyPaths", v2::ControllerComponentVariables::UseSslDefaultVerifyPaths},
    {"VerifyCsmsCommonName", v2::ControllerComponentVariables::VerifyCsmsCommonName},
    {"VerifyCsmsAllowWildcards", v2::ControllerComponentVariables::VerifyCsmsAllowWildcards},
    {"OcspRequestInterval", v2::ControllerComponentVariables::OcspRequestInterval},
    {"SeccLeafSubjectCommonName", v2::ControllerComponentVariables::ISO15118CtrlrSeccId},
    {"SeccLeafSubjectCountry", v2::ControllerComponentVariables::ISO15118CtrlrCountryName},
    {"SeccLeafSubjectOrganization", v2::ControllerComponentVariables::ISO15118CtrlrOrganizationName},
    {"QueueAllMessages", v2::ControllerComponentVariables::QueueAllMessages},
    {"MessageTypesDiscardForQueueing", v2::ControllerComponentVariables::MessageTypesDiscardForQueueing},
    {"MessageQueueSizeThreshold", v2::ControllerComponentVariables::MessageQueueSizeThreshold},
    {"MaxMessageSize", v2::ControllerComponentVariables::MaxMessageSize},
    {"TLSKeylogFile", v2::ControllerComponentVariables::TLSKeylogFile},
    {"EnableTLSKeylog", v2::ControllerComponentVariables::EnableTLSKeylog},
    {"NumberOfConnectors", v2::ControllerComponentVariables::NumberOfConnectors},

    // ============================================================================
    // Security Section
    // ============================================================================

    {"AdditionalRootCertificateCheck", v2::ControllerComponentVariables::AdditionalRootCertificateCheck},
    {"AuthorizationKey", v2::ControllerComponentVariables::BasicAuthPassword},
    {"CertificateSignedMaxChainSize", v2::ControllerComponentVariables::MaxCertificateChainSize},
    {"CpoName", v2::ControllerComponentVariables::OrganizationName},
    {"SecurityProfile", v2::ControllerComponentVariables::SecurityProfile},
    {"CertSigningWaitMinimum", v2::ControllerComponentVariables::CertSigningWaitMinimum},
    {"CertSigningRepeatTimes", v2::ControllerComponentVariables::CertSigningRepeatTimes},

    // ============================================================================
    // PnC Section
    // ============================================================================

    {"ISO15118PnCEnabled", v2::ControllerComponentVariables::PnCEnabled},
    {"CentralContractValidationAllowed", v2::ControllerComponentVariables::CentralContractValidationAllowed},
    {"ContractValidationOffline", v2::ControllerComponentVariables::ContractValidationOffline},

    // ============================================================================
    // CostAndPrice Section
    // ============================================================================

    {"NumberOfDecimalsForCostValues", v2::ControllerComponentVariables::NumberOfDecimalsForCostValues},
    {"TimeOffset", v2::ControllerComponentVariables::TimeOffset},
    {"NextTimeOffsetTransitionDateTime", v2::ControllerComponentVariables::NextTimeOffsetTransitionDateTime},
    {"TimeOffsetNextTransition", v2::ControllerComponentVariables::TimeOffsetNextTransition},

    // ============================================================================
    // Mavericks Section - OCPP 1.6 keys without direct OCPP 2.x equivalents
    // ============================================================================

    {"BlinkRepeat", v2::ControllerComponentVariables::BlinkRepeat},
    {"ConnectorPhaseRotationMaxLength", v2::ControllerComponentVariables::ConnectorPhaseRotationMaxLength},
    {"GetConfigurationMaxKeys", v2::ControllerComponentVariables::GetConfigurationMaxKeys},
    {"LightIntensity", v2::ControllerComponentVariables::LightIntensity},
    {"MinimumStatusDuration", v2::ControllerComponentVariables::MinimumStatusDuration},
    {"StopTransactionOnEVSideDisconnect", v2::ControllerComponentVariables::StopTransactionOnEVSideDisconnect},
    {"SupportedFeatureProfiles", v2::ControllerComponentVariables::SupportedFeatureProfiles},
    {"SupportedFeatureProfilesMaxLength", v2::ControllerComponentVariables::SupportedFeatureProfilesMaxLength},
    {"UnlockConnectorOnEVSideDisconnect", v2::ControllerComponentVariables::UnlockConnectorOnEVSideDisconnect},
    {"ReserveConnectorZeroSupported", v2::ControllerComponentVariables::ReserveConnectorZeroSupported},
    {"HostName", v2::ControllerComponentVariables::HostName},
    {"AllowChargingProfileWithoutStartSchedule",
     v2::ControllerComponentVariables::AllowChargingProfileWithoutStartSchedule},
    {"WaitForStopTransactionsOnResetTimeout", v2::ControllerComponentVariables::WaitForStopTransactionsOnResetTimeout},
    {"StopTransactionIfUnlockNotSupported", v2::ControllerComponentVariables::StopTransactionIfUnlockNotSupported},
    {"MeterPublicKeys", v2::ControllerComponentVariables::MeterPublicKeys}, // FIXME
    {"DisableSecurityEventNotifications", v2::ControllerComponentVariables::DisableSecurityEventNotifications},
    {"ISO15118CertificateManagementEnabled", v2::ControllerComponentVariables::ISO15118CertificateManagementEnabled},
    {"CustomDisplayCostAndPrice", v2::ControllerComponentVariables::CustomDisplayCostAndPrice},
    {"DefaultPrice", v2::ControllerComponentVariables::DefaultPrice},
    {"DefaultPriceText", v2::ControllerComponentVariables::DefaultPriceText},
    {"CustomIdleFeeAfterStop", v2::ControllerComponentVariables::CustomIdleFeeAfterStop},
    {"SupportedLanguages", v2::ControllerComponentVariables::SupportedLanguages},
    {"CustomMultiLanguageMessages", v2::ControllerComponentVariables::CustomMultiLanguageMessages},
    {"Language", v2::ControllerComponentVariables::Language},
    {"WaitForSetUserPriceTimeout", v2::ControllerComponentVariables::WaitForSetUserPriceTimeout},
};

/// \brief Build a lookup map from OCPP 1.6 key to OCPP 2.x ComponentVariable
/// \return Map of v16_key -> ComponentVariable
inline std::map<std::string, v2::ComponentVariable> build_v16_to_v2_lookup() {
    std::map<std::string, v2::ComponentVariable> lookup;
    for (const auto& mapping : OCPP16_TO_V2_MAPPINGS) {
        lookup[mapping.v16_key] = mapping.component_variable;
    }
    return lookup;
}

/// \brief Get OCPP 2.x ComponentVariable for a given OCPP 1.6 configuration key
/// \param v16_key The OCPP 1.6 configuration key
/// \return Optional ComponentVariable if found
inline std::optional<v2::ComponentVariable> get_v2_component_variable(const std::string& v16_key) {
    static const auto lookup = build_v16_to_v2_lookup();
    auto it = lookup.find(v16_key);
    if (it != lookup.end()) {
        return it->second;
    }
    return std::nullopt;
}

} // namespace v16
} // namespace ocpp