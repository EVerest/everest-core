// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_NOTIFYEVCHARGINGNEEDS_HPP
#define OCPP_V2_NOTIFYEVCHARGINGNEEDS_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP NotifyEVChargingNeeds message
struct NotifyEVChargingNeedsRequest : public ocpp::Message {
    int32_t evseId;
    ChargingNeeds chargingNeeds;
    std::optional<int32_t> maxScheduleTuples;
    std::optional<ocpp::DateTime> timestamp;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyEVChargingNeeds message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
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
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyEVChargingNeedsResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyEVChargingNeedsResponse \p k to a given json object \p j
void to_json(json& j, const NotifyEVChargingNeedsResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyEVChargingNeedsResponse \p k
void from_json(const json& j, NotifyEVChargingNeedsResponse& k);

/// \brief Writes the string representation of the given NotifyEVChargingNeedsResponse \p k to the given output stream
/// \p os \returns an output stream with the NotifyEVChargingNeedsResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyEVChargingNeedsResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_NOTIFYEVCHARGINGNEEDS_HPP
