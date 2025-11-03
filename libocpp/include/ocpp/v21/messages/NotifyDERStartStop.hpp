// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_NOTIFYDERSTARTSTOP_HPP
#define OCPP_V21_NOTIFYDERSTARTSTOP_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP NotifyDERStartStop message
struct NotifyDERStartStopRequest : public ocpp::Message {
    CiString<36> controlId;
    bool started;
    ocpp::DateTime timestamp;
    std::optional<std::vector<CiString<36>>> supersededIds;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyDERStartStop message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyDERStartStopRequest \p k to a given json object \p j
void to_json(json& j, const NotifyDERStartStopRequest& k);

/// \brief Conversion from a given json object \p j to a given NotifyDERStartStopRequest \p k
void from_json(const json& j, NotifyDERStartStopRequest& k);

/// \brief Writes the string representation of the given NotifyDERStartStopRequest \p k to the given output stream \p os
/// \returns an output stream with the NotifyDERStartStopRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyDERStartStopRequest& k);

/// \brief Contains a OCPP NotifyDERStartStopResponse message
struct NotifyDERStartStopResponse : public ocpp::Message {
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyDERStartStopResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyDERStartStopResponse \p k to a given json object \p j
void to_json(json& j, const NotifyDERStartStopResponse& k);

/// \brief Conversion from a given json object \p j to a given NotifyDERStartStopResponse \p k
void from_json(const json& j, NotifyDERStartStopResponse& k);

/// \brief Writes the string representation of the given NotifyDERStartStopResponse \p k to the given output stream \p
/// os \returns an output stream with the NotifyDERStartStopResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyDERStartStopResponse& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_NOTIFYDERSTARTSTOP_HPP
