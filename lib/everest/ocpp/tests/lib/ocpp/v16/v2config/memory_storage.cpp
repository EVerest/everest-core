// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "memory_storage.hpp"

#include <exception>
#include <ocpp/v16/charge_point_configuration_devicemodel.hpp>
#include <ocpp/v16/utils.hpp>
#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

#include <map>
#include <optional>
#include <string>

namespace {

// initial values are from the JSON unit test config files
// Do not add additional values
const std::map<std::string, std::string> required_vars_internal = {
    {"CentralSystemURI", "127.0.0.1:8180/steve/websocket/CentralSystemService/"},
    {"ChargeBoxSerialNumber", "cp001"},
    {"ChargePointId", "cp001"},
    {"ChargePointVendor", "Pionix"},
    {"ChargePointModel", "Yeti"},
    {"FirmwareVersion", "0.1"},
    {"SupportedCiphers12",
     "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:AES128-GCM-SHA256:AES256-GCM-SHA384"},
    {"SupportedCiphers13", "TLS_AES_256_GCM_SHA384:TLS_AES_128_GCM_SHA256"},
    {"SupportedMeasurands", "Energy.Active.Import.Register,Energy.Active.Export.Register,Power.Active.Import,Voltage,"
                            "Current.Import,Frequency,Current.Offered,Power.Offered,SoC,Temperature"},
    {"TLSKeylogFile", "/tmp/ocpp_tls_keylog.txt"},
    {"WebsocketPingPayload", "hello there"},
    {"AuthorizeConnectorZeroOnConnectorOne", "true"},
    {"LogMessages", "true"},
    {"UseSslDefaultVerifyPaths", "true"},
    {"VerifyCsmsCommonName", "true"},
    {"EnableTLSKeylog", "false"},
    {"LogMessagesRaw", "false"},
    {"LogRotation", "false"},
    {"LogRotationDateSuffix", "false"},
    {"StopTransactionIfUnlockNotSupported", "false"},
    {"UseTPM", "false"},
    {"UseTPMSeccLeafCertificate", "false"},
    {"VerifyCsmsAllowWildcards", "false"},
    {"MaxMessageSize", "65000"},
    {"MaxCompositeScheduleDuration", "31536000"},
    {"OcspRequestInterval", "604800"},
    {"RetryBackoffRandomRange", "10"},
    {"RetryBackoffRepeatTimes", "3"},
    {"RetryBackoffWaitMinimum", "3"},
    {"WaitForStopTransactionsOnResetTimeout", "60"},
    {"WebsocketPongTimeout", "5"},
    {"LogRotationMaximumFileCount", "0"},
    {"LogRotationMaximumFileSize", "0"},
    {"SupportedChargingProfilePurposeTypes", "ChargePointMaxProfile,TxDefaultProfile,TxProfile"},
    {"LogMessagesFormat", ""},
    {"CompositeScheduleDefaultLimitAmps", "48"},
    {"CompositeScheduleDefaultLimitWatts", "33120"},
    {"CompositeScheduleDefaultNumberPhases", "3"},
    {"SupplyVoltage", "230"},
}; // namespace

// initial values are from the JSON unit test config files
// Do not add additional values
const std::map<std::string, std::string> required_vars_core = {
    {"NumberOfConnectors", "1"},
    {"SupportedFeatureProfiles",
     "Core,FirmwareManagement,RemoteTrigger,Reservation,LocalAuthListManagement,SmartCharging"},
    {"ConnectorPhaseRotation", "0.RST,1.RST"},
    {"MeterValuesAlignedData", "Energy.Active.Import.Register"},
    {"MeterValuesSampledData", "Energy.Active.Import.Register"},
    {"StopTxnAlignedData", "Energy.Active.Import.Register"},
    {"StopTxnSampledData", "Energy.Active.Import.Register"},
    {"AuthorizeRemoteTxRequests", "false"},
    {"LocalAuthorizeOffline", "false"},
    {"LocalPreAuthorize", "false"},
    {"StopTransactionOnInvalidId", "true"},
    {"UnlockConnectorOnEVSideDisconnect", "true"},
    {"ClockAlignedDataInterval", "900"},
    {"ConnectionTimeOut", "10"},
    {"GetConfigurationMaxKeys", "100"},
    {"HeartbeatInterval", "86400"},
    {"MeterValueSampleInterval", "0"},
    {"ResetRetries", "1"},
    {"TransactionMessageAttempts", "1"},
    {"TransactionMessageRetryInterval", "10"},
    {"StopTransactionOnEVSideDisconnect", "true"},
};

// initial values are from the JSON unit test config files
// Do not add additional values
const std::map<std::string, std::string> required_vars_firmware_management = {
    {"SupportedFileTransferProtocols", "FTP"},
};

// initial values are from the JSON unit test config files
// Do not add additional values
const std::map<std::string, std::string> required_vars_smart_charging = {
    {"ChargeProfileMaxStackLevel", "42"},
    {"ChargingScheduleAllowedChargingRateUnit", "Current"},
    {"ChargingScheduleMaxPeriods", "42"},
    {"MaxChargingProfilesInstalled", "42"},
};

// initial values are from the JSON unit test config files
// Do not add additional values
const std::map<std::string, std::string> required_vars_security = {
    {"SecurityProfile", "0"},
    {"DisableSecurityEventNotifications", "false"},
};

// initial values are from the JSON unit test config files
// Do not add additional values
const std::map<std::string, std::string> required_vars_local_auth_list = {
    {"LocalAuthListEnabled", "true"},
    {"LocalAuthListMaxLength", "42"},
    {"SendLocalListMaxLength", "42"},
};

// initial values are from the JSON unit test config files
// Do not add additional values
const std::map<std::string, std::string> required_vars_pnc = {
    {"ISO15118CertificateManagementEnabled", "true"},
    {"ISO15118PnCEnabled", "true"},
    {"ContractValidationOffline", "true"},
};

// initial values are from the JSON unit test config files
// Do not add additional values
const std::map<std::string, std::string> required_vars_california_pricing = {
    {"CustomDisplayCostAndPrice", "false"},
};

// initial values are from the JSON unit test config files
// Do not add additional values
const std::map<std::string, std::string> required_vars_custom = {};

// additional values for full config
const std::map<std::string, std::string> full_vars_california_pricing = {
    {"SupportedLanguages", "en, nl, de, nb_NO"},
    {"CustomMultiLanguageMessages", "true"},
    {"Language", "en"},
};

using MemoryStorage = ocpp::v16::stubs::MemoryStorage;
MemoryStorage::Storage vars_internal;
MemoryStorage::Storage vars_core;
MemoryStorage::Storage vars_firmware_management;
MemoryStorage::Storage vars_smart_charging;
MemoryStorage::Storage vars_security;
MemoryStorage::Storage vars_local_auth_list;
MemoryStorage::Storage vars_pnc;
MemoryStorage::Storage vars_california_pricing;
MemoryStorage::Storage vars_custom;
MemoryStorage::Storage vars_additional;

const std::vector<MemoryStorage::Storage*> vars_list = {
    &vars_internal,        &vars_core, &vars_firmware_management, &vars_smart_charging, &vars_security,
    &vars_local_auth_list, &vars_pnc,  &vars_california_pricing,  &vars_custom,         &vars_additional,
};

const ocpp::v2::VariableCharacteristics characteristics = {ocpp::v2::DataEnum::string, false, {}, {}, {}, {}, {}, {}};
const ocpp::v2::VariableMetaData meta_data = {characteristics, {}, {}};

bool operator==(const ocpp::v2::Component& lhs, const ocpp::v2::Component& rhs) {
    return lhs.name == rhs.name;
}

bool operator==(const std::optional<ocpp::v2::Variable>& lhs, const ocpp::v2::Variable& rhs) {
    if (lhs) {
        return lhs.value().name == rhs.name;
    }
    return false;
}

bool is_same(const ocpp::v2::RequiredComponentVariable& var, const ocpp::v2::Component& component,
             const ocpp::v2::Variable& variable) {
    return ((var.component == component) && (var.variable == variable));
}

std::string enhanced_convert(const ocpp::v2::Component& component, const ocpp::v2::Variable& variable,
                             ocpp::v2::AttributeEnum attribute) {
    using namespace ocpp::v2::ControllerComponentVariables;
    auto result = ocpp::v16::keys::convert_v2(component, variable, attribute);
    if (!result.has_value()) {
        if (component.name == "EVSE" && variable.name == "ISO15118EvseId") {
            result = std::string{ocpp::v16::keys::convert(ocpp::v16::keys::valid_keys::ConnectorEvseIds)};
        }
    }
    return result.value_or(variable.name);
}

std::string central_system_uri_to_json(const std::string& value) {
    // convert to JSON
    auto profile = json::parse(R"([{"ocppCsmsUrl":""}])");
    profile[0]["ocppCsmsUrl"] = value;
    return profile.dump(-1);
}

void add_to_report(std::vector<ocpp::v2::ReportData>& report, const ocpp::v2::RequiredComponentVariable& rcv,
                   ocpp::v2::MutabilityEnum mutability, const std::string& value) {
    if (rcv.variable) {
        ocpp::v2::ReportData data;
        data.component = rcv.component;
        data.variable = rcv.variable.value();
        ocpp::v2::VariableAttribute va;
        va.type = ocpp::v2::AttributeEnum::Actual;
        va.mutability = mutability;
        if (!value.empty()) {
            va.value = value;
        }
        data.variableAttribute.push_back(std::move(va));
        report.push_back(std::move(data));
    }
}

} // namespace

