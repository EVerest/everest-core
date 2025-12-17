// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/UpdateDynamicSchedule.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string UpdateDynamicScheduleRequest::get_type() const {
    return "UpdateDynamicSchedule";
}

void to_json(json& j, const UpdateDynamicScheduleRequest& k) {
    // the required parts of the message
    j = json{
        {"chargingProfileId", k.chargingProfileId},
        {"scheduleUpdate", k.scheduleUpdate},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, UpdateDynamicScheduleRequest& k) {
    // the required parts of the message
    k.chargingProfileId = j.at("chargingProfileId");
    k.scheduleUpdate = j.at("scheduleUpdate");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given UpdateDynamicScheduleRequest \p k to the given output stream \p
/// os
/// \returns an output stream with the UpdateDynamicScheduleRequest written to
std::ostream& operator<<(std::ostream& os, const UpdateDynamicScheduleRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string UpdateDynamicScheduleResponse::get_type() const {
    return "UpdateDynamicScheduleResponse";
}

void to_json(json& j, const UpdateDynamicScheduleResponse& k) {
    // the required parts of the message
    j = json{
        {"status", ocpp::v2::conversions::charging_profile_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, UpdateDynamicScheduleResponse& k) {
    // the required parts of the message
    k.status = ocpp::v2::conversions::string_to_charging_profile_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given UpdateDynamicScheduleResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the UpdateDynamicScheduleResponse written to
std::ostream& operator<<(std::ostream& os, const UpdateDynamicScheduleResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
