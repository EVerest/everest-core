// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_NOTIFYDERALARM_HPP
#define OCPP_V21_NOTIFYDERALARM_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP NotifyDERAlarm message
struct NotifyDERAlarmRequest : public ocpp::Message {
    DERControlEnum controlType;
    ocpp::DateTime timestamp;
    std::optional<GridEventFaultEnum> gridEventFault;
    std::optional<bool> alarmEnded;
    std::optional<CiString<200>> extraInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyDERAlarm message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyDERAlarmRequest \p k to a given json object \p j
void to_json(json& j, const NotifyDERAlarmRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyDERAlarmRequest \p k
void from_json(const json& j, NotifyDERAlarmRequest& k);

/// \brief Writes the string representation of the given NotifyDERAlarmRequest \p k to the given output stream \p os
/// \returns an output stream with the NotifyDERAlarmRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyDERAlarmRequest& k);

/// \brief Contains a OCPP NotifyDERAlarmResponse message
struct NotifyDERAlarmResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyDERAlarmResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyDERAlarmResponse \p k to a given json object \p j
void to_json(json& j, const NotifyDERAlarmResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyDERAlarmResponse \p k
void from_json(const json& j, NotifyDERAlarmResponse& k);

/// \brief Writes the string representation of the given NotifyDERAlarmResponse \p k to the given output stream \p os
/// \returns an output stream with the NotifyDERAlarmResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyDERAlarmResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_NOTIFYDERALARM_HPP
