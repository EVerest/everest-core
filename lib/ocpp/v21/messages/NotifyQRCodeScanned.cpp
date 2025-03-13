// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/NotifyQRCodeScanned.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string NotifyQRCodeScannedRequest::get_type() const {
    return "NotifyQRCodeScanned";
}

void to_json(json& j, const NotifyQRCodeScannedRequest& k) {
    // the required parts of the message
    j = json{
        {"evseId", k.evseId},
        {"timeout", k.timeout},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyQRCodeScannedRequest& k) {
    // the required parts of the message
    k.evseId = j.at("evseId");
    k.timeout = j.at("timeout");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyQRCodeScannedRequest \p k to the given output stream \p
/// os \returns an output stream with the NotifyQRCodeScannedRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyQRCodeScannedRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyQRCodeScannedResponse::get_type() const {
    return "NotifyQRCodeScannedResponse";
}

void to_json(json& j, const NotifyQRCodeScannedResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyQRCodeScannedResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyQRCodeScannedResponse \p k to the given output stream \p
/// os \returns an output stream with the NotifyQRCodeScannedResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyQRCodeScannedResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
