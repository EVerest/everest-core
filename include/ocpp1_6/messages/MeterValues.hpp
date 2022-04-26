// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_METERVALUES_HPP
#define OCPP1_6_METERVALUES_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 MeterValues message
struct MeterValuesRequest : public Message {
    int32_t connectorId;
    std::vector<MeterValue> meterValue;
    boost::optional<int32_t> transactionId;

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

/// \brief Contains a OCPP 1.6 MeterValuesResponse message
struct MeterValuesResponse : public Message {

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

} // namespace ocpp1_6

#endif // OCPP1_6_METERVALUES_HPP
