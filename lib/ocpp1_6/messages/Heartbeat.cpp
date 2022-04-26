// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>

#include <ocpp1_6/messages/Heartbeat.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

std::string HeartbeatRequest::get_type() const {
    return "Heartbeat";
}

void to_json(json& j, const HeartbeatRequest& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
}

void from_json(const json& j, HeartbeatRequest& k) {
    // the required parts of the message

    // the optional parts of the message
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
}

void from_json(const json& j, HeartbeatResponse& k) {
    // the required parts of the message
    k.currentTime = DateTime(std::string(j.at("currentTime")));
    ;

    // the optional parts of the message
}

/// \brief Writes the string representation of the given HeartbeatResponse \p k to the given output stream \p os
/// \returns an output stream with the HeartbeatResponse written to
std::ostream& operator<<(std::ostream& os, const HeartbeatResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace ocpp1_6
