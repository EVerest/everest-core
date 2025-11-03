// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#ifndef OCPP_V21_NOTIFYPERIODICEVENTSTREAM_HPP
#define OCPP_V21_NOTIFYPERIODICEVENTSTREAM_HPP

#include <nlohmann/json_fwd.hpp>
#include <optional>

#include <ocpp/v2/ocpp_types.hpp>
using namespace ocpp::v2;
#include <ocpp/common/types.hpp>

namespace ocpp {
namespace v21 {

/// \brief Contains a OCPP NotifyPeriodicEventStream message
struct NotifyPeriodicEventStream : public ocpp::Message {
    std::vector<StreamDataElement> data;
    int32_t id;
    int32_t pending;
    ocpp::DateTime basetime;
    std::optional<CustomData> customData;

    /// \brief Provides the type of this NotifyPeriodicEventStream message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const override;
};

/// \brief Conversion from a given NotifyPeriodicEventStream \p k to a given json object \p j
void to_json(json& j, const NotifyPeriodicEventStream& k);

/// \brief Conversion from a given json object \p j to a given NotifyPeriodicEventStream \p k
void from_json(const json& j, NotifyPeriodicEventStream& k);

/// \brief Writes the string representation of the given NotifyPeriodicEventStream \p k to the given output stream \p os
/// \returns an output stream with the NotifyPeriodicEventStream written to
std::ostream& operator<<(std::ostream& os, const NotifyPeriodicEventStream& k);

} // namespace v21
} // namespace ocpp

#endif // OCPP_V21_NOTIFYPERIODICEVENTSTREAM_HPP
