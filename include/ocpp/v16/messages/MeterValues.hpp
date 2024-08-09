// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V16_METERVALUES_HPP
#define OCPP_V16_METERVALUES_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP MeterValues message
struct MeterValuesRequest : public ocpp::Message {
    int32_t connectorId;
    std::vector<MeterValue> meterValue;
    std::optional<int32_t> transactionId;

    /// \brief Provides the type of this MeterValues message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given MeterValuesRequest \p k to a given json object \p j
void to_json(json& j, const MeterValuesRequest& k);

/// \brief Conversion from a given json object \p j to a given MeterValuesRequest \p k
void from_json(const json& j, MeterValuesRequest& k);

/// \brief Writes the string representation of the given MeterValuesRequest \p k to the given output stream \p os
/// \returns an output stream with the MeterValuesRequest written to
std::ostream& operator<<(std::ostream& os, const MeterValuesRequest& k);

/// \brief Contains a OCPP MeterValuesResponse message
struct MeterValuesResponse : public ocpp::Message {

    /// \brief Provides the type of this MeterValuesResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given MeterValuesResponse \p k to a given json object \p j
void to_json(json& j, const MeterValuesResponse& k);

/// \brief Conversion from a given json object \p j to a given MeterValuesResponse \p k
void from_json(const json& j, MeterValuesResponse& k);

/// \brief Writes the string representation of the given MeterValuesResponse \p k to the given output stream \p os
/// \returns an output stream with the MeterValuesResponse written to
std::ostream& operator<<(std::ostream& os, const MeterValuesResponse& k);

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_METERVALUES_HPP
