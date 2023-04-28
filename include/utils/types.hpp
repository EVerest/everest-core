// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_TYPES_HPP
#define UTILS_TYPES_HPP

#include <map>
#include <string>
#include <vector>
#include <filesystem>

#include <boost/any.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using Value = boost::any;
using Parameters = std::map<std::string, boost::any>;
using Result = boost::optional<boost::any>;
using JsonCommand = std::function<json(json)>;
using Command = std::function<boost::optional<boost::any>(Parameters)>;
using ArgumentType = std::vector<std::string>;
using Arguments = std::map<std::string, ArgumentType>;
using ReturnType = std::vector<std::string>;
using JsonCallback = std::function<void(json)>;
using ValueCallback = std::function<void(Value)>;
using ConfigEntry = boost::variant<std::string, bool, int, double>;
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
    ExternalMQTT,
    Unknown
};

struct TypedHandler {
    std::string name;
    std::string id;
    HandlerType type;
    std::shared_ptr<Handler> handler;

    TypedHandler(const std::string& name, const std::string& id, HandlerType type, std::shared_ptr<Handler> handler) :
        name(name), id(id), type(type), handler(handler) {
    }

    TypedHandler(const std::string& name, HandlerType type, std::shared_ptr<Handler> handler) :
        TypedHandler(name, "", type, handler) {
    }

    TypedHandler(HandlerType type, std::shared_ptr<Handler> handler) : TypedHandler("", "", type, handler) {
    }
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
    Requirement(const std::string& requirement_id, size_t index) : id(requirement_id), index(index){};
    bool operator<(const Requirement& rhs) const {
        if (id < rhs.id) {
            return true;
        } else if (id == rhs.id) {
            return (index < rhs.index);
        } else {
            return false;
        }
    }
    std::string id;
    size_t index;
};

#define EVCALLBACK(function) [](auto&& PH1) { function(std::forward<decltype(PH1)>(PH1)); }

#endif // UTILS_TYPES_HPP
