// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V201_METERVALUES_HPP
#define OCPP_V201_METERVALUES_HPP

#include <boost/optional.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/v201/enums.hpp>
#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Contains a OCPP MeterValues message
struct MeterValuesRequest : public ocpp::Message {
    int32_t evseId;
    std::vector<MeterValue> meterValue;
    boost::optional<CustomData> customData;

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
    boost::optional<CustomData> customData;

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

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_METERVALUES_HPP
