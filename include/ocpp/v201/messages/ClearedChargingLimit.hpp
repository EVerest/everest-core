// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_CLEAREDCHARGINGLIMIT_HPP
#define OCPP_V201_CLEAREDCHARGINGLIMIT_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP ClearedChargingLimit message
struct ClearedChargingLimitRequest : public ocpp::Message {
    ChargingLimitSourceEnum chargingLimitSource;
    std::optional<CustomData> customData;
    std::optional<int32_t> evseId;

    /// \brief Provides the type of this ClearedChargingLimit message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ClearedChargingLimitRequest \p k to a given json object \p j
void to_json(json& j, const ClearedChargingLimitRequest& k);

/// \brief Conversion from a given json object \p j to a given ClearedChargingLimitRequest \p k
void from_json(const json& j, ClearedChargingLimitRequest& k);

/// \brief Writes the string representation of the given ClearedChargingLimitRequest \p k to the given output stream \p
/// os \returns an output stream with the ClearedChargingLimitRequest written to
std::ostream& operator<<(std::ostream& os, const ClearedChargingLimitRequest& k);

/// \brief Contains a OCPP ClearedChargingLimitResponse message
struct ClearedChargingLimitResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ClearedChargingLimitResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given ClearedChargingLimitResponse \p k to a given json object \p j
void to_json(json& j, const ClearedChargingLimitResponse& k);

/// \brief Conversion from a given json object \p j to a given ClearedChargingLimitResponse \p k
void from_json(const json& j, ClearedChargingLimitResponse& k);

/// \brief Writes the string representation of the given ClearedChargingLimitResponse \p k to the given output stream \p
/// os \returns an output stream with the ClearedChargingLimitResponse written to
std::ostream& operator<<(std::ostream& os, const ClearedChargingLimitResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_CLEAREDCHARGINGLIMIT_HPP
