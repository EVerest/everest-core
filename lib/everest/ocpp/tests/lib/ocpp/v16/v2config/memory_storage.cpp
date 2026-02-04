// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "memory_storage.hpp"

#include <iterator>
#include <ocpp/v16/known_keys.hpp>
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

void add_to_report(std::vector<ocpp::v2::ReportData>& report, const std::string_view& name,
                   const std::map<std::string, std::string>& vars) {
    for (const auto& i : vars) {
        const auto cv = ocpp::v16::keys::convert_v2(i.first);
        if (cv) {
            auto component = std::get<ocpp::v2::Component>(*cv);
            auto variable = std::get<ocpp::v2::Variable>(*cv);
            ocpp::v2::ReportData data;
            data.component = std::move(component);
            data.variable = std::move(variable);
            report.push_back(std::move(data));
        } else {
            std::cerr << "add_to_report: missing '" << i.first << "'\n";
        }
    }
}

void generate_report(std::vector<ocpp::v2::ReportData>& report) {
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

} // namespace

namespace ocpp::v16::stubs {

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

        if (name.find("MeterPublicKey[") == 0) {
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
    const auto name = keys::convert_v2(component_id, variable_id);
    std::optional<std::string> retrieved;
    if (name.empty()) {
        retrieved = get_v16(variable_id.name);
    } else {
        retrieved = get_v16(name);
    }
    if (retrieved) {
        value = *retrieved;
        result = GetVariableStatusEnum::Accepted;
        std::cout << component_id.name << '[' << name << "] has value: '" << *retrieved << "'\n";
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
    const auto key_str = keys::convert_v2(component_id, variable_id);
    MemoryStorage::SetVariableStatusEnum result;
    if (key_str.empty()) {
        result = set_v16_custom(variable_id.name, value);
    } else {
        result = set_v16(key_str, value);
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
    std::optional<MemoryStorage::MutabilityEnum> result;
    auto key_str = keys::convert_v2(component_id, variable_id);
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
            const std::string name = (key_str.empty()) ? std::string{variable_id.name} : key_str;
            auto found = locate_v16(name);
            if (found) {
                result = MemoryStorage::MutabilityEnum::ReadWrite;
            }
        } else {
            result = MemoryStorage::MutabilityEnum::ReadOnly;
        }
    }
    return result;
}

std::optional<MemoryStorage::VariableMetaData> MemoryStorage::get_variable_meta_data(const Component& component_id,
                                                                                     const Variable& variable_id) {
    std::optional<MemoryStorage::VariableMetaData> result;
    const auto key_str = keys::convert_v2(component_id, variable_id);
    const auto retrieved = get_v16(key_str);
    if (retrieved) {
        MemoryStorage::VariableMetaData md;
        md.characteristics.dataType = v2::DataEnum::string;
        md.characteristics.supportsMonitoring = false;
        result = std::move(md);
    } else {
        std::cerr << "get_variable_meta_data not implemented for: " << component_id.name << ':' << variable_id.name
                  << " (" << key_str << ")\n";
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
    return {};
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