namespace ocpp::v16::stubs {
// ----------------------------------------------------------------------------
// Static methods

std::optional<std::string> MemoryStorage::set_connector_id(std::int32_t id, const std::string& current,
                                                           const std::string& value) {
    std::optional<std::string> result;
    if (id > 0) {
        const std::size_t index = id - 1;
        auto vec = ocpp::v16::utils::split_string(',', current);
        if (index >= vec.size()) {
            // add empty elements
            vec.insert(vec.end(), (index + 1) - vec.size(), {});
        }
        vec[index] = value;
        result = ocpp::v16::utils::to_csl(vec);
    }
    return result;
}

std::optional<std::string> MemoryStorage::get_connector_id(std::int32_t id, const std::string& current) {
    std::optional<std::string> result;
    if (id > 0 && !current.empty()) {
        const std::size_t index = id - 1;
        auto vec = ocpp::v16::utils::split_string(',', current);
        if (index < vec.size()) {
            result = vec[index];
        } else {
            result = std::move(std::string{});
        }
    }
    return result;
}

MemoryStorage::MemoryStorage() {
    vars_internal = required_vars_internal;
    vars_core = required_vars_core;
    vars_firmware_management = required_vars_firmware_management;
    vars_smart_charging = required_vars_smart_charging;
    vars_security = required_vars_security;
    vars_local_auth_list = required_vars_local_auth_list;
    vars_pnc = required_vars_pnc;
    vars_california_pricing = required_vars_california_pricing;
    vars_custom = required_vars_custom;

    vars_additional.clear();
    read_only.clear();
}

void MemoryStorage::apply_full_config() {
    vars_california_pricing.insert(full_vars_california_pricing.begin(), full_vars_california_pricing.end());
}

std::optional<MemoryStorage::Storage::iterator> MemoryStorage::locate_v16(const std::string& name) const {
    // since V16 items are unique, just search through the storage items
    // to locate it
    for (auto& i : vars_list) {
        if (auto it = i->find(name); it != i->end()) {
            return it;
        }
    }
    return std::nullopt;
}

std::optional<std::string> MemoryStorage::get_v16(const std::string& name) const {
    // since V16 items are unique, just search through the storage items
    // to locate it
    auto it = locate_v16(name);
    if (it) {
        return it.value()->second;
    } else {
        std::cout << "get_v16: unable to locate '" << name << "'\n";
    }
    return std::nullopt;
}

std::optional<std::string> MemoryStorage::get_v16(ocpp::v16::keys::valid_keys key) const {
    return get_v16(std::string{ocpp::v16::keys::convert(key)});
}

MemoryStorage::SetVariableStatusEnum MemoryStorage::set_v16(const std::string& name, const std::string& value) {
    // since V16 items are unique, just search through the storage items
    // to locate it
    SetVariableStatusEnum result{SetVariableStatusEnum::UnknownVariable};

    auto it = locate_v16(name);
    if (it) {
        it.value()->second = value;
        result = SetVariableStatusEnum::Accepted;
    } else {
        auto found = keys::convert(name);
        bool create = found.has_value();

        if (name.rfind("MeterPublicKey") == 0) {
            create = true;
        }

        if (create) {
            std::cout << "set_v16: unable to locate '" << name << "' creating\n";
            vars_additional.insert({name, value});
            result = SetVariableStatusEnum::Accepted;

        } else {
            std::cout << "set_v16: unable to locate '" << name << "' ignoring\n";
        }
    }

    return result;
}

MemoryStorage::SetVariableStatusEnum MemoryStorage::set_v16_custom(const std::string& name, const std::string& value) {
    // since V16 items are unique, just search through the storage items
    // to locate it

    auto it = locate_v16(name);
    if (it) {
        it.value()->second = value;
    } else {
        vars_additional.insert({name, value});
    }

    return SetVariableStatusEnum::Accepted;
}

void MemoryStorage::set_readonly(const std::string& key) {
    read_only.insert(key);
}

std::optional<MemoryStorage::MutabilityEnum> MemoryStorage::get_mutability(const std::string& key_str) {
    std::optional<MutabilityEnum> result;

    const auto sv_key_opt = keys::convert(key_str);
    if (sv_key_opt) {
        const auto sv_key = sv_key_opt.value();
        if (sv_key == keys::valid_keys::AuthorizationKey) {
            result = MemoryStorage::MutabilityEnum::WriteOnly;
        } else {
            result = (keys::is_readonly(sv_key)) ? MemoryStorage::MutabilityEnum::ReadOnly
                                                 : MemoryStorage::MutabilityEnum::ReadWrite;
        }
    } else {
        if (const auto it = read_only.find(key_str); it == read_only.end()) {
            // check if key exists (not in the read only list)
            auto found = locate_v16(key_str);
            if (found) {
                result = MemoryStorage::MutabilityEnum::ReadWrite;
            }
        } else {
            result = MemoryStorage::MutabilityEnum::ReadOnly;
        }
    }

    return result;
}

void MemoryStorage::add_supported_measureands_values_list(ocpp::v2::ReportData& data) {
    const auto supported = get_v16(ocpp::v16::keys::valid_keys::SupportedMeasurands);
    if (supported) {
        ocpp::v2::VariableCharacteristics vc;
        vc.valuesList = supported.value();
        data.variableCharacteristics = std::move(vc);
    }
}

void MemoryStorage::add_to_report(std::vector<ocpp::v2::ReportData>& report, const std::string_view& name,
                                  const std::string_view& value) {
    using namespace ocpp::v2::ControllerComponentVariables;
    const auto cv = ocpp::v16::keys::convert_v2(name);
    const std::string name_str{name};
    std::string value_str{value};
    if (cv) {
        auto component = std::get<ocpp::v2::Component>(*cv);
        auto variable = std::get<ocpp::v2::Variable>(*cv);
        auto attribute = std::get<ocpp::v2::AttributeEnum>(*cv);
        ocpp::v2::ReportData data;
        data.component = std::move(component);
        data.variable = std::move(variable);
        ocpp::v2::VariableAttribute va;
        va.type = attribute;
        va.mutability = get_mutability(name_str);
        if (!value_str.empty()) {
            va.value = std::move(value_str);
        }

        const auto key = keys::convert(name);
        if (key == keys::valid_keys::MeterValuesAlignedData) {
            add_supported_measureands_values_list(data);
        } else if (key == keys::valid_keys::StopTxnAlignedData) {
            add_supported_measureands_values_list(data);
        } else if (key == keys::valid_keys::StopTxnSampledData) {
            add_supported_measureands_values_list(data);
        } else if (key == keys::valid_keys::MeterValuesSampledData) {
            add_supported_measureands_values_list(data);
        }

        data.variableAttribute.push_back(std::move(va));
        report.push_back(std::move(data));
    } else {
        if (name_str == "SupportedMeasurands") {
            // ignore
        } else {
            std::cerr << "add_to_report: missing '" << name_str << "'\n";
        }
    }
}

void MemoryStorage::add_to_report(std::vector<ocpp::v2::ReportData>& report, const std::string_view& name,
                                  const std::map<std::string, std::string>& vars) {
    using namespace ocpp::v2::ControllerComponentVariables;
    for (const auto& i : vars) {
        add_to_report(report, i.first, i.second);
    }
}

void MemoryStorage::generate_report(std::vector<ocpp::v2::ReportData>& report) {
    report.clear();
    add_to_report(report, "Internal", vars_internal);
    add_to_report(report, "Core", vars_core);
    add_to_report(report, "FirmwareManagement", vars_firmware_management);
    add_to_report(report, "SmartCharging", vars_smart_charging);
    add_to_report(report, "Security", vars_security);
    add_to_report(report, "LocalAuthListManagement", vars_local_auth_list);
    add_to_report(report, "PnC", vars_pnc);
    add_to_report(report, "CostAndPrice", vars_california_pricing);
    add_to_report(report, "Custom", vars_custom);
}

void MemoryStorage::set(const std::string_view& component, const std::string_view& variable,
                        const std::string_view& value) {
    const std::string variable_v{variable};
    std::cout << "set " << component << '[' << variable << "] = '" << value << "'\n";
    if (component == "Internal") {
        // std::cout << "Internal[" << variable_id.name << "]=" << value << '\n';
        vars_internal[variable_v] = value;
    } else if (component == "Core") {
        // std::cout << "Core[" << variable << "]=" << value << '\n';
        vars_core[variable_v] = value;
    } else if (component == "FirmwareManagement") {
        // std::cout << "FirmwareManagement[" << variable << "]=" << value << '\n';
        vars_firmware_management[variable_v] = value;
    } else if (component == "SmartCharging") {
        // std::cout << "SmartCharging[" << variable << "]=" << value << '\n';
        vars_smart_charging[variable_v] = value;
    } else if (component == "Security") {
        // std::cout << "Security[" << variable << "]=" << value << '\n';
        vars_security[variable_v] = value;
    } else if (component == "LocalAuthListManagement") {
        // std::cout << "LocalAuthListManagement[" << variable << "]=" << value << '\n';
        vars_local_auth_list[variable_v] = value;
    } else if (component == "PnC") {
        // std::cout << "PnC[" << variable << "]=" << value << '\n';
        vars_pnc[variable_v] = value;
    } else if (component == "CostAndPrice") {
        // std::cout << "CostAndPrice[" << variable << "]=" << value << '\n';
        vars_california_pricing[variable_v] = value;
    } else if (component == "Custom") {
        // std::cout << "Custom[" << variable << "]=" << value << '\n';
        vars_custom[variable_v] = value;
    } else {
        std::cerr << "set not implemented for: " << component << '\n';
    }
}

std::string MemoryStorage::get(const std::string_view& component, const std::string_view& variable) {
    Component component_id;
    Variable variable_id;
    std::string result;

    component_id.name = std::string{component};
    variable_id.name = std::string{variable};
    (void)get_variable(component_id, variable_id, AttributeEnum::Actual, result, false);
    return result;
}

void MemoryStorage::clear(const std::string_view& component, const std::string_view& variable) {
    const std::string var{variable};

    if (component == "Internal") {
        vars_internal.erase(var);
    } else if (component == "Core") {
        vars_core.erase(var);
    } else if (component == "FirmwareManagement") {
        vars_firmware_management.erase(var);
    } else if (component == "SmartCharging") {
        vars_smart_charging.erase(var);
    } else if (component == "Security") {
        vars_security.erase(var);
    } else if (component == "LocalAuthListManagement") {
        vars_local_auth_list.erase(var);
    } else if (component == "PnC") {
        vars_pnc.erase(var);
    } else if (component == "CostAndPrice") {
        vars_california_pricing.erase(var);
    } else if (component == "Custom") {
        vars_custom.erase(var);
    } else {
        std::cerr << "clear not implemented for: " << component << '\n';
    }
}

MemoryStorage::GetVariableStatusEnum MemoryStorage::get_variable(const Component& component_id,
                                                                 const Variable& variable_id,
                                                                 const AttributeEnum& attribute_enum,
                                                                 std::string& value, bool allow_write_only) const {
    auto result = GetVariableStatusEnum::UnknownVariable;
    const auto name = enhanced_convert(component_id, variable_id, attribute_enum);
    std::optional<std::string> retrieved;
    // std::cout << "--> " << component_id.name << '[' << variable_id.name << "]\n";
    if (name.empty()) {
        retrieved = get_v16(variable_id.name);
    } else {
        retrieved = get_v16(name);
        if (retrieved) {
            const auto key_opt = v16::keys::convert(name);
            if (key_opt) {
                if (key_opt.value() == v16::keys::valid_keys::ConnectorEvseIds) {
                    if (component_id.evse) {
                        const auto id = component_id.evse.value().id;
                        auto fetched = get_connector_id(id, *retrieved);
                        if (fetched) {
                            retrieved = *fetched;
                        } else {
                            retrieved = "";
                        }
                        std::cout << component_id.name << '[' << variable_id.name << "]." << id << " has value: '"
                                  << *retrieved << "' (" << name << ")\n";
                    } else {
                        std::cerr << "get_value with missing evse: " << component_id.name << '[' << variable_id.name
                                  << "]\n";
                    }
                }
            }
        } else {
            std::cout << component_id.name << '[' << variable_id.name << "] has no value (" << name << ")\n";
        }
    }
    if (retrieved) {
        value = *retrieved;
        result = GetVariableStatusEnum::Accepted;
        // std::cout << component_id.name << '[' << variable_id.name << "]." << id << " has value: '" << *retrieved
        //           << "' (" << name << ")\n";
    } else {
        std::cerr << "get_variable not implemented for: " << component_id.name << ':' << variable_id.name << " ("
                  << name << ")\n";
    }
    return result;
}

MemoryStorage::SetVariableStatusEnum MemoryStorage::set_value(const Component& component_id,
                                                              const Variable& variable_id,
                                                              const AttributeEnum& attribute_enum,
                                                              const std::string& value, const std::string& source,
                                                              bool allow_read_only) {
    const auto key_str = enhanced_convert(component_id, variable_id, attribute_enum);
    MemoryStorage::SetVariableStatusEnum result;
    if (!key_str.empty()) {
        const auto key_opt = v16::keys::convert(key_str);
        std::string store_value = value;
        result = MemoryStorage::SetVariableStatusEnum::Accepted;
        if (key_opt) {
            if (key_opt.value() == v16::keys::valid_keys::ConnectorEvseIds) {
                result = MemoryStorage::SetVariableStatusEnum::Rejected;
                auto retrieved = get_v16(v16::keys::valid_keys::ConnectorEvseIds);
                store_value.clear();
                if (retrieved) {
                    store_value = *retrieved;
                }
                if (component_id.evse) {
                    const auto id = component_id.evse.value().id;
                    auto updated = set_connector_id(id, store_value, value);
                    if (updated) {
                        store_value = *updated;
                        result = MemoryStorage::SetVariableStatusEnum::Accepted;
                    } else {
                        std::cerr << "set_value with invalid evse: " << component_id.name << '[' << variable_id.name
                                  << "] " << id << '\n';
                    }
                } else {
                    std::cerr << "set_value with missing evse: " << component_id.name << '[' << variable_id.name
                              << "]\n";
                }
            }
        }
        if (result == MemoryStorage::SetVariableStatusEnum::Accepted) {
            result = set_v16(key_str, store_value);
            std::cout << component_id.name << '[' << variable_id.name << "] = '" << store_value << "' (" << key_str
                      << ") " << (int)result << '\n';
        }
    } else {
        result = set_v16_custom(variable_id.name, value);
        std::cout << component_id.name << '[' << variable_id.name << "] = '" << value << "' " << (int)result << '\n';
    }
    return result;
}

MemoryStorage::SetVariableStatusEnum MemoryStorage::set_read_only_value(const Component& component_id,
                                                                        const Variable& variable_id,
                                                                        const AttributeEnum& attribute_enum,
                                                                        const std::string& value,
                                                                        const std::string& source) {
    return set_value(component_id, variable_id, attribute_enum, value, source);
}

std::optional<MemoryStorage::MutabilityEnum> MemoryStorage::get_mutability(const Component& component_id,
                                                                           const Variable& variable_id,
                                                                           const AttributeEnum& attribute_enum) {
    auto key_str_opt = keys::convert_v2(component_id, variable_id, attribute_enum);
    auto key_str = key_str_opt.value_or(variable_id.name);
    return get_mutability(key_str);
}

std::optional<MemoryStorage::VariableMetaData> MemoryStorage::get_variable_meta_data(const Component& component_id,
                                                                                     const Variable& variable_id) {
    std::optional<MemoryStorage::VariableMetaData> result;
    const auto key_str = keys::convert_v2(component_id, variable_id, ocpp::v2::AttributeEnum::Actual);
    const auto retrieved = get_v16(key_str.value_or(""));
    if (retrieved) {
        MemoryStorage::VariableMetaData md;
        md.characteristics.dataType = v2::DataEnum::string;
        md.characteristics.supportsMonitoring = false;
        result = std::move(md);
    } else {
        std::cerr << "get_variable_meta_data not implemented for: " << component_id.name << ':' << variable_id.name
                  << " (" << key_str.value_or("") << ")\n";
    }
    return result;
}

std::vector<MemoryStorage::ReportData> MemoryStorage::get_base_report_data(const ReportBaseEnum& report_base) {
    if (report_base == v2::ReportBaseEnum::ConfigurationInventory) {
        std::vector<MemoryStorage::ReportData> result;
        generate_report(result);
        return result;
    }
    return {};
}

std::vector<MemoryStorage::ReportData>
MemoryStorage::get_custom_report_data(const std::optional<std::vector<ComponentVariable>>& component_variables,
                                      const std::optional<std::vector<ComponentCriterionEnum>>& component_criteria) {
    std::vector<MemoryStorage::ReportData> result;
    if (component_variables) {
        for (const auto& component : component_variables.value()) {
            if (component.variable) {
                const auto name = ocpp::v16::keys::convert_v2(component.component, component.variable.value(),
                                                              ocpp::v2::AttributeEnum::Actual);
                if (name) {
                    const auto retrieved = get_v16(name.value());
                    if (retrieved) {
                        add_to_report(result, name.value(), *retrieved);
                    }
                }
            }
        }
    }
    return result;
}

std::vector<MemoryStorage::SetMonitoringResult>
MemoryStorage::set_monitors(const std::vector<SetMonitoringData>& requests, const VariableMonitorType type) {
    return {};
}

bool MemoryStorage::update_monitor_reference(std::int32_t monitor_id, const std::string& reference_value) {
    return false;
}

std::vector<MemoryStorage::VariableMonitoringPeriodic> MemoryStorage::get_periodic_monitors() {
    return {};
}

std::vector<MemoryStorage::MonitoringData>
MemoryStorage::get_monitors(const std::vector<MonitoringCriterionEnum>& criteria,
                            const std::vector<ComponentVariable>& component_variables) {
    return {};
}

std::vector<MemoryStorage::ClearMonitoringResult> MemoryStorage::clear_monitors(const std::vector<int>& request_ids,
                                                                                bool allow_protected) {
    return {};
}

std::int32_t MemoryStorage::clear_custom_monitors() {
    return -1;
}

void MemoryStorage::register_variable_listener(
    std::function<void(const std::unordered_map<std::int64_t, VariableMonitoringMeta>& monitors,
                       const Component& component, const Variable& variable,
                       const VariableCharacteristics& characteristics, const VariableAttribute& attribute,
                       const std::string& value_previous, const std::string& value_current)>&& listener) {
}

void MemoryStorage::register_monitor_listener(
    std::function<void(const VariableMonitoringMeta& updated_monitor, const Component& component,
                       const Variable& variable, const VariableCharacteristics& characteristics,
                       const VariableAttribute& attribute, const std::string& current_value)>&& listener) {
}

void MemoryStorage::check_integrity(const std::map<std::int32_t, std::int32_t>& evse_connector_structure) {
}

} // namespace ocpp::v16::stubs
