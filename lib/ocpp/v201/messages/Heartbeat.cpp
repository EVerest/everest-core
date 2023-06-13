// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/Heartbeat.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string HeartbeatRequest::get_type() const {
    return "Heartbeat";
}

void to_json(json& j, const HeartbeatRequest& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, HeartbeatRequest& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given HeartbeatRequest \p k to the given output stream \p os
/// \returns an output stream with the HeartbeatRequest written to
std::ostream& operator<<(std::ostream& os, const HeartbeatRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string HeartbeatResponse::get_type() const {
    return "HeartbeatResponse";
}

void to_json(json& j, const HeartbeatResponse& k) {
    // the required parts of the message
    j = json{
        {"currentTime", k.currentTime.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, HeartbeatResponse& k) {
    // the required parts of the message
    k.currentTime = ocpp::DateTime(std::string(j.at("currentTime")));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given HeartbeatResponse \p k to the given output stream \p os
/// \returns an output stream with the HeartbeatResponse written to
std::ostream& operator<<(std::ostream& os, const HeartbeatResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
