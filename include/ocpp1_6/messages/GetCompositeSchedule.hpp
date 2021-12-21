// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_GETCOMPOSITESCHEDULE_HPP
#define OCPP1_6_GETCOMPOSITESCHEDULE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct GetCompositeScheduleRequest : public Message {
    int32_t connectorId;
    int32_t duration;
    boost::optional<ChargingRateUnit> chargingRateUnit;

    std::string get_type() const {
        return "GetCompositeSchedule";
    }

    friend void to_json(json& j, const GetCompositeScheduleRequest& k) {
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

    friend void from_json(const json& j, GetCompositeScheduleRequest& k) {
        // the required parts of the message
        k.connectorId = j.at("connectorId");
        k.duration = j.at("duration");

        // the optional parts of the message
        if (j.contains("chargingRateUnit")) {
            k.chargingRateUnit.emplace(conversions::string_to_charging_rate_unit(j.at("chargingRateUnit")));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct GetCompositeScheduleResponse : public Message {
    GetCompositeScheduleStatus status;
    boost::optional<int32_t> connectorId;
    boost::optional<DateTime> scheduleStart;
    boost::optional<ChargingSchedule> chargingSchedule;

    std::string get_type() const {
        return "GetCompositeScheduleResponse";
    }

    friend void to_json(json& j, const GetCompositeScheduleResponse& k) {
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

    friend void from_json(const json& j, GetCompositeScheduleResponse& k) {
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

    friend std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_GETCOMPOSITESCHEDULE_HPP
