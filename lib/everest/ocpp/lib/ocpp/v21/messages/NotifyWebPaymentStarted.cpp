// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/NotifyWebPaymentStarted.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string NotifyWebPaymentStartedRequest::get_type() const {
    return "NotifyWebPaymentStarted";
}

void to_json(json& j, const NotifyWebPaymentStartedRequest& k) {
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

void from_json(const json& j, NotifyWebPaymentStartedRequest& k) {
    // the required parts of the message
    k.evseId = j.at("evseId");
    k.timeout = j.at("timeout");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyWebPaymentStartedRequest \p k to the given output stream
/// \p os
/// \returns an output stream with the NotifyWebPaymentStartedRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyWebPaymentStartedRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyWebPaymentStartedResponse::get_type() const {
    return "NotifyWebPaymentStartedResponse";
}

void to_json(json& j, const NotifyWebPaymentStartedResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyWebPaymentStartedResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyWebPaymentStartedResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the NotifyWebPaymentStartedResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyWebPaymentStartedResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
