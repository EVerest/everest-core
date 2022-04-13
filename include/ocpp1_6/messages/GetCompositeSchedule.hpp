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
    std::string get_type() const;
};

/// \brief Conversion from a given GetCompositeScheduleRequest \p k to a given json object \p j
void to_json(json& j, const GetCompositeScheduleRequest& k);

/// \brief Conversion from a given json object \p j to a given GetCompositeScheduleRequest \p k
void from_json(const json& j, GetCompositeScheduleRequest& k);

/// \brief Writes the string representation of the given GetCompositeScheduleRequest \p k to the given output stream \p
/// os \returns an output stream with the GetCompositeScheduleRequest written to
std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleRequest& k);

/// \brief Contains a OCPP 1.6 GetCompositeScheduleResponse message
struct GetCompositeScheduleResponse : public Message {
    GetCompositeScheduleStatus status;
    boost::optional<int32_t> connectorId;
    boost::optional<DateTime> scheduleStart;
    boost::optional<ChargingSchedule> chargingSchedule;

    /// \brief Provides the type of this GetCompositeScheduleResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetCompositeScheduleResponse \p k to a given json object \p j
void to_json(json& j, const GetCompositeScheduleResponse& k);

/// \brief Conversion from a given json object \p j to a given GetCompositeScheduleResponse \p k
void from_json(const json& j, GetCompositeScheduleResponse& k);

/// \brief Writes the string representation of the given GetCompositeScheduleResponse \p k to the given output stream \p
/// os \returns an output stream with the GetCompositeScheduleResponse written to
std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_GETCOMPOSITESCHEDULE_HPP
