// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_REMOTESTOPTRANSACTION_HPP
#define OCPP1_6_REMOTESTOPTRANSACTION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct RemoteStopTransactionRequest : public Message {
    int32_t transactionId;

    std::string get_type() const {
        return "RemoteStopTransaction";
    }

    friend void to_json(json& j, const RemoteStopTransactionRequest& k) {
        // the required parts of the message
        j = json{
            {"transactionId", k.transactionId},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, RemoteStopTransactionRequest& k) {
        // the required parts of the message
        k.transactionId = j.at("transactionId");

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const RemoteStopTransactionRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct RemoteStopTransactionResponse : public Message {
    RemoteStartStopStatus status;

    std::string get_type() const {
        return "RemoteStopTransactionResponse";
    }

    friend void to_json(json& j, const RemoteStopTransactionResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::remote_start_stop_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, RemoteStopTransactionResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_remote_start_stop_status(j.at("status"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const RemoteStopTransactionResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_REMOTESTOPTRANSACTION_HPP
