// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_CLEAREDCHARGINGLIMIT_HPP
#define OCPP_V2_CLEAREDCHARGINGLIMIT_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP ClearedChargingLimit message
struct ClearedChargingLimitRequest : public ocpp::Message {
    CiString<20> chargingLimitSource;
    std::optional<int32_t> evseId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ClearedChargingLimit message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
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
    std::string get_type() const override;
};

/// \brief Conversion from a given ClearedChargingLimitResponse \p k to a given json object \p j
void to_json(json& j, const ClearedChargingLimitResponse& k);

/// \brief Conversion from a given json object \p j to a given ClearedChargingLimitResponse \p k
void from_json(const json& j, ClearedChargingLimitResponse& k);

/// \brief Writes the string representation of the given ClearedChargingLimitResponse \p k to the given output stream \p
/// os \returns an output stream with the ClearedChargingLimitResponse written to
std::ostream& operator<<(std::ostream& os, const ClearedChargingLimitResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_CLEAREDCHARGINGLIMIT_HPP
