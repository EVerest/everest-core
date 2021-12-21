// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_AUTHORIZE_HPP
#define OCPP1_6_AUTHORIZE_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct AuthorizeRequest : public Message {
    CiString20Type idTag;

    std::string get_type() const {
        return "Authorize";
    }

    friend void to_json(json& j, const AuthorizeRequest& k) {
        // the required parts of the message
        j = json{
            {"idTag", k.idTag},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, AuthorizeRequest& k) {
        // the required parts of the message
        k.idTag = j.at("idTag");

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const AuthorizeRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct AuthorizeResponse : public Message {
    IdTagInfo idTagInfo;

    std::string get_type() const {
        return "AuthorizeResponse";
    }

    friend void to_json(json& j, const AuthorizeResponse& k) {
        // the required parts of the message
        j = json{
            {"idTagInfo", k.idTagInfo},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, AuthorizeResponse& k) {
        // the required parts of the message
        k.idTagInfo = j.at("idTagInfo");

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const AuthorizeResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_AUTHORIZE_HPP
