// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/v201/messages/Reset.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string ResetRequest::get_type() const {
    return "Reset";
}

void to_json(json& j, const ResetRequest& k) {
    // the required parts of the message
    j = json{
        {"type", conversions::reset_enum_to_string(k.type)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.evseId) {
        j["evseId"] = k.evseId.value();
    }
}

void from_json(const json& j, ResetRequest& k) {
    // the required parts of the message
    k.type = conversions::string_to_reset_enum(j.at("type"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("evseId")) {
        k.evseId.emplace(j.at("evseId"));
    }
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
        {"status", conversions::reset_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, ResetResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_reset_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given ResetResponse \p k to the given output stream \p os
/// \returns an output stream with the ResetResponse written to
std::ostream& operator<<(std::ostream& os, const ResetResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
