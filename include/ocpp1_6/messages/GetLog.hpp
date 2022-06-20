// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_GETLOG_HPP
#define OCPP1_6_GETLOG_HPP

#include <boost/optional.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 GetLog message
struct GetLogRequest : public Message {
    LogParametersType log;
    LogEnumType logType;
    int32_t requestId;
    boost::optional<int32_t> retries;
    boost::optional<int32_t> retryInterval;

    /// \brief Provides the type of this GetLog message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetLogRequest \p k to a given json object \p j
void to_json(json& j, const GetLogRequest& k);

/// \brief Conversion from a given json object \p j to a given GetLogRequest \p k
void from_json(const json& j, GetLogRequest& k);

/// \brief Writes the string representation of the given GetLogRequest \p k to the given output stream \p os
/// \returns an output stream with the GetLogRequest written to
std::ostream& operator<<(std::ostream& os, const GetLogRequest& k);

/// \brief Contains a OCPP 1.6 GetLogResponse message
struct GetLogResponse : public Message {
    LogStatusEnumType status;
    boost::optional<CiString255Type> filename;

    /// \brief Provides the type of this GetLogResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const;
};

/// \brief Conversion from a given GetLogResponse \p k to a given json object \p j
void to_json(json& j, const GetLogResponse& k);

/// \brief Conversion from a given json object \p j to a given GetLogResponse \p k
void from_json(const json& j, GetLogResponse& k);

/// \brief Writes the string representation of the given GetLogResponse \p k to the given output stream \p os
/// \returns an output stream with the GetLogResponse written to
std::ostream& operator<<(std::ostream& os, const GetLogResponse& k);

} // namespace ocpp1_6

#endif // OCPP1_6_GETLOG_HPP
