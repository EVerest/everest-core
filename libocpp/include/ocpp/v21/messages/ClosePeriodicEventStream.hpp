// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_CLOSEPERIODICEVENTSTREAM_HPP
#define OCPP_V21_CLOSEPERIODICEVENTSTREAM_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP ClosePeriodicEventStream message
struct ClosePeriodicEventStreamRequest : public ocpp::Message {
    int32_t id;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ClosePeriodicEventStream message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ClosePeriodicEventStreamRequest \p k to a given json object \p j
void to_json(json& j, const ClosePeriodicEventStreamRequest& k);

/// \brief Conversion from a given json object \p j to a given ClosePeriodicEventStreamRequest \p k
void from_json(const json& j, ClosePeriodicEventStreamRequest& k);

/// \brief Writes the string representation of the given ClosePeriodicEventStreamRequest \p k to the given output stream
/// \p os \returns an output stream with the ClosePeriodicEventStreamRequest written to
std::ostream& operator<<(std::ostream& os, const ClosePeriodicEventStreamRequest& k);

/// \brief Contains a OCPP ClosePeriodicEventStreamResponse message
struct ClosePeriodicEventStreamResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this ClosePeriodicEventStreamResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given ClosePeriodicEventStreamResponse \p k to a given json object \p j
void to_json(json& j, const ClosePeriodicEventStreamResponse& k);

/// \brief Conversion from a given json object \p j to a given ClosePeriodicEventStreamResponse \p k
void from_json(const json& j, ClosePeriodicEventStreamResponse& k);

/// \brief Writes the string representation of the given ClosePeriodicEventStreamResponse \p k to the given output
/// stream \p os \returns an output stream with the ClosePeriodicEventStreamResponse written to
std::ostream& operator<<(std::ostream& os, const ClosePeriodicEventStreamResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_CLOSEPERIODICEVENTSTREAM_HPP
