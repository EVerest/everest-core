// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/PublishFirmware.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string PublishFirmwareRequest::get_type() const {
    return "PublishFirmware";
}

void to_json(json& j, const PublishFirmwareRequest& k) {
    // the required parts of the message
    j = json{
        {"location", k.location},
        {"checksum", k.checksum},
        {"requestId", k.requestId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.retries) {
        j["retries"] = k.retries.value();
    }
    if (k.retryInterval) {
        j["retryInterval"] = k.retryInterval.value();
    }
}

void from_json(const json& j, PublishFirmwareRequest& k) {
    // the required parts of the message
    k.location = j.at("location");
    k.checksum = j.at("checksum");
    k.requestId = j.at("requestId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("retries")) {
        k.retries.emplace(j.at("retries"));
    }
    if (j.contains("retryInterval")) {
        k.retryInterval.emplace(j.at("retryInterval"));
    }
}

/// \brief Writes the string representation of the given PublishFirmwareRequest \p k to the given output stream \p os
/// \returns an output stream with the PublishFirmwareRequest written to
std::ostream& operator<<(std::ostream& os, const PublishFirmwareRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string PublishFirmwareResponse::get_type() const {
    return "PublishFirmwareResponse";
}

void to_json(json& j, const PublishFirmwareResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::generic_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, PublishFirmwareResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_generic_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given PublishFirmwareResponse \p k to the given output stream \p os
/// \returns an output stream with the PublishFirmwareResponse written to
std::ostream& operator<<(std::ostream& os, const PublishFirmwareResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
