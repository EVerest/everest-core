// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V201_SETMONITORINGLEVEL_HPP
#define OCPP_V201_SETMONITORINGLEVEL_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/ocpp_enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP SetMonitoringLevel message
struct SetMonitoringLevelRequest : public ocpp::Message {
    int32_t severity;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this SetMonitoringLevel message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SetMonitoringLevelRequest \p k to a given json object \p j
void to_json(json& j, const SetMonitoringLevelRequest& k);

/// \brief Conversion from a given json object \p j to a given SetMonitoringLevelRequest \p k
void from_json(const json& j, SetMonitoringLevelRequest& k);

/// \brief Writes the string representation of the given SetMonitoringLevelRequest \p k to the given output stream \p os
/// \returns an output stream with the SetMonitoringLevelRequest written to
std::ostream& operator<<(std::ostream& os, const SetMonitoringLevelRequest& k);

/// \brief Contains a OCPP SetMonitoringLevelResponse message
struct SetMonitoringLevelResponse : public ocpp::Message {
    GenericStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this SetMonitoringLevelResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given SetMonitoringLevelResponse \p k to a given json object \p j
void to_json(json& j, const SetMonitoringLevelResponse& k);

/// \brief Conversion from a given json object \p j to a given SetMonitoringLevelResponse \p k
void from_json(const json& j, SetMonitoringLevelResponse& k);

/// \brief Writes the string representation of the given SetMonitoringLevelResponse \p k to the given output stream \p
/// os \returns an output stream with the SetMonitoringLevelResponse written to
std::ostream& operator<<(std::ostream& os, const SetMonitoringLevelResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_SETMONITORINGLEVEL_HPP
