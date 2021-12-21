// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_HEARTBEAT_HPP
#define OCPP1_6_HEARTBEAT_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct HeartbeatRequest : public Message {

    std::string get_type() const {
        return "Heartbeat";
    }

    friend void to_json(json& j, const HeartbeatRequest& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
    }

    friend void from_json(const json& j, HeartbeatRequest& k) {
        // the required parts of the message

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const HeartbeatRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct HeartbeatResponse : public Message {
    DateTime currentTime;

    std::string get_type() const {
        return "HeartbeatResponse";
    }

    friend void to_json(json& j, const HeartbeatResponse& k) {
        // the required parts of the message
        j = json{
            {"currentTime", k.currentTime.to_rfc3339()},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, HeartbeatResponse& k) {
        // the required parts of the message
        k.currentTime = DateTime(std::string(j.at("currentTime")));
        ;

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const HeartbeatResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_HEARTBEAT_HPP
