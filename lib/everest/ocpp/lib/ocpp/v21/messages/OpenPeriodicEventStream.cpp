// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/OpenPeriodicEventStream.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string OpenPeriodicEventStreamRequest::get_type() const {
    return "OpenPeriodicEventStream";
}

void to_json(json& j, const OpenPeriodicEventStreamRequest& k) {
    // the required parts of the message
    j = json{
        {"constantStreamData", k.constantStreamData},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, OpenPeriodicEventStreamRequest& k) {
    // the required parts of the message
    k.constantStreamData = j.at("constantStreamData");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given OpenPeriodicEventStreamRequest \p k to the given output stream
/// \p os \returns an output stream with the OpenPeriodicEventStreamRequest written to
std::ostream& operator<<(std::ostream& os, const OpenPeriodicEventStreamRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string OpenPeriodicEventStreamResponse::get_type() const {
    return "OpenPeriodicEventStreamResponse";
}

void to_json(json& j, const OpenPeriodicEventStreamResponse& k) {
    // the required parts of the message
    j = json{
        {"status", ocpp::v2::conversions::generic_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, OpenPeriodicEventStreamResponse& k) {
    // the required parts of the message
    k.status = ocpp::v2::conversions::string_to_generic_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given OpenPeriodicEventStreamResponse \p k to the given output stream
/// \p os \returns an output stream with the OpenPeriodicEventStreamResponse written to
std::ostream& operator<<(std::ostream& os, const OpenPeriodicEventStreamResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
