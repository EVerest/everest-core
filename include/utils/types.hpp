// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_TYPES_HPP
#define UTILS_TYPES_HPP

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
using Handler = std::function<void(json)>;
using StringHandler = std::function<void(std::string)>;

enum class HandlerType {
    Call,
    Result,
    SubscribeVar,
    SubscribeError,
    ClearErrorRequest,
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
    bool telemetry_enabled;
};

struct TelemetryConfig {
    int id;
};

struct Requirement {
    Requirement(const std::string& requirement_id_, size_t index_);
    bool operator<(const Requirement& rhs) const;
    std::string id;
    size_t index;
};

struct ImplementationIdentifier {
    ImplementationIdentifier(const std::string& module_id_, const std::string& implementation_id_);
    std::string to_string() const;
    std::string module_id;
    std::string implementation_id;
};
bool operator==(const ImplementationIdentifier& lhs, const ImplementationIdentifier& rhs);
bool operator!=(const ImplementationIdentifier& lhs, const ImplementationIdentifier& rhs);

#define EVCALLBACK(function) [](auto&& PH1) { function(std::forward<decltype(PH1)>(PH1)); }

#endif // UTILS_TYPES_HPP
