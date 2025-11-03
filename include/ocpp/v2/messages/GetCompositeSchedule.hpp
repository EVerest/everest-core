// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_GETCOMPOSITESCHEDULE_HPP
#define OCPP_V2_GETCOMPOSITESCHEDULE_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP GetCompositeSchedule message
struct GetCompositeScheduleRequest : public ocpp::Message {
    int32_t duration;
    int32_t evseId;
    std::optional<ChargingRateUnitEnum> chargingRateUnit;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetCompositeSchedule message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
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
    std::optional<StatusInfo> statusInfo;
    std::optional<CompositeSchedule> schedule;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetCompositeScheduleResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetCompositeScheduleResponse \p k to a given json object \p j
void to_json(json& j, const GetCompositeScheduleResponse& k);

/// \brief Conversion from a given json object \p j to a given GetCompositeScheduleResponse \p k
void from_json(const json& j, GetCompositeScheduleResponse& k);

/// \brief Writes the string representation of the given GetCompositeScheduleResponse \p k to the given output stream \p
/// os \returns an output stream with the GetCompositeScheduleResponse written to
std::ostream& operator<<(std::ostream& os, const GetCompositeScheduleResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_GETCOMPOSITESCHEDULE_HPP
