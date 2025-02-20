// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v201/messages/GetCompositeSchedule.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string GetCompositeScheduleRequest::get_type() const {
    return "GetCompositeSchedule";
}

void to_json(json& j, const GetCompositeScheduleRequest& k) {
    // the required parts of the message
    j = json{
        {"duration", k.duration},
        {"evseId", k.evseId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.chargingRateUnit) {
        j["chargingRateUnit"] = conversions::charging_rate_unit_enum_to_string(k.chargingRateUnit.value());
    }
}

void from_json(const json& j, GetCompositeScheduleRequest& k) {
    // the required parts of the message
    k.duration = j.at("duration");
    k.evseId = j.at("evseId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("chargingRateUnit")) {
        k.chargingRateUnit.emplace(conversions::string_to_charging_rate_unit_enum(j.at("chargingRateUnit")));
    }
}

/// \brief Writes the string representation of the given GetCompositeScheduleRequest \p k to the given output stream \p
/// os \returns an output stream with the GetCompositeScheduleRequest written to
std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetCompositeScheduleResponse::get_type() const {
    return "GetCompositeScheduleResponse";
}

void to_json(json& j, const GetCompositeScheduleResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::generic_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.schedule) {
        j["schedule"] = k.schedule.value();
    }
}

void from_json(const json& j, GetCompositeScheduleResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_generic_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("schedule")) {
        k.schedule.emplace(j.at("schedule"));
    }
}

/// \brief Writes the string representation of the given GetCompositeScheduleResponse \p k to the given output stream \p
/// os \returns an output stream with the GetCompositeScheduleResponse written to
std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
