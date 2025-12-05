// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_ADJUSTPERIODICEVENTSTREAM_HPP
#define OCPP_V21_ADJUSTPERIODICEVENTSTREAM_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP AdjustPeriodicEventStream message
struct AdjustPeriodicEventStreamRequest : public ocpp::Message {
    int32_t id;
    PeriodicEventStreamParams params;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this AdjustPeriodicEventStream message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given AdjustPeriodicEventStreamRequest \p k to a given json object \p j
void to_json(json& j, const AdjustPeriodicEventStreamRequest& k);

/// \brief Conversion from a given json object \p j to a given AdjustPeriodicEventStreamRequest \p k
void from_json(const json& j, AdjustPeriodicEventStreamRequest& k);

/// \brief Writes the string representation of the given AdjustPeriodicEventStreamRequest \p k to the given output
/// stream \p os \returns an output stream with the AdjustPeriodicEventStreamRequest written to
std::ostream& operator<<(std::ostream& os, const AdjustPeriodicEventStreamRequest& k);

/// \brief Contains a OCPP AdjustPeriodicEventStreamResponse message
struct AdjustPeriodicEventStreamResponse : public ocpp::Message {
    GenericStatusEnum status;
    std::optional<StatusInfo> statusInfo;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this AdjustPeriodicEventStreamResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given AdjustPeriodicEventStreamResponse \p k to a given json object \p j
void to_json(json& j, const AdjustPeriodicEventStreamResponse& k);

/// \brief Conversion from a given json object \p j to a given AdjustPeriodicEventStreamResponse \p k
void from_json(const json& j, AdjustPeriodicEventStreamResponse& k);

/// \brief Writes the string representation of the given AdjustPeriodicEventStreamResponse \p k to the given output
/// stream \p os \returns an output stream with the AdjustPeriodicEventStreamResponse written to
std::ostream& operator<<(std::ostream& os, const AdjustPeriodicEventStreamResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_ADJUSTPERIODICEVENTSTREAM_HPP
