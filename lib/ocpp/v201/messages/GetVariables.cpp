// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/v201/messages/GetVariables.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string GetVariablesRequest::get_type() const {
    return "GetVariables";
}

void to_json(json& j, const GetVariablesRequest& k) {
    // the required parts of the message
    j = json{
        {"getVariableData", k.getVariableData},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetVariablesRequest& k) {
    // the required parts of the message
    for (auto val : j.at("getVariableData")) {
        k.getVariableData.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetVariablesRequest \p k to the given output stream \p os
/// \returns an output stream with the GetVariablesRequest written to
std::ostream& operator<<(std::ostream& os, const GetVariablesRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetVariablesResponse::get_type() const {
    return "GetVariablesResponse";
}

void to_json(json& j, const GetVariablesResponse& k) {
    // the required parts of the message
    j = json{
        {"getVariableResult", k.getVariableResult},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetVariablesResponse& k) {
    // the required parts of the message
    for (auto val : j.at("getVariableResult")) {
        k.getVariableResult.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetVariablesResponse \p k to the given output stream \p os
/// \returns an output stream with the GetVariablesResponse written to
std::ostream& operator<<(std::ostream& os, const GetVariablesResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
