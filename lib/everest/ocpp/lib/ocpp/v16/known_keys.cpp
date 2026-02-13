// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include <ocpp/v16/known_keys.hpp>

#include <algorithm>
#include <iterator>

namespace {
using ocpp::v16::keys::valid_keys;

// clang-format off
#define FOR_ALL_READONLY(key) \
    key(CentralSystemURI) \
    key(ChargePointId) \
    key(ChargeBoxSerialNumber) \
    key(ChargePointModel) \
    key(ChargePointSerialNumber) \
    key(ChargePointVendor) \
    key(FirmwareVersion) \
    key(ICCID) \
    key(IMSI) \
    key(MeterSerialNumber) \
    key(MeterType) \
    key(AuthorizeConnectorZeroOnConnectorOne) \
    key(LogMessages) \
    key(LogMessagesRaw) \
    key(LogMessagesFormat) \
    key(LogRotation) \
    key(LogRotationDateSuffix) \
    key(LogRotationMaximumFileCount) \
    key(LogRotationMaximumFileSize) \
    key(SupportedChargingProfilePurposeTypes) \
    key(MaxCompositeScheduleDuration) \
    key(SupportedCiphers12) \
    key(SupportedCiphers13) \
    key(UseSslDefaultVerifyPaths) \
    key(VerifyCsmsCommonName) \
    key(VerifyCsmsAllowWildcards) \
    key(SupportedMeasurands) \
    key(MaxMessageSize) \
    key(WebsocketPingPayload) \
    key(WebsocketPongTimeout) \
    key(QueueAllMessages) \
    key(MessageTypesDiscardForQueueing) \
    key(MessageQueueSizeThreshold) \
    key(ConnectorPhaseRotationMaxLength) \
    key(GetConfigurationMaxKeys) \
    key(MeterValuesAlignedDataMaxLength) \
    key(MeterValuesSampledDataMaxLength) \
    key(NumberOfConnectors) \
    key(ReserveConnectorZeroSupported) \
    key(StopTransactionOnEVSideDisconnect) \
    key(StopTxnAlignedDataMaxLength) \
    key(StopTxnSampledDataMaxLength) \
    key(SupportedFeatureProfiles) \
    key(SupportedFeatureProfilesMaxLength) \
    key(HostName) \
    key(IFace) \
    key(SupportedFileTransferProtocols) \
    key(ChargeProfileMaxStackLevel) \
    key(ChargingScheduleAllowedChargingRateUnit) \
    key(ChargingScheduleMaxPeriods) \
    key(ConnectorSwitch3to1PhaseSupported) \
    key(MaxChargingProfilesInstalled) \
    key(AdditionalRootCertificateCheck) \
    key(CertificateSignedMaxChainSize) \
    key(CertificateStoreMaxLength) \
    key(LocalAuthListMaxLength) \
    key(SendLocalListMaxLength) \
    key(CustomDisplayCostAndPrice) \
    key(NumberOfDecimalsForCostValues) \
    key(CustomMultiLanguageMessages) \
    key(SupportedLanguages) \
    key(Language) \
    key(EnableTLSKeylog) \
    key(TLSKeylogFile) \
    key(UseTPM) \
    key(UseTPMSeccLeafCertificate)

// Hidden keys are ones that are not made available over OCPP
//  AuthorizationKey because it contains the connection secret

#define FOR_ALL_HIDDEN(key) \
    key(AuthorizationKey)

// clang-format on

#define VALUE(a) valid_keys::a,

constexpr valid_keys read_only[] = {FOR_ALL_READONLY(VALUE)};
constexpr valid_keys hidden[] = {FOR_ALL_HIDDEN(VALUE)};

#undef VALUE
} // namespace

namespace ocpp::v16::keys {

#define VALUE(a, b)                                                                                                    \
    if (str == #b) {                                                                                                   \
        return valid_keys::b;                                                                                          \
    }

std::optional<valid_keys> convert(const std::string_view& str) {
    FOR_ALL_KEYS(VALUE)
    return {};
}

#undef VALUE
#define VALUE(a, b)                                                                                                    \
    case valid_keys::b:                                                                                                \
        return #b;

std::string_view convert(valid_keys key) {
    switch (key) {
        FOR_ALL_KEYS(VALUE)
    default:
        break;
    }
    return {};
}

#undef VALUE
#define VALUE(a, b)                                                                                                    \
    case valid_keys::b:                                                                                                \
        return sections::a;

sections to_section(valid_keys key) {
    switch (key) {
        FOR_ALL_KEYS(VALUE)
    default:
        break;
    }
    return sections::Custom;
}

#undef VALUE
#define VALUE(a, b)                                                                                                    \
    case valid_keys::b:                                                                                                \
        return #a;

std::string_view to_section_string_view(valid_keys key) {
    switch (key) {
        FOR_ALL_KEYS(VALUE)
    default:
        break;
    }
    return {};
}

#undef VALUE

bool is_readonly(valid_keys key) {
    return std::find(std::cbegin(read_only), std::cend(read_only), key) != std::cend(read_only);
}

bool is_hidden(valid_keys key) {
    return std::find(std::cbegin(hidden), std::cend(hidden), key) != std::cend(hidden);
}

} // namespace ocpp::v16::keys
