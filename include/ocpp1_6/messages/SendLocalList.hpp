// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_SENDLOCALLIST_HPP
#define OCPP1_6_SENDLOCALLIST_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct SendLocalListRequest : public Message {
    int32_t listVersion;
    UpdateType updateType;
    boost::optional<std::vector<LocalAuthorizationList>> localAuthorizationList;

    std::string get_type() const {
        return "SendLocalList";
    }

    friend void to_json(json& j, const SendLocalListRequest& k) {
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

    friend void from_json(const json& j, SendLocalListRequest& k) {
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

    friend std::ostream& operator<<(std::ostream& os, const SendLocalListRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct SendLocalListResponse : public Message {
    UpdateStatus status;

    std::string get_type() const {
        return "SendLocalListResponse";
    }

    friend void to_json(json& j, const SendLocalListResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::update_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, SendLocalListResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_update_status(j.at("status"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const SendLocalListResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_SENDLOCALLIST_HPP
