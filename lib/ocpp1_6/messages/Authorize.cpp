// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <ostream>

#include <boost/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp1_6/messages/Authorize.hpp>
#include <ocpp1_6/ocpp_types.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

std::string AuthorizeRequest::get_type() const {
    return "Authorize";
}

void to_json(json& j, const AuthorizeRequest& k) {
    // the required parts of the message
    j = json{
        {"idTag", k.idTag},
    };
    // the optional parts of the message
}

void from_json(const json& j, AuthorizeRequest& k) {
    // the required parts of the message
    k.idTag = j.at("idTag");

    // the optional parts of the message
}

/// \brief Writes the string representation of the given AuthorizeRequest \p k to the given output stream \p os
/// \returns an output stream with the AuthorizeRequest written to
std::ostream& operator<<(std::ostream& os, const AuthorizeRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string AuthorizeResponse::get_type() const {
    return "AuthorizeResponse";
}

void to_json(json& j, const AuthorizeResponse& k) {
    // the required parts of the message
    j = json{
        {"idTagInfo", k.idTagInfo},
    };
    // the optional parts of the message
}

void from_json(const json& j, AuthorizeResponse& k) {
    // the required parts of the message
    k.idTagInfo = j.at("idTagInfo");

    // the optional parts of the message
}

/// \brief Writes the string representation of the given AuthorizeResponse \p k to the given output stream \p os
/// \returns an output stream with the AuthorizeResponse written to
std::ostream& operator<<(std::ostream& os, const AuthorizeResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace ocpp1_6
