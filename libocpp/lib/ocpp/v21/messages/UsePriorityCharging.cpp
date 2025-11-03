// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/UsePriorityCharging.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string UsePriorityChargingRequest::get_type() const {
    return "UsePriorityCharging";
}

void to_json(json& j, const UsePriorityChargingRequest& k) {
    // the required parts of the message
    j = json{
        {"transactionId", k.transactionId},
        {"activate", k.activate},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, UsePriorityChargingRequest& k) {
    // the required parts of the message
    k.transactionId = j.at("transactionId");
    k.activate = j.at("activate");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given UsePriorityChargingRequest \p k to the given output stream \p
/// os \returns an output stream with the UsePriorityChargingRequest written to
std::ostream& operator<<(std::ostream& os, const UsePriorityChargingRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string UsePriorityChargingResponse::get_type() const {
    return "UsePriorityChargingResponse";
}

void to_json(json& j, const UsePriorityChargingResponse& k) {
    // the required parts of the message
    j = json{
        {"status", ocpp::v2::conversions::priority_charging_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, UsePriorityChargingResponse& k) {
    // the required parts of the message
    k.status = ocpp::v2::conversions::string_to_priority_charging_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given UsePriorityChargingResponse \p k to the given output stream \p
/// os \returns an output stream with the UsePriorityChargingResponse written to
std::ostream& operator<<(std::ostream& os, const UsePriorityChargingResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
