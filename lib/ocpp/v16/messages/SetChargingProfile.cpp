// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v16/messages/SetChargingProfile.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string SetChargingProfileRequest::get_type() const {
    return "SetChargingProfile";
}

void to_json(json& j, const SetChargingProfileRequest& k) {
    // the required parts of the message
    j = json{
        {"connectorId", k.connectorId},
        {"csChargingProfiles", k.csChargingProfiles},
    };
    // the optional parts of the message
}

void from_json(const json& j, SetChargingProfileRequest& k) {
    // the required parts of the message
    k.connectorId = j.at("connectorId");
    k.csChargingProfiles = j.at("csChargingProfiles");

    // the optional parts of the message
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
        {"status", conversions::charging_profile_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, SetChargingProfileResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_charging_profile_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given SetChargingProfileResponse \p k to the given output stream \p
/// os \returns an output stream with the SetChargingProfileResponse written to
std::ostream& operator<<(std::ostream& os, const SetChargingProfileResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
