// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_GETDIAGNOSTICS_HPP
#define OCPP1_6_GETDIAGNOSTICS_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 GetDiagnostics message
struct GetDiagnosticsRequest : public Message {
    std::string location;
    boost::optional<int32_t> retries;
    boost::optional<int32_t> retryInterval;
    boost::optional<DateTime> startTime;
    boost::optional<DateTime> stopTime;

    /// \brief Provides the type of this GetDiagnostics message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetDiagnosticsRequest \p k to a given json object \p j
void to_json(json& j, const GetDiagnosticsRequest& k);

/// \brief Conversion from a given json object \p j to a given GetDiagnosticsRequest \p k
void from_json(const json& j, GetDiagnosticsRequest& k);

/// \brief Writes the string representation of the given GetDiagnosticsRequest \p k to the given output stream \p os
/// \returns an output stream with the GetDiagnosticsRequest written to
std::ostream& operator<<(std::ostream& os, const GetDiagnosticsRequest& k);

/// \brief Contains a OCPP 1.6 GetDiagnosticsResponse message
struct GetDiagnosticsResponse : public Message {
    boost::optional<CiString255Type> fileName;

    /// \brief Provides the type of this GetDiagnosticsResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetDiagnosticsResponse \p k to a given json object \p j
void to_json(json& j, const GetDiagnosticsResponse& k);

/// \brief Conversion from a given json object \p j to a given GetDiagnosticsResponse \p k
void from_json(const json& j, GetDiagnosticsResponse& k);

/// \brief Writes the string representation of the given GetDiagnosticsResponse \p k to the given output stream \p os
/// \returns an output stream with the GetDiagnosticsResponse written to
std::ostream& operator<<(std::ostream& os, const GetDiagnosticsResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_GETDIAGNOSTICS_HPP
