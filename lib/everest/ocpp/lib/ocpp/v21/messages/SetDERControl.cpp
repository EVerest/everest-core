// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/SetDERControl.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string SetDERControlRequest::get_type() const {
    return "SetDERControl";
}

void to_json(json& j, const SetDERControlRequest& k) {
    // the required parts of the message
    j = json{
        {"isDefault", k.isDefault},
        {"controlId", k.controlId},
        {"controlType", ocpp::v2::conversions::dercontrol_enum_to_string(k.controlType)},
    };
    // the optional parts of the message
    if (k.curve) {
        j["curve"] = k.curve.value();
    }
    if (k.enterService) {
        j["enterService"] = k.enterService.value();
    }
    if (k.fixedPFAbsorb) {
        j["fixedPFAbsorb"] = k.fixedPFAbsorb.value();
    }
    if (k.fixedPFInject) {
        j["fixedPFInject"] = k.fixedPFInject.value();
    }
    if (k.fixedVar) {
        j["fixedVar"] = k.fixedVar.value();
    }
    if (k.freqDroop) {
        j["freqDroop"] = k.freqDroop.value();
    }
    if (k.gradient) {
        j["gradient"] = k.gradient.value();
    }
    if (k.limitMaxDischarge) {
        j["limitMaxDischarge"] = k.limitMaxDischarge.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetDERControlRequest& k) {
    // the required parts of the message
    k.isDefault = j.at("isDefault");
    k.controlId = j.at("controlId");
    k.controlType = ocpp::v2::conversions::string_to_dercontrol_enum(j.at("controlType"));

    // the optional parts of the message
    if (j.contains("curve")) {
        k.curve.emplace(j.at("curve"));
    }
    if (j.contains("enterService")) {
        k.enterService.emplace(j.at("enterService"));
    }
    if (j.contains("fixedPFAbsorb")) {
        k.fixedPFAbsorb.emplace(j.at("fixedPFAbsorb"));
    }
    if (j.contains("fixedPFInject")) {
        k.fixedPFInject.emplace(j.at("fixedPFInject"));
    }
    if (j.contains("fixedVar")) {
        k.fixedVar.emplace(j.at("fixedVar"));
    }
    if (j.contains("freqDroop")) {
        k.freqDroop.emplace(j.at("freqDroop"));
    }
    if (j.contains("gradient")) {
        k.gradient.emplace(j.at("gradient"));
    }
    if (j.contains("limitMaxDischarge")) {
        k.limitMaxDischarge.emplace(j.at("limitMaxDischarge"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetDERControlRequest \p k to the given output stream \p os
/// \returns an output stream with the SetDERControlRequest written to
std::ostream& operator<<(std::ostream& os, const SetDERControlRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SetDERControlResponse::get_type() const {
    return "SetDERControlResponse";
}

void to_json(json& j, const SetDERControlResponse& k) {
    // the required parts of the message
    j = json{
        {"status", ocpp::v2::conversions::dercontrol_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.supersededIds) {
        j["supersededIds"] = json::array();
        for (const auto& val : k.supersededIds.value()) {
            j["supersededIds"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetDERControlResponse& k) {
    // the required parts of the message
    k.status = ocpp::v2::conversions::string_to_dercontrol_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("supersededIds")) {
        const json& arr = j.at("supersededIds");
        std::vector<CiString<36>> vec;
        for (const auto& val : arr) {
            vec.push_back(val);
        }
        k.supersededIds.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetDERControlResponse \p k to the given output stream \p os
/// \returns an output stream with the SetDERControlResponse written to
std::ostream& operator<<(std::ostream& os, const SetDERControlResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
