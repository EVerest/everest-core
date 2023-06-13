// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/SendLocalList.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string SendLocalListRequest::get_type() const {
    return "SendLocalList";
}

void to_json(json& j, const SendLocalListRequest& k) {
    // the required parts of the message
    j = json{
        {"versionNumber", k.versionNumber},
        {"updateType", conversions::update_enum_to_string(k.updateType)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.localAuthorizationList) {
        j["localAuthorizationList"] = json::array();
        for (auto val : k.localAuthorizationList.value()) {
            j["localAuthorizationList"].push_back(val);
        }
    }
}

void from_json(const json& j, SendLocalListRequest& k) {
    // the required parts of the message
    k.versionNumber = j.at("versionNumber");
    k.updateType = conversions::string_to_update_enum(j.at("updateType"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("localAuthorizationList")) {
        json arr = j.at("localAuthorizationList");
        std::vector<AuthorizationData> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.localAuthorizationList.emplace(vec);
    }
}

/// \brief Writes the string representation of the given SendLocalListRequest \p k to the given output stream \p os
/// \returns an output stream with the SendLocalListRequest written to
std::ostream& operator<<(std::ostream& os, const SendLocalListRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SendLocalListResponse::get_type() const {
    return "SendLocalListResponse";
}

void to_json(json& j, const SendLocalListResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::send_local_list_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, SendLocalListResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_send_local_list_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given SendLocalListResponse \p k to the given output stream \p os
/// \returns an output stream with the SendLocalListResponse written to
std::ostream& operator<<(std::ostream& os, const SendLocalListResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
