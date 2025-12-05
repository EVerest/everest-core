// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/ClosePeriodicEventStream.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string ClosePeriodicEventStreamRequest::get_type() const {
    return "ClosePeriodicEventStream";
}

void to_json(json& j, const ClosePeriodicEventStreamRequest& k) {
    // the required parts of the message
    j = json{
        {"id", k.id},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ClosePeriodicEventStreamRequest& k) {
    // the required parts of the message
    k.id = j.at("id");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ClosePeriodicEventStreamRequest \p k to the given output stream
/// \p os \returns an output stream with the ClosePeriodicEventStreamRequest written to
std::ostream& operator<<(std::ostream& os, const ClosePeriodicEventStreamRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ClosePeriodicEventStreamResponse::get_type() const {
    return "ClosePeriodicEventStreamResponse";
}

void to_json(json& j, const ClosePeriodicEventStreamResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, ClosePeriodicEventStreamResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given ClosePeriodicEventStreamResponse \p k to the given output
/// stream \p os \returns an output stream with the ClosePeriodicEventStreamResponse written to
std::ostream& operator<<(std::ostream& os, const ClosePeriodicEventStreamResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
