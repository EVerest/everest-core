// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/SetVariables.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string SetVariablesRequest::get_type() const {
    return "SetVariables";
}

void to_json(json& j, const SetVariablesRequest& k) {
    // the required parts of the message
    j = json{
        {"setVariableData", k.setVariableData},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetVariablesRequest& k) {
    // the required parts of the message
    for (auto val : j.at("setVariableData")) {
        k.setVariableData.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetVariablesRequest \p k to the given output stream \p os
/// \returns an output stream with the SetVariablesRequest written to
std::ostream& operator<<(std::ostream& os, const SetVariablesRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SetVariablesResponse::get_type() const {
    return "SetVariablesResponse";
}

void to_json(json& j, const SetVariablesResponse& k) {
    // the required parts of the message
    j = json{
        {"setVariableResult", k.setVariableResult},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetVariablesResponse& k) {
    // the required parts of the message
    for (auto val : j.at("setVariableResult")) {
        k.setVariableResult.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetVariablesResponse \p k to the given output stream \p os
/// \returns an output stream with the SetVariablesResponse written to
std::ostream& operator<<(std::ostream& os, const SetVariablesResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
