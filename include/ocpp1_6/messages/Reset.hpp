// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_RESET_HPP
#define OCPP1_6_RESET_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct ResetRequest : public Message {
    ResetType type;

    std::string get_type() const {
        return "Reset";
    }

    friend void to_json(json& j, const ResetRequest& k) {
        // the required parts of the message
        j = json{
            {"type", conversions::reset_type_to_string(k.type)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, ResetRequest& k) {
        // the required parts of the message
        k.type = conversions::string_to_reset_type(j.at("type"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const ResetRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct ResetResponse : public Message {
    ResetStatus status;

    std::string get_type() const {
        return "ResetResponse";
    }

    friend void to_json(json& j, const ResetResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::reset_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, ResetResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_reset_status(j.at("status"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const ResetResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_RESET_HPP
