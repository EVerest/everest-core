// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/messages/Reset.hpp>
#include <ocpp1_6/ocpp_types.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

std::string ResetRequest::get_type() const {
    return "Reset";
}

void to_json(json& j, const ResetRequest& k) {
    // the required parts of the message
    j = json{
        {"type", conversions::reset_type_to_string(k.type)},
    };
    // the optional parts of the message
}

void from_json(const json& j, ResetRequest& k) {
    // the required parts of the message
    k.type = conversions::string_to_reset_type(j.at("type"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given ResetRequest \p k to the given output stream \p os
/// \returns an output stream with the ResetRequest written to
std::ostream& operator<<(std::ostream& os, const ResetRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ResetResponse::get_type() const {
    return "ResetResponse";
}

void to_json(json& j, const ResetResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::reset_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, ResetResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_reset_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given ResetResponse \p k to the given output stream \p os
/// \returns an output stream with the ResetResponse written to
std::ostream& operator<<(std::ostream& os, const ResetResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace ocpp1_6
