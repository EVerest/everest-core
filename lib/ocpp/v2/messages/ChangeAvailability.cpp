// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/ChangeAvailability.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string ChangeAvailabilityRequest::get_type() const {
    return "ChangeAvailability";
}

void to_json(json& j, const ChangeAvailabilityRequest& k) {
    // the required parts of the message
    j = json{
        {"operationalStatus", conversions::operational_status_enum_to_string(k.operationalStatus)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.evse) {
        j["evse"] = k.evse.value();
    }
}

void from_json(const json& j, ChangeAvailabilityRequest& k) {
    // the required parts of the message
    k.operationalStatus = conversions::string_to_operational_status_enum(j.at("operationalStatus"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("evse")) {
        k.evse.emplace(j.at("evse"));
    }
}

/// \brief Writes the string representation of the given ChangeAvailabilityRequest \p k to the given output stream \p os
/// \returns an output stream with the ChangeAvailabilityRequest written to
std::ostream& operator<<(std::ostream& os, const ChangeAvailabilityRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string ChangeAvailabilityResponse::get_type() const {
    return "ChangeAvailabilityResponse";
}

void to_json(json& j, const ChangeAvailabilityResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::change_availability_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, ChangeAvailabilityResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_change_availability_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given ChangeAvailabilityResponse \p k to the given output stream \p
/// os \returns an output stream with the ChangeAvailabilityResponse written to
std::ostream& operator<<(std::ostream& os, const ChangeAvailabilityResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
