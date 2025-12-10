// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_NOTIFYPRIORITYCHARGING_HPP
#define OCPP_V21_NOTIFYPRIORITYCHARGING_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP NotifyPriorityCharging message
struct NotifyPriorityChargingRequest : public ocpp::Message {
    CiString<36> transactionId;
    bool activated;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyPriorityCharging message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyPriorityChargingRequest \p k to a given json object \p j
void to_json(json& j, const NotifyPriorityChargingRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyPriorityChargingRequest \p k
void from_json(const json& j, NotifyPriorityChargingRequest& k);

/// \brief Writes the string representation of the given NotifyPriorityChargingRequest \p k to the given output stream
/// \p os
/// \returns an output stream with the NotifyPriorityChargingRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyPriorityChargingRequest& k);

/// \brief Contains a OCPP NotifyPriorityChargingResponse message
struct NotifyPriorityChargingResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyPriorityChargingResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyPriorityChargingResponse \p k to a given json object \p j
void to_json(json& j, const NotifyPriorityChargingResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyPriorityChargingResponse \p k
void from_json(const json& j, NotifyPriorityChargingResponse& k);

/// \brief Writes the string representation of the given NotifyPriorityChargingResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the NotifyPriorityChargingResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyPriorityChargingResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_NOTIFYPRIORITYCHARGING_HPP
