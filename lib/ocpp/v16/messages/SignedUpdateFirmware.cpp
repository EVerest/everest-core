// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <boost/optional/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp/v16/messages/SignedUpdateFirmware.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string SignedUpdateFirmwareRequest::get_type() const {
    return "SignedUpdateFirmware";
}

void to_json(json& j, const SignedUpdateFirmwareRequest& k) {
    // the required parts of the message
    j = json{
        {"requestId", k.requestId},
        {"firmware", k.firmware},
    };
    // the optional parts of the message
    if (k.retries) {
        j["retries"] = k.retries.value();
    }
    if (k.retryInterval) {
        j["retryInterval"] = k.retryInterval.value();
    }
}

void from_json(const json& j, SignedUpdateFirmwareRequest& k) {
    // the required parts of the message
    k.requestId = j.at("requestId");
    k.firmware = j.at("firmware");

    // the optional parts of the message
    if (j.contains("retries")) {
        k.retries.emplace(j.at("retries"));
    }
    if (j.contains("retryInterval")) {
        k.retryInterval.emplace(j.at("retryInterval"));
    }
}

/// \brief Writes the string representation of the given SignedUpdateFirmwareRequest \p k to the given output stream \p
/// os \returns an output stream with the SignedUpdateFirmwareRequest written to
std::ostream& operator<<(std::ostream& os, const SignedUpdateFirmwareRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SignedUpdateFirmwareResponse::get_type() const {
    return "SignedUpdateFirmwareResponse";
}

void to_json(json& j, const SignedUpdateFirmwareResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::update_firmware_status_enum_type_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, SignedUpdateFirmwareResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_update_firmware_status_enum_type(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given SignedUpdateFirmwareResponse \p k to the given output stream \p
/// os \returns an output stream with the SignedUpdateFirmwareResponse written to
std::ostream& operator<<(std::ostream& os, const SignedUpdateFirmwareResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
