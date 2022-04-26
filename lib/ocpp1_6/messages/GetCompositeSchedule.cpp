// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <boost/optional/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/messages/GetCompositeSchedule.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

std::string GetCompositeScheduleRequest::get_type() const {
    return "GetCompositeSchedule";
}

void to_json(json& j, const GetCompositeScheduleRequest& k) {
    // the required parts of the message
    j = json{
        {"connectorId", k.connectorId},
        {"duration", k.duration},
    };
    // the optional parts of the message
    if (k.chargingRateUnit) {
        j["chargingRateUnit"] = conversions::charging_rate_unit_to_string(k.chargingRateUnit.value());
    }
}

void from_json(const json& j, GetCompositeScheduleRequest& k) {
    // the required parts of the message
    k.connectorId = j.at("connectorId");
    k.duration = j.at("duration");

    // the optional parts of the message
    if (j.contains("chargingRateUnit")) {
        k.chargingRateUnit.emplace(conversions::string_to_charging_rate_unit(j.at("chargingRateUnit")));
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
        {"status", conversions::get_composite_schedule_status_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.connectorId) {
        j["connectorId"] = k.connectorId.value();
    }
    if (k.scheduleStart) {
        j["scheduleStart"] = k.scheduleStart.value().to_rfc3339();
    }
    if (k.chargingSchedule) {
        j["chargingSchedule"] = k.chargingSchedule.value();
    }
}

void from_json(const json& j, GetCompositeScheduleResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_get_composite_schedule_status(j.at("status"));

    // the optional parts of the message
    if (j.contains("connectorId")) {
        k.connectorId.emplace(j.at("connectorId"));
    }
    if (j.contains("scheduleStart")) {
        k.scheduleStart.emplace(DateTime(std::string(j.at("scheduleStart"))));
    }
    if (j.contains("chargingSchedule")) {
        k.chargingSchedule.emplace(j.at("chargingSchedule"));
    }
}

/// \brief Writes the string representation of the given GetCompositeScheduleResponse \p k to the given output stream \p
/// os \returns an output stream with the GetCompositeScheduleResponse written to
std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace ocpp1_6
