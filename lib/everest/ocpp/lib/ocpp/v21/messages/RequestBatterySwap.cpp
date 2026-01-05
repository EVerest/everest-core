// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/RequestBatterySwap.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string RequestBatterySwapRequest::get_type() const {
    return "RequestBatterySwap";
}

void to_json(json& j, const RequestBatterySwapRequest& k) {
    // the required parts of the message
    j = json{
        {"idToken", k.idToken},
        {"requestId", k.requestId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, RequestBatterySwapRequest& k) {
    // the required parts of the message
    k.idToken = j.at("idToken");
    k.requestId = j.at("requestId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given RequestBatterySwapRequest \p k to the given output stream \p os
/// \returns an output stream with the RequestBatterySwapRequest written to
std::ostream& operator<<(std::ostream& os, const RequestBatterySwapRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string RequestBatterySwapResponse::get_type() const {
    return "RequestBatterySwapResponse";
}

void to_json(json& j, const RequestBatterySwapResponse& k) {
    // the required parts of the message
    j = json{
        {"status", ocpp::v2::conversions::generic_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, RequestBatterySwapResponse& k) {
    // the required parts of the message
    k.status = ocpp::v2::conversions::string_to_generic_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given RequestBatterySwapResponse \p k to the given output stream \p
/// os
/// \returns an output stream with the RequestBatterySwapResponse written to
std::ostream& operator<<(std::ostream& os, const RequestBatterySwapResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
