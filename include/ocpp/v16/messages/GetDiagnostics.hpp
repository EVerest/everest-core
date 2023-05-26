// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_V16_GETDIAGNOSTICS_HPP
#define OCPP_V16_GETDIAGNOSTICS_HPP

#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_types.hpp>

namespace ocpp {
namespace v16 {

/// \brief Contains a OCPP GetDiagnostics message
struct GetDiagnosticsRequest : public ocpp::Message {
    std::string location;
    std::optional<int32_t> retries;
    std::optional<int32_t> retryInterval;
    std::optional<ocpp::DateTime> startTime;
    std::optional<ocpp::DateTime> stopTime;

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

/// \brief Contains a OCPP GetDiagnosticsResponse message
struct GetDiagnosticsResponse : public ocpp::Message {
    std::optional<CiString<255>> fileName;

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

} // namespace v16
} // namespace ocpp

#endif // OCPP_V16_GETDIAGNOSTICS_HPP
