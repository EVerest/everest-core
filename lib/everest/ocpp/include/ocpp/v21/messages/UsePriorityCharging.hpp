// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_USEPRIORITYCHARGING_HPP
#define OCPP_V21_USEPRIORITYCHARGING_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP UsePriorityCharging message
struct UsePriorityChargingRequest : public ocpp::Message {
    CiString<36> transactionId;
    bool activate;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this UsePriorityCharging message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given UsePriorityChargingRequest \p k to a given json object \p j
void to_json(json& j, const UsePriorityChargingRequest& k);

/// \brief Conversion from a given json object \p j to a given UsePriorityChargingRequest \p k
void from_json(const json& j, UsePriorityChargingRequest& k);

/// \brief Writes the string representation of the given UsePriorityChargingRequest \p k to the given output stream \p
/// os
/// \returns an output stream with the UsePriorityChargingRequest written to
std::ostream& operator<<(std::ostream& os, const UsePriorityChargingRequest& k);

/// \brief Contains a OCPP UsePriorityChargingResponse message
struct UsePriorityChargingResponse : public ocpp::Message {
    PriorityChargingStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this UsePriorityChargingResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given UsePriorityChargingResponse \p k to a given json object \p j
void to_json(json& j, const UsePriorityChargingResponse& k);

/// \brief Conversion from a given json object \p j to a given UsePriorityChargingResponse \p k
void from_json(const json& j, UsePriorityChargingResponse& k);

/// \brief Writes the string representation of the given UsePriorityChargingResponse \p k to the given output stream \p
/// os
/// \returns an output stream with the UsePriorityChargingResponse written to
std::ostream& operator<<(std::ostream& os, const UsePriorityChargingResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_USEPRIORITYCHARGING_HPP
