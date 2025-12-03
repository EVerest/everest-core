// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_PULLDYNAMICSCHEDULEUPDATE_HPP
#define OCPP_V21_PULLDYNAMICSCHEDULEUPDATE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP PullDynamicScheduleUpdate message
struct PullDynamicScheduleUpdateRequest : public ocpp::Message {
    int32_t chargingProfileId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this PullDynamicScheduleUpdate message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given PullDynamicScheduleUpdateRequest \p k to a given json object \p j
void to_json(json& j, const PullDynamicScheduleUpdateRequest& k);

/// \brief Conversion from a given json object \p j to a given PullDynamicScheduleUpdateRequest \p k
void from_json(const json& j, PullDynamicScheduleUpdateRequest& k);

/// \brief Writes the string representation of the given PullDynamicScheduleUpdateRequest \p k to the given output
/// stream \p os \returns an output stream with the PullDynamicScheduleUpdateRequest written to
std::ostream& operator<<(std::ostream& os, const PullDynamicScheduleUpdateRequest& k);

/// \brief Contains a OCPP PullDynamicScheduleUpdateResponse message
struct PullDynamicScheduleUpdateResponse : public ocpp::Message {
    ChargingProfileStatusEnum status;
    std::optional<ChargingScheduleUpdate> scheduleUpdate;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this PullDynamicScheduleUpdateResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given PullDynamicScheduleUpdateResponse \p k to a given json object \p j
void to_json(json& j, const PullDynamicScheduleUpdateResponse& k);

/// \brief Conversion from a given json object \p j to a given PullDynamicScheduleUpdateResponse \p k
void from_json(const json& j, PullDynamicScheduleUpdateResponse& k);

/// \brief Writes the string representation of the given PullDynamicScheduleUpdateResponse \p k to the given output
/// stream \p os \returns an output stream with the PullDynamicScheduleUpdateResponse written to
std::ostream& operator<<(std::ostream& os, const PullDynamicScheduleUpdateResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_PULLDYNAMICSCHEDULEUPDATE_HPP
