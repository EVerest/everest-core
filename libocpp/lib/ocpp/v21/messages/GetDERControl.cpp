// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/GetDERControl.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string GetDERControlRequest::get_type() const {
    return "GetDERControl";
}

void to_json(json& j, const GetDERControlRequest& k) {
    // the required parts of the message
    j = json{
        {"requestId", k.requestId},
    };
    // the optional parts of the message
    if (k.isDefault) {
        j["isDefault"] = k.isDefault.value();
    }
    if (k.controlType) {
        j["controlType"] = ocpp::v2::conversions::dercontrol_enum_to_string(k.controlType.value());
    }
    if (k.controlId) {
        j["controlId"] = k.controlId.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetDERControlRequest& k) {
    // the required parts of the message
    k.requestId = j.at("requestId");

    // the optional parts of the message
    if (j.contains("isDefault")) {
        k.isDefault.emplace(j.at("isDefault"));
    }
    if (j.contains("controlType")) {
        k.controlType.emplace(ocpp::v2::conversions::string_to_dercontrol_enum(j.at("controlType")));
    }
    if (j.contains("controlId")) {
        k.controlId.emplace(j.at("controlId"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetDERControlRequest \p k to the given output stream \p os
/// \returns an output stream with the GetDERControlRequest written to
std::ostream& operator<<(std::ostream& os, const GetDERControlRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetDERControlResponse::get_type() const {
    return "GetDERControlResponse";
}

void to_json(json& j, const GetDERControlResponse& k) {
    // the required parts of the message
    j = json{
        {"status", ocpp::v2::conversions::dercontrol_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetDERControlResponse& k) {
    // the required parts of the message
    k.status = ocpp::v2::conversions::string_to_dercontrol_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetDERControlResponse \p k to the given output stream \p os
/// \returns an output stream with the GetDERControlResponse written to
std::ostream& operator<<(std::ostream& os, const GetDERControlResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
