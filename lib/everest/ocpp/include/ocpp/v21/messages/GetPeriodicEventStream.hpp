// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_GETPERIODICEVENTSTREAM_HPP
#define OCPP_V21_GETPERIODICEVENTSTREAM_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP GetPeriodicEventStream message
struct GetPeriodicEventStreamRequest : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetPeriodicEventStream message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetPeriodicEventStreamRequest \p k to a given json object \p j
void to_json(json& j, const GetPeriodicEventStreamRequest& k);

/// \brief Conversion from a given json object \p j to a given GetPeriodicEventStreamRequest \p k
void from_json(const json& j, GetPeriodicEventStreamRequest& k);

/// \brief Writes the string representation of the given GetPeriodicEventStreamRequest \p k to the given output stream
/// \p os
/// \returns an output stream with the GetPeriodicEventStreamRequest written to
std::ostream& operator<<(std::ostream& os, const GetPeriodicEventStreamRequest& k);

/// \brief Contains a OCPP GetPeriodicEventStreamResponse message
struct GetPeriodicEventStreamResponse : public ocpp::Message {
    std::optional<std::vector<ConstantStreamData>> constantStreamData;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this GetPeriodicEventStreamResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given GetPeriodicEventStreamResponse \p k to a given json object \p j
void to_json(json& j, const GetPeriodicEventStreamResponse& k);

/// \brief Conversion from a given json object \p j to a given GetPeriodicEventStreamResponse \p k
void from_json(const json& j, GetPeriodicEventStreamResponse& k);

/// \brief Writes the string representation of the given GetPeriodicEventStreamResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the GetPeriodicEventStreamResponse written to
std::ostream& operator<<(std::ostream& os, const GetPeriodicEventStreamResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_GETPERIODICEVENTSTREAM_HPP
