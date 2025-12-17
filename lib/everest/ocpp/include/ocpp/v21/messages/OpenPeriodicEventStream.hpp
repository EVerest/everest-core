// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_OPENPERIODICEVENTSTREAM_HPP
#define OCPP_V21_OPENPERIODICEVENTSTREAM_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP OpenPeriodicEventStream message
struct OpenPeriodicEventStreamRequest : public ocpp::Message {
    ConstantStreamData constantStreamData;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this OpenPeriodicEventStream message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given OpenPeriodicEventStreamRequest \p k to a given json object \p j
void to_json(json& j, const OpenPeriodicEventStreamRequest& k);

/// \brief Conversion from a given json object \p j to a given OpenPeriodicEventStreamRequest \p k
void from_json(const json& j, OpenPeriodicEventStreamRequest& k);

/// \brief Writes the string representation of the given OpenPeriodicEventStreamRequest \p k to the given output stream
/// \p os
/// \returns an output stream with the OpenPeriodicEventStreamRequest written to
std::ostream& operator<<(std::ostream& os, const OpenPeriodicEventStreamRequest& k);

/// \brief Contains a OCPP OpenPeriodicEventStreamResponse message
struct OpenPeriodicEventStreamResponse : public ocpp::Message {
    GenericStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this OpenPeriodicEventStreamResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given OpenPeriodicEventStreamResponse \p k to a given json object \p j
void to_json(json& j, const OpenPeriodicEventStreamResponse& k);

/// \brief Conversion from a given json object \p j to a given OpenPeriodicEventStreamResponse \p k
void from_json(const json& j, OpenPeriodicEventStreamResponse& k);

/// \brief Writes the string representation of the given OpenPeriodicEventStreamResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the OpenPeriodicEventStreamResponse written to
std::ostream& operator<<(std::ostream& os, const OpenPeriodicEventStreamResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_OPENPERIODICEVENTSTREAM_HPP
