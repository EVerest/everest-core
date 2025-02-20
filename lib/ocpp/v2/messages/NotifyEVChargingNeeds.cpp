// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/NotifyEVChargingNeeds.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string NotifyEVChargingNeedsRequest::get_type() const {
    return "NotifyEVChargingNeeds";
}

void to_json(json& j, const NotifyEVChargingNeedsRequest& k) {
    // the required parts of the message
    j = json{
        {"chargingNeeds", k.chargingNeeds},
        {"evseId", k.evseId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.maxScheduleTuples) {
        j["maxScheduleTuples"] = k.maxScheduleTuples.value();
    }
}

void from_json(const json& j, NotifyEVChargingNeedsRequest& k) {
    // the required parts of the message
    k.chargingNeeds = j.at("chargingNeeds");
    k.evseId = j.at("evseId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("maxScheduleTuples")) {
        k.maxScheduleTuples.emplace(j.at("maxScheduleTuples"));
    }
}

/// \brief Writes the string representation of the given NotifyEVChargingNeedsRequest \p k to the given output stream \p
/// os \returns an output stream with the NotifyEVChargingNeedsRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyEVChargingNeedsRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyEVChargingNeedsResponse::get_type() const {
    return "NotifyEVChargingNeedsResponse";
}

void to_json(json& j, const NotifyEVChargingNeedsResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::notify_evcharging_needs_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, NotifyEVChargingNeedsResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_notify_evcharging_needs_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given NotifyEVChargingNeedsResponse \p k to the given output stream
/// \p os \returns an output stream with the NotifyEVChargingNeedsResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyEVChargingNeedsResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
