// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/v201/messages/ClearChargingProfile.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string ClearChargingProfileRequest::get_type() const {
    return "ClearChargingProfile";
}

void to_json(json& j, const ClearChargingProfileRequest& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.chargingProfileId) {
        j["chargingProfileId"] = k.chargingProfileId.value();
    }
    if (k.chargingProfileCriteria) {
        j["chargingProfileCriteria"] = k.chargingProfileCriteria.value();
    }
}

void from_json(const json& j, ClearChargingProfileRequest& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("chargingProfileId")) {
        k.chargingProfileId.emplace(j.at("chargingProfileId"));
    }
    if (j.contains("chargingProfileCriteria")) {
        k.chargingProfileCriteria.emplace(j.at("chargingProfileCriteria"));
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
        {"status", conversions::clear_charging_profile_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, ClearChargingProfileResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_clear_charging_profile_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given ClearChargingProfileResponse \p k to the given output stream \p
/// os \returns an output stream with the ClearChargingProfileResponse written to
std::ostream& operator<<(std::ostream& os, const ClearChargingProfileResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
