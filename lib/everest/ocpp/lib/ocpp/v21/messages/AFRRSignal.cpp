// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/AFRRSignal.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string AFRRSignalRequest::get_type() const {
    return "AFRRSignal";
}

void to_json(json& j, const AFRRSignalRequest& k) {
    // the required parts of the message
    j = json{
        {"timestamp", k.timestamp.to_rfc3339()},
        {"signal", k.signal},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, AFRRSignalRequest& k) {
    // the required parts of the message
    k.timestamp = ocpp::DateTime(std::string(j.at("timestamp")));
    k.signal = j.at("signal");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given AFRRSignalRequest \p k to the given output stream \p os
/// \returns an output stream with the AFRRSignalRequest written to
std::ostream& operator<<(std::ostream& os, const AFRRSignalRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string AFRRSignalResponse::get_type() const {
    return "AFRRSignalResponse";
}

void to_json(json& j, const AFRRSignalResponse& k) {
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

void from_json(const json& j, AFRRSignalResponse& k) {
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

/// \brief Writes the string representation of the given AFRRSignalResponse \p k to the given output stream \p os
/// \returns an output stream with the AFRRSignalResponse written to
std::ostream& operator<<(std::ostream& os, const AFRRSignalResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
