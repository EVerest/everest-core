// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "ocpp/v2/ocpp_enums.hpp"
#include <everest/logging.hpp>
#include <ocpp/v16/known_keys.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>

#include <algorithm>
#include <iterator>
#include <map>
#include <optional>
#include <string>
#include <tuple>
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

constexpr char convert(ocpp::v2::AttributeEnum attribute) {
    char result{'.'};
    switch (attribute) {
    case ocpp::v2::AttributeEnum::Actual:
        result = 'A';
        break;
    case ocpp::v2::AttributeEnum::MaxSet:
        result = '+';
        break;
    case ocpp::v2::AttributeEnum::MinSet:
        result = '-';
        break;
    case ocpp::v2::AttributeEnum::Target:
        result = '=';
        break;
    default:
        break;
    }
    return result;
}

inline std::string mapping_name(const ocpp::v2::Component& component, const ocpp::v2::Variable& variable,
                                ocpp::v2::AttributeEnum attribute) {
    return std::string{component.name} + std::string{variable.name} + std::string{variable.instance.value_or("")} +
           convert(attribute);
}

class V2ConfigMap {
private:
    bool configured{false};
    using v2details = std::pair<const ocpp::v2::ComponentVariable*, ocpp::v2::AttributeEnum>;

    std::map<std::string_view, v2details> map;
    std::map<std::string, std::string_view> reverse_map;

    void configure(const std::string_view& v16, const ocpp::v2::ComponentVariable& cv,
                   ocpp::v2::AttributeEnum attribute);
    void configure();
    void warn_no_mapping(const std::string_view& v16);
    void check();

public:
    ocpp::v16::keys::DeviceModel_CV convert_v2(const std::string_view& v16_key);
    std::optional<std::string> convert_v2(const ocpp::v2::Component& component, const ocpp::v2::Variable& variable,
                                          ocpp::v2::AttributeEnum attribute);
};

void V2ConfigMap::configure(const std::string_view& v16, const ocpp::v2::ComponentVariable& cv,
                            ocpp::v2::AttributeEnum attribute) {
    if (cv.variable) {
        if (const auto it = map.find(v16); it != map.end()) {
            const auto& tmp_cv = std::get<const ocpp::v2::ComponentVariable*>(it->second);
            if (tmp_cv->variable) {
                EVLOG_warning << "V16 " << v16 << ": '" << tmp_cv->variable->name << "' replaced with '"
                              << cv.variable->name << '\'';
            }
        }
        map.insert_or_assign(v16, v2details{&cv, attribute});
        std::string name = mapping_name(cv.component, cv.variable.value(), attribute);
        if (const auto it = reverse_map.find(name); it != reverse_map.end()) {
            EVLOG_error << "V2 " << name << ": '" << it->second << "' replaced with '" << v16 << '\'';
        }
        reverse_map.insert_or_assign(std::move(name), v16);
    }
}

#define VALUE(a, b, c) configure(#a, ocpp::v2::ControllerComponentVariables::b, ocpp::v2::AttributeEnum::c);
void V2ConfigMap::configure() {
    if (!configured) {
        configured = true;
        MAPPING_ALL(VALUE)
        check();
    }
}
#undef VALUE

void V2ConfigMap::warn_no_mapping(const std::string_view& v16) {
    const auto it = map.find(v16);
    if (it == map.end()) {
        EVLOG_error << "No V2 mapping for " << v16;
    }
}

#define VALUE(a, b) warn_no_mapping(#b);
void V2ConfigMap::check() {
    if (configured) {
        FOR_ALL_MAPPED_KEYS(VALUE);
    }
}
#undef VALUE

ocpp::v16::keys::DeviceModel_CV V2ConfigMap::convert_v2(const std::string_view& v16_key) {
    configure();
    ocpp::v16::keys::DeviceModel_CV result;
    std::string name{v16_key};
    if (const auto it = map.find(name); it != map.end()) {
        const auto& cv = std::get<const ocpp::v2::ComponentVariable*>(it->second);
        const auto attribute = std::get<ocpp::v2::AttributeEnum>(it->second);
        if (cv->variable) {
            result = std::make_tuple(cv->component, cv->variable.value(), attribute);
        }
    }
    return result;
}

std::optional<std::string> V2ConfigMap::convert_v2(const ocpp::v2::Component& component,
                                                   const ocpp::v2::Variable& variable,
                                                   ocpp::v2::AttributeEnum attribute) {
    configure();
    std::optional<std::string> result;
    std::string name = mapping_name(component, variable, attribute);
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

std::optional<std::string> convert_v2(const ocpp::v2::Component& component, const ocpp::v2::Variable& variable,
                                      ocpp::v2::AttributeEnum attribute) {
    return v2_map.convert_v2(component, variable, attribute);
}

} // namespace ocpp::v16::keys
