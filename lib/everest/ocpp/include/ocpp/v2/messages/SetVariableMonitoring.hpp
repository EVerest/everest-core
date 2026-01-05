// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_SETVARIABLEMONITORING_HPP
#define OCPP_V2_SETVARIABLEMONITORING_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP SetVariableMonitoring message
struct SetVariableMonitoringRequest : public ocpp::Message {
    std::vector<SetMonitoringData> setMonitoringData;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetVariableMonitoring message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetVariableMonitoringRequest \p k to a given json object \p j
void to_json(json& j, const SetVariableMonitoringRequest& k);

/// \brief Conversion from a given json object \p j to a given SetVariableMonitoringRequest \p k
void from_json(const json& j, SetVariableMonitoringRequest& k);

/// \brief Writes the string representation of the given SetVariableMonitoringRequest \p k to the given output stream \p
/// os
/// \returns an output stream with the SetVariableMonitoringRequest written to
std::ostream& operator<<(std::ostream& os, const SetVariableMonitoringRequest& k);

/// \brief Contains a OCPP SetVariableMonitoringResponse message
struct SetVariableMonitoringResponse : public ocpp::Message {
    std::vector<SetMonitoringResult> setMonitoringResult;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetVariableMonitoringResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given SetVariableMonitoringResponse \p k to a given json object \p j
void to_json(json& j, const SetVariableMonitoringResponse& k);

/// \brief Conversion from a given json object \p j to a given SetVariableMonitoringResponse \p k
void from_json(const json& j, SetVariableMonitoringResponse& k);

/// \brief Writes the string representation of the given SetVariableMonitoringResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the SetVariableMonitoringResponse written to
std::ostream& operator<<(std::ostream& os, const SetVariableMonitoringResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_SETVARIABLEMONITORING_HPP
