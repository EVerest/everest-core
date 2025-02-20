// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/SetNetworkProfile.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string SetNetworkProfileRequest::get_type() const {
    return "SetNetworkProfile";
}

void to_json(json& j, const SetNetworkProfileRequest& k) {
    // the required parts of the message
    j = json{
        {"configurationSlot", k.configurationSlot},
        {"connectionData", k.connectionData},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, SetNetworkProfileRequest& k) {
    // the required parts of the message
    k.configurationSlot = j.at("configurationSlot");
    k.connectionData = j.at("connectionData");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given SetNetworkProfileRequest \p k to the given output stream \p os
/// \returns an output stream with the SetNetworkProfileRequest written to
std::ostream& operator<<(std::ostream& os, const SetNetworkProfileRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SetNetworkProfileResponse::get_type() const {
    return "SetNetworkProfileResponse";
}

void to_json(json& j, const SetNetworkProfileResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::set_network_profile_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, SetNetworkProfileResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_set_network_profile_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given SetNetworkProfileResponse \p k to the given output stream \p os
/// \returns an output stream with the SetNetworkProfileResponse written to
std::ostream& operator<<(std::ostream& os, const SetNetworkProfileResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
