// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/PullDynamicScheduleUpdate.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string PullDynamicScheduleUpdateRequest::get_type() const {
    return "PullDynamicScheduleUpdate";
}

void to_json(json& j, const PullDynamicScheduleUpdateRequest& k) {
    // the required parts of the message
    j = json{
        {"chargingProfileId", k.chargingProfileId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, PullDynamicScheduleUpdateRequest& k) {
    // the required parts of the message
    k.chargingProfileId = j.at("chargingProfileId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given PullDynamicScheduleUpdateRequest \p k to the given output
/// stream \p os \returns an output stream with the PullDynamicScheduleUpdateRequest written to
std::ostream& operator<<(std::ostream& os, const PullDynamicScheduleUpdateRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string PullDynamicScheduleUpdateResponse::get_type() const {
    return "PullDynamicScheduleUpdateResponse";
}

void to_json(json& j, const PullDynamicScheduleUpdateResponse& k) {
    // the required parts of the message
    j = json{
        {"status", ocpp::v2::conversions::charging_profile_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.scheduleUpdate) {
        j["scheduleUpdate"] = k.scheduleUpdate.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, PullDynamicScheduleUpdateResponse& k) {
    // the required parts of the message
    k.status = ocpp::v2::conversions::string_to_charging_profile_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("scheduleUpdate")) {
        k.scheduleUpdate.emplace(j.at("scheduleUpdate"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given PullDynamicScheduleUpdateResponse \p k to the given output
/// stream \p os \returns an output stream with the PullDynamicScheduleUpdateResponse written to
std::ostream& operator<<(std::ostream& os, const PullDynamicScheduleUpdateResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
