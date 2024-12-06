// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_TYPES_HPP
#define UTILS_TYPES_HPP

#include <cstddef>
#include <filesystem>
#include <fmt/core.h>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using Value = json;
using Parameters = json;
using Result = std::optional<json>;
using JsonCommand = std::function<json(json)>;
using Command = std::function<Result(Parameters)>;
using ArgumentType = std::vector<std::string>;
using Arguments = std::map<std::string, ArgumentType>;
using ReturnType = std::vector<std::string>;
using JsonCallback = std::function<void(json)>;
using ValueCallback = std::function<void(Value)>;
using ConfigEntry = std::variant<std::string, bool, int, double>;
using ConfigMap = std::map<std::string, ConfigEntry>;
using ModuleConfigs = std::map<std::string, ConfigMap>;
using Array = json::array_t;
using Object = json::object_t;
// TODO (aw): can we pass the handler arguments by const ref?
using Handler = std::function<void(const std::string&, json)>;
using StringHandler = std::function<void(std::string)>;
using StringPairHandler = std::function<void(const std::string& topic, const std::string& data)>;

enum class HandlerType {
    Call,
    Result,
    SubscribeVar,
    SubscribeError,
    ClearErrorRequest,
    GetConfig,
    ExternalMQTT,
    Unknown
};

struct TypedHandler {
    std::string name;
    std::string id;
    HandlerType type;
    std::shared_ptr<Handler> handler;

    TypedHandler(const std::string& name_, const std::string& id_, HandlerType type_,
                 std::shared_ptr<Handler> handler_);
    TypedHandler(const std::string& name_, HandlerType type_, std::shared_ptr<Handler> handler_);
    TypedHandler(HandlerType type_, std::shared_ptr<Handler> handler_);
};

using Token = std::shared_ptr<TypedHandler>;

/// \brief MQTT Quality of service
enum class QOS {
    QOS0, ///< At most once delivery
    QOS1, ///< At least once delivery
    QOS2  ///< Exactly once delivery
};

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

struct ModuleInfo {
    struct Paths {
        std::filesystem::path etc;
        std::filesystem::path libexec;
        std::filesystem::path share;
    };

    std::string name;
    std::vector<std::string> authors;
    std::string license;
    std::string id;
    Paths paths;
    bool telemetry_enabled;
    bool global_errors_enabled;
    std::optional<Mapping> mapping;
};

struct TelemetryConfig {
    int id;
    explicit TelemetryConfig(int id) : id(id) {
    }
};

struct Requirement {
    std::string id;
    size_t index = 0;
};

bool operator<(const Requirement& lhs, const Requirement& rhs);

/// \brief A Fulfillment relates a Requirement to its connected implementation, identified via its module and
/// implementation id.
struct Fulfillment {
    std::string module_id;
    std::string implementation_id;
    Requirement requirement;
};

/// \brief Contains everything that's needed to initialize a requirement in user code
struct RequirementInitializer {
    Requirement requirement;
    Fulfillment fulfillment;
    std::optional<Mapping> mapping;
};

using RequirementInitialization = std::map<std::string, std::vector<RequirementInitializer>>;

struct ImplementationIdentifier {
    ImplementationIdentifier(const std::string& module_id_, const std::string& implementation_id_,
                             std::optional<Mapping> mapping_ = std::nullopt);
    std::string to_string() const;
    std::string module_id;
    std::string implementation_id;
    std::optional<Mapping> mapping;
};
bool operator==(const ImplementationIdentifier& lhs, const ImplementationIdentifier& rhs);
bool operator!=(const ImplementationIdentifier& lhs, const ImplementationIdentifier& rhs);

NLOHMANN_JSON_NAMESPACE_BEGIN
template <> struct adl_serializer<Mapping> {
    static void to_json(json& j, const Mapping& m);
    static Mapping from_json(const json& j);
};
template <> struct adl_serializer<TelemetryConfig> {
    static void to_json(json& j, const TelemetryConfig& t);
    static TelemetryConfig from_json(const json& j);
};
template <> struct adl_serializer<ModuleTierMappings> {
    static void to_json(json& j, const ModuleTierMappings& m);
    static ModuleTierMappings from_json(const json& j);
};
NLOHMANN_JSON_NAMESPACE_END

#define EVCALLBACK(function) [](auto&& PH1) { function(std::forward<decltype(PH1)>(PH1)); }

namespace Everest {
struct BootException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
} // namespace Everest

#endif // UTILS_TYPES_HPP
