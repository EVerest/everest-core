// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/ClearDERControl.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string ClearDERControlRequest::get_type() const {
    return "ClearDERControl";
}

void to_json(json& j, const ClearDERControlRequest& k) {
    // the required parts of the message
    j = json{
        {"isDefault", k.isDefault},
    };
    // the optional parts of the message
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

void from_json(const json& j, ClearDERControlRequest& k) {
    // the required parts of the message
    k.isDefault = j.at("isDefault");

    // the optional parts of the message
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

/// \brief Writes the string representation of the given ClearDERControlRequest \p k to the given output stream \p os
/// \returns an output stream with the ClearDERControlRequest written to
std::ostream& operator<<(std::ostream& os, const ClearDERControlRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ClearDERControlResponse::get_type() const {
    return "ClearDERControlResponse";
}

void to_json(json& j, const ClearDERControlResponse& k) {
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

void from_json(const json& j, ClearDERControlResponse& k) {
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

/// \brief Writes the string representation of the given ClearDERControlResponse \p k to the given output stream \p os
/// \returns an output stream with the ClearDERControlResponse written to
std::ostream& operator<<(std::ostream& os, const ClearDERControlResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
