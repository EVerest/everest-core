// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/NotifyCRL.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string NotifyCRLRequest::get_type() const {
    return "NotifyCRL";
}

void to_json(json& j, const NotifyCRLRequest& k) {
    // the required parts of the message
    j = json{
        {"requestId", k.requestId},
        {"status", ocpp::v2::conversions::notify_crlstatus_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.location) {
        j["location"] = k.location.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyCRLRequest& k) {
    // the required parts of the message
    k.requestId = j.at("requestId");
    k.status = ocpp::v2::conversions::string_to_notify_crlstatus_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("location")) {
        k.location.emplace(j.at("location"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyCRLRequest \p k to the given output stream \p os
/// \returns an output stream with the NotifyCRLRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyCRLRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyCRLResponse::get_type() const {
    return "NotifyCRLResponse";
}

void to_json(json& j, const NotifyCRLResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyCRLResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyCRLResponse \p k to the given output stream \p os
/// \returns an output stream with the NotifyCRLResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyCRLResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
