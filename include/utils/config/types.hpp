// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <utils/config/settings.hpp>

/// \brief A Mapping that can be used to map a module or implementation to a specific EVSE or optionally to a Connector
struct Mapping {
    int evse;                     ///< The EVSE id
    std::optional<int> connector; ///< An optional Connector id

    Mapping(int evse) : evse(evse) {
    }

    Mapping(int evse, int connector) : evse(evse), connector(connector) {
    }
};

/// \brief Writes the string representation of the given Mapping \p mapping to the given output stream \p os
/// \returns an output stream with the Mapping written to
inline std::ostream& operator<<(std::ostream& os, const Mapping& mapping) {
    os << "Mapping(evse: " << mapping.evse;
    if (mapping.connector.has_value()) {
        os << ", connector: " << mapping.connector.value();
    }
    os << ")";

    return os;
}

/// \brief Writes the string representation of the given Mapping \p mapping to the given output stream \p os
/// \returns an output stream with the Mapping written to
inline std::ostream& operator<<(std::ostream& os, const std::optional<Mapping>& mapping) {
    if (mapping.has_value()) {
        os << mapping.value();
    } else {
        os << "Mapping(charging station)";
    }

    return os;
}

/// \brief A 3 tier mapping for a module and its individual implementations
struct ModuleTierMappings {
    std::optional<Mapping> module; ///< Mapping of the whole module to an EVSE id and optional Connector id. If this is
                                   ///< absent the module is assumed to be mapped to the whole charging station
    std::unordered_map<std::string, std::optional<Mapping>>
        implementations; ///< Mappings for the individual implementations of the module
};

/// \brief A Requirement of an EVerest module
struct Requirement {
    std::string id;
    size_t index = 0;
};

bool operator<(const Requirement& lhs, const Requirement& rhs);

/// \brief A Fulfillment relates a Requirement to its connected implementation, identified via its module and
/// implementation id.
struct Fulfillment {
    std::string module_id;         // the id of the module that fulfills the requirement
    std::string implementation_id; // the id of the implementation that fulfills the requirement
    Requirement requirement;       // the requirement of the module that is fulfilled
};

struct TelemetryConfig {
    int id;
    explicit TelemetryConfig(int id) : id(id) {
    }
};

namespace everest::config {

namespace fs = std::filesystem;

struct ConfigurationParameter;
struct ModuleConfig;
using ModuleId = std::string;
using RequirementId = std::string;
using ConfigEntry = std::variant<std::string, bool, int, double>;
using ImplementationIdentifier = std::string;
using ModuleConnections = std::map<RequirementId, std::vector<Fulfillment>>;
using ModuleConfigurations = std::map<ModuleId, ModuleConfig>;
using ModuleConfigurationParameters = std::map<ImplementationIdentifier, std::vector<ConfigurationParameter>>;

enum class Mutability {
    ReadOnly,
    ReadWrite,
    WriteOnly
};

enum class Datatype {
    String,
    Decimal,
    Integer,
    Boolean,
    Path
};

/// \brief Struct that contains the characteristics of a configuration parameter including its datatype, mutability and
/// unit
struct ConfigurationParameterCharacteristics {
    Datatype datatype;
    Mutability mutability;
    std::optional<std::string> unit;
};

/// \brief Struct that contains the name, value and characteristics of a configuration parameter
struct ConfigurationParameter {
    std::string name;
    ConfigEntry value;
    ConfigurationParameterCharacteristics characteristics;

    bool validate_type() const;
};

/// \brief Struct that contains the configuration of an EVerest module
struct ModuleConfig {
    bool standalone;
    std::string module_name;
    std::string module_id;
    std::optional<std::string> capabilities;
    ModuleConfigurationParameters configuration_parameters; // contains: config_module and config_implementations
                                                            // as well as the upcoming "config" key
    bool telemetry_enabled;
    std::optional<TelemetryConfig> telemetry_config;
    ModuleConnections connections;
    ModuleTierMappings mapping;
};

Datatype string_to_datatype(const std::string& str);
std::string datatype_to_string(const Datatype datatype);

Mutability string_to_mutability(const std::string& str);
std::string mutability_to_string(const Mutability mutability);

} // namespace everest::config

NLOHMANN_JSON_NAMESPACE_BEGIN

template <> struct adl_serializer<everest::config::ModuleConfig> {
    static void to_json(nlohmann::json& j, const everest::config::ModuleConfig& m);
    static void from_json(const nlohmann::json& j, everest::config::ModuleConfig& m);
};

template <> struct adl_serializer<everest::config::ConfigurationParameterCharacteristics> {
    static void to_json(nlohmann::json& j, const everest::config::ConfigurationParameterCharacteristics& c);
    static void from_json(const nlohmann::json& j, everest::config::ConfigurationParameterCharacteristics& c);
};

template <> struct adl_serializer<everest::config::ConfigEntry> {
    static void to_json(nlohmann::json& j, const everest::config::ConfigEntry& entry);
    static void from_json(const nlohmann::json& j, everest::config::ConfigEntry& entry);
};

template <> struct adl_serializer<everest::config::ConfigurationParameter> {
    static void to_json(nlohmann::json& j, const everest::config::ConfigurationParameter& p);
    static void from_json(const nlohmann::json& j, everest::config::ConfigurationParameter& p);
};

NLOHMANN_JSON_NAMESPACE_END
