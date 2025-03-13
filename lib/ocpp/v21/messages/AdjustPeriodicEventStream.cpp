// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/AdjustPeriodicEventStream.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string AdjustPeriodicEventStreamRequest::get_type() const {
    return "AdjustPeriodicEventStream";
}

void to_json(json& j, const AdjustPeriodicEventStreamRequest& k) {
    // the required parts of the message
    j = json{
        {"id", k.id},
        {"params", k.params},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, AdjustPeriodicEventStreamRequest& k) {
    // the required parts of the message
    k.id = j.at("id");
    k.params = j.at("params");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given AdjustPeriodicEventStreamRequest \p k to the given output
/// stream \p os \returns an output stream with the AdjustPeriodicEventStreamRequest written to
std::ostream& operator<<(std::ostream& os, const AdjustPeriodicEventStreamRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string AdjustPeriodicEventStreamResponse::get_type() const {
    return "AdjustPeriodicEventStreamResponse";
}

void to_json(json& j, const AdjustPeriodicEventStreamResponse& k) {
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

void from_json(const json& j, AdjustPeriodicEventStreamResponse& k) {
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

/// \brief Writes the string representation of the given AdjustPeriodicEventStreamResponse \p k to the given output
/// stream \p os \returns an output stream with the AdjustPeriodicEventStreamResponse written to
std::ostream& operator<<(std::ostream& os, const AdjustPeriodicEventStreamResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
