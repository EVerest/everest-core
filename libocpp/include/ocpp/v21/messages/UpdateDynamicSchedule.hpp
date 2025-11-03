// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_UPDATEDYNAMICSCHEDULE_HPP
#define OCPP_V21_UPDATEDYNAMICSCHEDULE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP UpdateDynamicSchedule message
struct UpdateDynamicScheduleRequest : public ocpp::Message {
    int32_t chargingProfileId;
    ChargingScheduleUpdate scheduleUpdate;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this UpdateDynamicSchedule message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given UpdateDynamicScheduleRequest \p k to a given json object \p j
void to_json(json& j, const UpdateDynamicScheduleRequest& k);

/// \brief Conversion from a given json object \p j to a given UpdateDynamicScheduleRequest \p k
void from_json(const json& j, UpdateDynamicScheduleRequest& k);

/// \brief Writes the string representation of the given UpdateDynamicScheduleRequest \p k to the given output stream \p
/// os \returns an output stream with the UpdateDynamicScheduleRequest written to
std::ostream& operator<<(std::ostream& os, const UpdateDynamicScheduleRequest& k);

/// \brief Contains a OCPP UpdateDynamicScheduleResponse message
struct UpdateDynamicScheduleResponse : public ocpp::Message {
    ChargingProfileStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this UpdateDynamicScheduleResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given UpdateDynamicScheduleResponse \p k to a given json object \p j
void to_json(json& j, const UpdateDynamicScheduleResponse& k);

/// \brief Conversion from a given json object \p j to a given UpdateDynamicScheduleResponse \p k
void from_json(const json& j, UpdateDynamicScheduleResponse& k);

/// \brief Writes the string representation of the given UpdateDynamicScheduleResponse \p k to the given output stream
/// \p os \returns an output stream with the UpdateDynamicScheduleResponse written to
std::ostream& operator<<(std::ostream& os, const UpdateDynamicScheduleResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_UPDATEDYNAMICSCHEDULE_HPP
