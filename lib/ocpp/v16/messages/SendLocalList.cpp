// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/v16/messages/SendLocalList.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string SendLocalListRequest::get_type() const {
    return "SendLocalList";
}

void to_json(json& j, const SendLocalListRequest& k) {
    // the required parts of the message
    j = json{
        {"listVersion", k.listVersion},
        {"updateType", conversions::update_type_to_string(k.updateType)},
    };
    // the optional parts of the message
    if (k.localAuthorizationList) {
        j["localAuthorizationList"] = json::array();
        for (auto val : k.localAuthorizationList.value()) {
            j["localAuthorizationList"].push_back(val);
        }
    }
}

void from_json(const json& j, SendLocalListRequest& k) {
    // the required parts of the message
    k.listVersion = j.at("listVersion");
    k.updateType = conversions::string_to_update_type(j.at("updateType"));

    // the optional parts of the message
    if (j.contains("localAuthorizationList")) {
        json arr = j.at("localAuthorizationList");
        std::vector<LocalAuthorizationList> vec;
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
        {"status", conversions::update_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, SendLocalListResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_update_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given SendLocalListResponse \p k to the given output stream \p os
/// \returns an output stream with the SendLocalListResponse written to
std::ostream& operator<<(std::ostream& os, const SendLocalListResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
