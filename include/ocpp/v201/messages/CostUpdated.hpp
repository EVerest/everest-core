// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_COSTUPDATED_HPP
#define OCPP_V201_COSTUPDATED_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP CostUpdated message
struct CostUpdatedRequest : public ocpp::Message {
    float totalCost;
    CiString<36> transactionId;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this CostUpdated message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given CostUpdatedRequest \p k to a given json object \p j
void to_json(json& j, const CostUpdatedRequest& k);

/// \brief Conversion from a given json object \p j to a given CostUpdatedRequest \p k
void from_json(const json& j, CostUpdatedRequest& k);

/// \brief Writes the string representation of the given CostUpdatedRequest \p k to the given output stream \p os
/// \returns an output stream with the CostUpdatedRequest written to
std::ostream& operator<<(std::ostream& os, const CostUpdatedRequest& k);

/// \brief Contains a OCPP CostUpdatedResponse message
struct CostUpdatedResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this CostUpdatedResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given CostUpdatedResponse \p k to a given json object \p j
void to_json(json& j, const CostUpdatedResponse& k);

/// \brief Conversion from a given json object \p j to a given CostUpdatedResponse \p k
void from_json(const json& j, CostUpdatedResponse& k);

/// \brief Writes the string representation of the given CostUpdatedResponse \p k to the given output stream \p os
/// \returns an output stream with the CostUpdatedResponse written to
std::ostream& operator<<(std::ostream& os, const CostUpdatedResponse& k);

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_COSTUPDATED_HPP
