// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V2_GETLOG_HPP
#define OCPP_V2_GETLOG_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/common/types.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Contains a OCPP GetLog message
struct GetLogRequest : public ocpp::Message {
    LogParameters log;
    LogEnum logType;
    int32_t requestId;
    std::optional<int32_t> retries;
    std::optional<int32_t> retryInterval;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetLog message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetLogRequest \p k to a given json object \p j
void to_json(json& j, const GetLogRequest& k);

/// \brief Conversion from a given json object \p j to a given GetLogRequest \p k
void from_json(const json& j, GetLogRequest& k);

/// \brief Writes the string representation of the given GetLogRequest \p k to the given output stream \p os
/// \returns an output stream with the GetLogRequest written to
std::ostream& operator<<(std::ostream& os, const GetLogRequest& k);

/// \brief Contains a OCPP GetLogResponse message
struct GetLogResponse : public ocpp::Message {
    LogStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CiString<255>> filename;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetLogResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetLogResponse \p k to a given json object \p j
void to_json(json& j, const GetLogResponse& k);

/// \brief Conversion from a given json object \p j to a given GetLogResponse \p k
void from_json(const json& j, GetLogResponse& k);

/// \brief Writes the string representation of the given GetLogResponse \p k to the given output stream \p os
/// \returns an output stream with the GetLogResponse written to
std::ostream& operator<<(std::ostream& os, const GetLogResponse& k);

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_GETLOG_HPP
