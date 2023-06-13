// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v16/messages/ClearChargingProfile.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string ClearChargingProfileRequest::get_type() const {
    return "ClearChargingProfile";
}

void to_json(json& j, const ClearChargingProfileRequest& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
    if (k.id) {
        j["id"] = k.id.value();
    }
    if (k.connectorId) {
        j["connectorId"] = k.connectorId.value();
    }
    if (k.chargingProfilePurpose) {
        j["chargingProfilePurpose"] =
            conversions::charging_profile_purpose_type_to_string(k.chargingProfilePurpose.value());
    }
    if (k.stackLevel) {
        j["stackLevel"] = k.stackLevel.value();
    }
}

void from_json(const json& j, ClearChargingProfileRequest& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("id")) {
        k.id.emplace(j.at("id"));
    }
    if (j.contains("connectorId")) {
        k.connectorId.emplace(j.at("connectorId"));
    }
    if (j.contains("chargingProfilePurpose")) {
        k.chargingProfilePurpose.emplace(
            conversions::string_to_charging_profile_purpose_type(j.at("chargingProfilePurpose")));
    }
    if (j.contains("stackLevel")) {
        k.stackLevel.emplace(j.at("stackLevel"));
    }
}

/// \brief Writes the string representation of the given ClearChargingProfileRequest \p k to the given output stream \p
/// os \returns an output stream with the ClearChargingProfileRequest written to
std::ostream& operator<<(std::ostream& os, const ClearChargingProfileRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ClearChargingProfileResponse::get_type() const {
    return "ClearChargingProfileResponse";
}

void to_json(json& j, const ClearChargingProfileResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::clear_charging_profile_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, ClearChargingProfileResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_clear_charging_profile_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given ClearChargingProfileResponse \p k to the given output stream \p
/// os \returns an output stream with the ClearChargingProfileResponse written to
std::ostream& operator<<(std::ostream& os, const ClearChargingProfileResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
