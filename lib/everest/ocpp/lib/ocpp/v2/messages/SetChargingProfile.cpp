// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/SetChargingProfile.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string SetChargingProfileRequest::get_type() const {
    return "SetChargingProfile";
}

void to_json(json& j, const SetChargingProfileRequest& k) {
    // the required parts of the message
    j = json{
        {"evseId", k.evseId},
        {"chargingProfile", k.chargingProfile},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetChargingProfileRequest& k) {
    // the required parts of the message
    k.evseId = j.at("evseId");
    k.chargingProfile = j.at("chargingProfile");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetChargingProfileRequest \p k to the given output stream \p os
/// \returns an output stream with the SetChargingProfileRequest written to
std::ostream& operator<<(std::ostream& os, const SetChargingProfileRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SetChargingProfileResponse::get_type() const {
    return "SetChargingProfileResponse";
}

void to_json(json& j, const SetChargingProfileResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::charging_profile_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetChargingProfileResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_charging_profile_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetChargingProfileResponse \p k to the given output stream \p
/// os
/// \returns an output stream with the SetChargingProfileResponse written to
std::ostream& operator<<(std::ostream& os, const SetChargingProfileResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
