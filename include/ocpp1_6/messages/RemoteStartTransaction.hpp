// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_REMOTESTARTTRANSACTION_HPP
#define OCPP1_6_REMOTESTARTTRANSACTION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {
struct RemoteStartTransactionRequest : public Message {
    CiString20Type idTag;
    boost::optional<int32_t> connectorId;
    boost::optional<ChargingProfile> chargingProfile;

    std::string get_type() const {
        return "RemoteStartTransaction";
    }

    friend void to_json(json& j, const RemoteStartTransactionRequest& k) {
        // the required parts of the message
        j = json{
            {"idTag", k.idTag},
        };
        // the optional parts of the message
        if (k.connectorId) {
            j["connectorId"] = k.connectorId.value();
        }
        if (k.chargingProfile) {
            j["chargingProfile"] = k.chargingProfile.value();
        }
    }

    friend void from_json(const json& j, RemoteStartTransactionRequest& k) {
        // the required parts of the message
        k.idTag = j.at("idTag");

        // the optional parts of the message
        if (j.contains("connectorId")) {
            k.connectorId.emplace(j.at("connectorId"));
        }
        if (j.contains("chargingProfile")) {
            k.chargingProfile.emplace(j.at("chargingProfile"));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const RemoteStartTransactionRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

struct RemoteStartTransactionResponse : public Message {
    RemoteStartStopStatus status;

    std::string get_type() const {
        return "RemoteStartTransactionResponse";
    }

    friend void to_json(json& j, const RemoteStartTransactionResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::remote_start_stop_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    friend void from_json(const json& j, RemoteStartTransactionResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_remote_start_stop_status(j.at("status"));

        // the optional parts of the message
    }

    friend std::ostream& operator<<(std::ostream& os, const RemoteStartTransactionResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_REMOTESTARTTRANSACTION_HPP
