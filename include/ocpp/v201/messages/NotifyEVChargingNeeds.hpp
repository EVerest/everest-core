// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_NOTIFYEVCHARGINGNEEDS_HPP
#define OCPP_V201_NOTIFYEVCHARGINGNEEDS_HPP

#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP NotifyEVChargingNeeds message
struct NotifyEVChargingNeedsRequest : public ocpp::Message {
    ChargingNeeds chargingNeeds;
    int32_t evseId;
    std::optional<CustomData> customData;
    std::optional<int32_t> maxScheduleTuples;

    /// \brief Provides the type of this NotifyEVChargingNeeds message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given NotifyEVChargingNeedsRequest \p k to a given json object \p j
void to_json(json& j, const NotifyEVChargingNeedsRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyEVChargingNeedsRequest \p k
void from_json(const json& j, NotifyEVChargingNeedsRequest& k);

/// \brief Writes the string representation of the given NotifyEVChargingNeedsRequest \p k to the given output stream \p
/// os \returns an output stream with the NotifyEVChargingNeedsRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyEVChargingNeedsRequest& k);

/// \brief Contains a OCPP NotifyEVChargingNeedsResponse message
struct NotifyEVChargingNeedsResponse : public ocpp::Message {
    NotifyEVChargingNeedsStatusEnum status;
    std::optional<CustomData> customData;
    std::optional<StatusInfo> statusInfo;

    /// \brief Provides the type of this NotifyEVChargingNeedsResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given NotifyEVChargingNeedsResponse \p k to a given json object \p j
void to_json(json& j, const NotifyEVChargingNeedsResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyEVChargingNeedsResponse \p k
void from_json(const json& j, NotifyEVChargingNeedsResponse& k);

/// \brief Writes the string representation of the given NotifyEVChargingNeedsResponse \p k to the given output stream
/// \p os \returns an output stream with the NotifyEVChargingNeedsResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyEVChargingNeedsResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_NOTIFYEVCHARGINGNEEDS_HPP
