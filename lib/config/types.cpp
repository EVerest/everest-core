// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <nlohmann/json.hpp>
#include <utils/config/types.hpp>
#include <utils/types.hpp>

using json = nlohmann::json;

bool operator<(const Requirement& lhs, const Requirement& rhs) {
    if (lhs.id < rhs.id) {
        return true;
    } else if (lhs.id == rhs.id) {
        return (lhs.index < rhs.index);
    } else {
        return false;
    }
}

namespace everest::config {

bool ConfigurationParameter::validate_type() const {
    return std::visit(
        [&](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;

            switch (characteristics.datatype) {
            case Datatype::String:
                return std::is_same_v<T, std::string>;

            case Datatype::Boolean:
                return std::is_same_v<T, bool>;

            case Datatype::Integer:
                return std::is_same_v<T, int>;

            case Datatype::Decimal:
                return std::is_same_v<T, double> || std::is_same_v<T, int>;
                // allow integers where decimal is expected

            default:
                return false;
            }
        },
        value);
}

Datatype string_to_datatype(const std::string& str) {
    if (str == "string") {
        return Datatype::String;
    } else if (str == "number") {
        return Datatype::Decimal;
    } else if (str == "integer") {
        return Datatype::Integer;
    } else if (str == "boolean" or "bool") {
        return Datatype::Boolean;
    } else if (str == "path") {
        return Datatype::Path;
    }
    throw std::out_of_range("Could not convert: " + str + " to Datatype");
}

std::string datatype_to_string(const Datatype datatype) {
    switch (datatype) {
    case Datatype::String:
        return "string";
    case Datatype::Decimal:
        return "number";
    case Datatype::Integer:
        return "integer";
    case Datatype::Boolean:
        return "bool";
    case Datatype::Path:
        return "path";
    }
    throw std::out_of_range("Could not convert Datatype to string");
}

Mutability string_to_mutability(const std::string& str) {
    if (str == "ReadOnly") {
        return Mutability::ReadOnly;
    } else if (str == "ReadWrite") {
        return Mutability::ReadWrite;
    } else if (str == "WriteOnly") {
        return Mutability::WriteOnly;
    }
    throw std::out_of_range("Could not convert: " + str + " to Mutability");
}

std::string mutability_to_string(const Mutability mutability) {
    switch (mutability) {
    case Mutability::ReadOnly:
        return "ReadOnly";
    case Mutability::ReadWrite:
        return "ReadWrite";
    case Mutability::WriteOnly:
        return "WriteOnly";
    }
    throw std::out_of_range("Could not convert Mutability to string");
}

} // namespace everest::config

NLOHMANN_JSON_NAMESPACE_BEGIN

void adl_serializer<everest::config::ConfigurationParameterCharacteristics>::to_json(
    nlohmann::json& j, const everest::config::ConfigurationParameterCharacteristics& c) {
    j["datatype"] = datatype_to_string(c.datatype);
    j["mutability"] = mutability_to_string(c.mutability);
    if (c.unit.has_value()) {
        j["unit"] = c.unit.value();
    }
}

void adl_serializer<everest::config::ConfigurationParameterCharacteristics>::from_json(
    const nlohmann::json& j, everest::config::ConfigurationParameterCharacteristics& c) {
    c.datatype = everest::config::string_to_datatype(j.at("datatype").get<std::string>());
    c.mutability = everest::config::string_to_mutability(j.at("mutability").get<std::string>());
    if (j.contains("unit")) {
        c.unit = j.at("unit").get<std::string>();
    }
}

void adl_serializer<everest::config::ConfigEntry>::to_json(nlohmann::json& j,
                                                           const everest::config::ConfigEntry& entry) {
    std::visit([&j](auto&& arg) { j = arg; }, entry);
}

void adl_serializer<everest::config::ConfigEntry>::from_json(const nlohmann::json& j,
                                                             everest::config::ConfigEntry& entry) {
    if (j.is_boolean()) {
        entry = j.get<bool>();
    } else if (j.is_number_integer()) {
        entry = j.get<int>();
    } else if (j.is_number_float()) {
        entry = j.get<double>();
    } else if (j.is_string()) {
        entry = j.get<std::string>();
    } else {
        throw std::runtime_error("Unsupported JSON type for ConfigEntry");
    }
}

void adl_serializer<everest::config::ConfigurationParameter>::to_json(
    nlohmann::json& j, const everest::config::ConfigurationParameter& p) {
    j["name"] = p.name;
    j["value"] = p.value;
    j["characteristics"] = p.characteristics;
}

void adl_serializer<everest::config::ConfigurationParameter>::from_json(const nlohmann::json& j,
                                                                        everest::config::ConfigurationParameter& p) {
    p.name = j.at("name").get<std::string>();
    p.value = j.at("value").get<everest::config::ConfigEntry>();
    p.characteristics = j.at("characteristics").get<everest::config::ConfigurationParameterCharacteristics>();
}

void adl_serializer<everest::config::ModuleConfig>::to_json(nlohmann::json& j, const everest::config::ModuleConfig& m) {
    j["standalone"] = m.standalone;
    j["module_name"] = m.module_name;
    j["module_id"] = m.module_id;
    if (m.capabilities.has_value()) {
        j["capabilities"] = m.capabilities.value();
    }
    if (m.telemetry_config.has_value()) {
        j["telemetry_config"] = m.telemetry_config.value();
    }
    j["configuration_parameters"] = m.configuration_parameters;
    j["telemetry_enabled"] = m.telemetry_enabled;
    j["connections"] = m.connections;
    j["mapping"] = m.mapping;
}

void adl_serializer<everest::config::ModuleConfig>::from_json(const nlohmann::json& j,
                                                              everest::config::ModuleConfig& m) {
    m.standalone = j.at("standalone").get<bool>();
    m.module_name = j.at("module_name").get<std::string>();
    m.module_id = j.at("module_id").get<std::string>();
    if (j.contains("capabilities")) {
        m.capabilities = j.at("capabilities").get<std::string>();
    }
    if (j.contains("telemetry_config")) {
        m.telemetry_config = j.at("telemetry_config").get<TelemetryConfig>();
    }
    m.configuration_parameters = j.at("configuration_parameters").get<everest::config::ModuleConfigurationParameters>();
    m.telemetry_enabled = j.at("telemetry_enabled").get<bool>();
    m.connections = j.at("connections").get<everest::config::ModuleConnections>();
    m.mapping = j.at("mapping").get<ModuleTierMappings>();
}

NLOHMANN_JSON_NAMESPACE_END
