// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_GETCOMPOSITESCHEDULE_HPP
#define OCPP1_6_GETCOMPOSITESCHEDULE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 GetCompositeSchedule message
struct GetCompositeScheduleRequest : public Message {
    int32_t connectorId;
    int32_t duration;
    boost::optional<ChargingRateUnit> chargingRateUnit;

    /// \brief Provides the type of this GetCompositeSchedule message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "GetCompositeSchedule";
    }

    /// \brief Conversion from a given GetCompositeScheduleRequest \p k to a given json object \p j
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

    /// \brief Conversion from a given json object \p j to a given GetCompositeScheduleRequest \p k
    friend void from_json(const json& j, GetCompositeScheduleRequest& k) {
        // the required parts of the message
        k.connectorId = j.at("connectorId");
        k.duration = j.at("duration");

        // the optional parts of the message
        if (j.contains("chargingRateUnit")) {
            k.chargingRateUnit.emplace(conversions::string_to_charging_rate_unit(j.at("chargingRateUnit")));
        }
    }

    /// \brief Writes the string representation of the given GetCompositeScheduleRequest \p k to the given output stream
    /// \p os \returns an output stream with the GetCompositeScheduleRequest written to
    friend std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 GetCompositeScheduleResponse message
struct GetCompositeScheduleResponse : public Message {
    GetCompositeScheduleStatus status;
    boost::optional<int32_t> connectorId;
    boost::optional<DateTime> scheduleStart;
    boost::optional<ChargingSchedule> chargingSchedule;

    /// \brief Provides the type of this GetCompositeScheduleResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "GetCompositeScheduleResponse";
    }

    /// \brief Conversion from a given GetCompositeScheduleResponse \p k to a given json object \p j
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

    /// \brief Conversion from a given json object \p j to a given GetCompositeScheduleResponse \p k
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

    /// \brief Writes the string representation of the given GetCompositeScheduleResponse \p k to the given output
    /// stream \p os \returns an output stream with the GetCompositeScheduleResponse written to
    friend std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_GETCOMPOSITESCHEDULE_HPP
