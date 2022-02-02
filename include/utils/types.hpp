// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_TYPES_HPP
#define UTILS_TYPES_HPP

#include <map>
#include <string>
#include <vector>

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
using ConfigEntry = boost::variant<std::string, double, int, bool>;
using ConfigMap = std::map<std::string, ConfigEntry>;
using ModuleConfigs = std::map<std::string, ConfigMap>;
struct ModuleInfo {
    std::string name;
    std::vector<std::string> authors;
    std::string license;
    std::string id;
};
using Array = json::array_t;
using Object = json::object_t;
// TODO (aw): can we pass the handler arguments by const ref?
using Handler = std::function<void(json)>;
using StringHandler = std::function<void(std::string)>;
using Token = std::shared_ptr<Handler>;

#define EVCALLBACK(function) [](auto&& PH1) { function(std::forward<decltype(PH1)>(PH1)); }

#endif // UTILS_TYPES_HPP
