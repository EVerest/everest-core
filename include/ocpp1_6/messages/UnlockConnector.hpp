// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_UNLOCKCONNECTOR_HPP
#define OCPP1_6_UNLOCKCONNECTOR_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct UnlockConnectorRequest : public Message {
    int32_t connectorId;

    std::string get_type() const {
        return "UnlockConnector";
    }

    friend void to_json(json& j, const UnlockConnectorRequest& k) {
        // the required parts of the message
        j = json{
            {"connectorId", k.connectorId},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, UnlockConnectorRequest& k) {
        // the required parts of the message
        k.connectorId = j.at("connectorId");

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const UnlockConnectorRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct UnlockConnectorResponse : public Message {
    UnlockStatus status;

    std::string get_type() const {
        return "UnlockConnectorResponse";
    }

    friend void to_json(json& j, const UnlockConnectorResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::unlock_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, UnlockConnectorResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_unlock_status(j.at("status"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const UnlockConnectorResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_UNLOCKCONNECTOR_HPP
