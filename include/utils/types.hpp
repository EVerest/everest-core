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

#include <utils/config/types.hpp>

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
using ConfigMap = std::map<std::string, everest::config::ConfigEntry>;
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
    bool telemetry_enabled = false;
    bool global_errors_enabled = false;
    std::optional<Mapping> mapping;
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
template <> struct adl_serializer<Requirement> {
    static void to_json(json& j, const Requirement& r);
    static Requirement from_json(const json& j);
};

template <> struct adl_serializer<Fulfillment> {
    static void to_json(json& j, const Fulfillment& f);
    static Fulfillment from_json(const json& j);
};
NLOHMANN_JSON_NAMESPACE_END

#define EVCALLBACK(function) [](auto&& PH1) { function(std::forward<decltype(PH1)>(PH1)); }

namespace Everest {
inline constexpr int mqtt_get_config_timeout_ms = 5000;

/// \brief Events than can happen related to commands
enum class CmdEvent {
    MessageParsingFailed,   ///< Parsing of the message to the command handler has failed
    SchemaValidationFailed, ///< Schema validation of arguments or result has failed
    HandlerException,       ///< An exception was thrown during handling of the command in user code
    Timeout,  ///< A timeout happened when calling the command (eg. when the callee doesn't respond to the caller)
    Shutdown, ///< EVerest is shutting down during a command call
    NotReady  ///< EVerest / the callee is not yet ready but received a command call already from another module
};

struct CmdResultError {
    CmdEvent event;
    std::string msg;
    std::exception_ptr ex;
};

struct CmdResult {
    std::optional<json> result;
    std::optional<CmdResultError> error;
};

struct MQTTRequest {
    std::string response_topic;
    QOS qos = QOS::QOS2;
    std::chrono::milliseconds timeout = std::chrono::milliseconds(mqtt_get_config_timeout_ms);
    std::optional<std::string> request_topic;
    std::optional<std::string> request_data;
};
} // namespace Everest

#endif // UTILS_TYPES_HPP
