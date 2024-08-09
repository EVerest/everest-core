// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V201_GETCOMPOSITESCHEDULE_HPP
#define OCPP_V201_GETCOMPOSITESCHEDULE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP GetCompositeSchedule message
struct GetCompositeScheduleRequest : public ocpp::Message {
    int32_t duration;
    int32_t evseId;
    std::optional<CustomData> customData;
    std::optional<ChargingRateUnitEnum> chargingRateUnit;

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

/// \brief Contains a OCPP GetCompositeScheduleResponse message
struct GetCompositeScheduleResponse : public ocpp::Message {
    GenericStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;
    std::optional<CompositeSchedule> schedule;

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

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_GETCOMPOSITESCHEDULE_HPP
