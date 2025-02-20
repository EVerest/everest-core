// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/GetLocalListVersion.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string GetLocalListVersionRequest::get_type() const {
    return "GetLocalListVersion";
}

void to_json(json& j, const GetLocalListVersionRequest& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetLocalListVersionRequest& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetLocalListVersionRequest \p k to the given output stream \p
/// os \returns an output stream with the GetLocalListVersionRequest written to
std::ostream& operator<<(std::ostream& os, const GetLocalListVersionRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetLocalListVersionResponse::get_type() const {
    return "GetLocalListVersionResponse";
}

void to_json(json& j, const GetLocalListVersionResponse& k) {
    // the required parts of the message
    j = json{
        {"versionNumber", k.versionNumber},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetLocalListVersionResponse& k) {
    // the required parts of the message
    k.versionNumber = j.at("versionNumber");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetLocalListVersionResponse \p k to the given output stream \p
/// os \returns an output stream with the GetLocalListVersionResponse written to
std::ostream& operator<<(std::ostream& os, const GetLocalListVersionResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
