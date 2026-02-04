// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>
#include <ocpp/v16/known_keys.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>

#include <algorithm>
#include <iterator>
#include <map>
#include <optional>
#include <string>
#include <utility>

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
    key(LogRotationMaximumFileSize) \
    key(LogRotationMaximumFileCount) \
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
    key(Language)

// Hidden keys are ones that are not made available over OCPP
//  AuthorizationKey because it contains the connection secret
//  <others> because they are not in ChargePointConfiguration::get()
//  (which could be a bug)

#define FOR_ALL_HIDDEN(key) \
    key(AuthorizationKey) \
    key(EnableTLSKeylog) \
    key(LogRotation) \
    key(LogRotationDateSuffix) \
    key(LogRotationMaximumFileCount) \
    key(LogRotationMaximumFileSize) \
    key(TLSKeylogFile) \
    key(UseTPM) \
    key(UseTPMSeccLeafCertificate)

// clang-format on

#define VALUE(a) valid_keys::a,

constexpr valid_keys read_only[] = {FOR_ALL_READONLY(VALUE)};
constexpr valid_keys hidden[] = {FOR_ALL_HIDDEN(VALUE)};

#undef VALUE

class V2ConfigMap {
private:
    bool configured{false};
    std::map<std::string_view, const ocpp::v2::ComponentVariable*> map;
    std::map<std::string, std::string_view> reverse_map;

    void configure(const std::string_view& v16, const ocpp::v2::ComponentVariable& cv);
    void configure();

public:
    ocpp::v16::keys::DeviceModel_CV convert_v2(const std::string_view& v16_key);
    std::string convert_v2(const ocpp::v2::Component& component, const ocpp::v2::Variable& variable);
};

void V2ConfigMap::configure(const std::string_view& v16, const ocpp::v2::ComponentVariable& cv) {
    const auto& variable = cv.variable;
    if (variable) {
        if (const auto it = map.find(v16); it != map.end()) {
            if (it->second->variable) {
                EVLOG_warning << "V16 " << v16 << ": '" << it->second->variable->name << "' replaced with '"
                              << variable->name << '\'';
            }
        }
        map.insert({v16, &cv});
        std::string name = std::string{cv.component.name} + std::string{variable->name};
        if (variable->instance) {
            name += std::string{variable->instance.value()};
        }
        if (const auto it = reverse_map.find(name); it != reverse_map.end()) {
            std::cerr << "V2 " << name << ": '" << it->second << "' replaced with '" << v16 << '\'';
        }
        reverse_map.insert({std::move(name), v16});
    }
}

#define VALUE(a, b) configure(#a, ocpp::v2::ControllerComponentVariables::b);
void V2ConfigMap::configure() {
    if (!configured) {
        configured = true;
        MAPPING_ALL(VALUE)
    }
}
#undef VALUE

ocpp::v16::keys::DeviceModel_CV V2ConfigMap::convert_v2(const std::string_view& v16_key) {
    configure();
    ocpp::v16::keys::DeviceModel_CV result;
    std::string name{v16_key};
    if (const auto it = map.find(name); it != map.end()) {
        if (it->second->variable) {
            result = std::make_pair(it->second->component, it->second->variable.value());
        }
    }
    return result;
}

std::string V2ConfigMap::convert_v2(const ocpp::v2::Component& component, const ocpp::v2::Variable& variable) {
    configure();
    std::string result = variable.name;
    const std::string name = std::string{component.name} + std::string{variable.name};
    if (const auto it = reverse_map.find(name); it != reverse_map.end()) {
        result = it->second;
    }
    return result;
}

V2ConfigMap v2_map;

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

#undef VALUE
#define VALUE(a, b)                                                                                                    \
    case valid_keys::b:                                                                                                \
        return ocpp::v16::SupportedFeatureProfiles::a;

std::optional<ocpp::v16::SupportedFeatureProfiles> get_profile(valid_keys key) {
    switch (key) {
        FOR_ALL_KEYS(VALUE)
    default:
        break;
    }
    return std::nullopt;
}

#undef VALUE

DeviceModel_CV convert_v2(const std::string_view& str) {
    return v2_map.convert_v2(str);
}

DeviceModel_CV convert_v2(valid_keys key) {
    return convert_v2(convert(key));
}

std::string convert_v2(const ocpp::v2::Component& component, const ocpp::v2::Variable& variable) {
    return v2_map.convert_v2(component, variable);
}

} // namespace ocpp::v16::keys
