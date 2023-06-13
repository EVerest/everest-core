// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/UnpublishFirmware.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string UnpublishFirmwareRequest::get_type() const {
    return "UnpublishFirmware";
}

void to_json(json& j, const UnpublishFirmwareRequest& k) {
    // the required parts of the message
    j = json{
        {"checksum", k.checksum},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, UnpublishFirmwareRequest& k) {
    // the required parts of the message
    k.checksum = j.at("checksum");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given UnpublishFirmwareRequest \p k to the given output stream \p os
/// \returns an output stream with the UnpublishFirmwareRequest written to
std::ostream& operator<<(std::ostream& os, const UnpublishFirmwareRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string UnpublishFirmwareResponse::get_type() const {
    return "UnpublishFirmwareResponse";
}

void to_json(json& j, const UnpublishFirmwareResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::unpublish_firmware_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, UnpublishFirmwareResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_unpublish_firmware_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given UnpublishFirmwareResponse \p k to the given output stream \p os
/// \returns an output stream with the UnpublishFirmwareResponse written to
std::ostream& operator<<(std::ostream& os, const UnpublishFirmwareResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
