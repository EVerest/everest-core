// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/NotifyPriorityCharging.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string NotifyPriorityChargingRequest::get_type() const {
    return "NotifyPriorityCharging";
}

void to_json(json& j, const NotifyPriorityChargingRequest& k) {
    // the required parts of the message
    j = json{
        {"transactionId", k.transactionId},
        {"activated", k.activated},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyPriorityChargingRequest& k) {
    // the required parts of the message
    k.transactionId = j.at("transactionId");
    k.activated = j.at("activated");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyPriorityChargingRequest \p k to the given output stream
/// \p os \returns an output stream with the NotifyPriorityChargingRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyPriorityChargingRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyPriorityChargingResponse::get_type() const {
    return "NotifyPriorityChargingResponse";
}

void to_json(json& j, const NotifyPriorityChargingResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyPriorityChargingResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyPriorityChargingResponse \p k to the given output stream
/// \p os \returns an output stream with the NotifyPriorityChargingResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyPriorityChargingResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
