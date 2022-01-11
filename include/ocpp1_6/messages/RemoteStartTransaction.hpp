// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_REMOTESTARTTRANSACTION_HPP
#define OCPP1_6_REMOTESTARTTRANSACTION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 RemoteStartTransaction message
struct RemoteStartTransactionRequest : public Message {
    CiString20Type idTag;
    boost::optional<int32_t> connectorId;
    boost::optional<ChargingProfile> chargingProfile;

    /// \brief Provides the type of this RemoteStartTransaction message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "RemoteStartTransaction";
    }

    /// \brief Conversion from a given RemoteStartTransactionRequest \p k to a given json object \p j
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

    /// \brief Conversion from a given json object \p j to a given RemoteStartTransactionRequest \p k
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

    /// \brief Writes the string representation of the given RemoteStartTransactionRequest \p k to the given output
    /// stream \p os \returns an output stream with the RemoteStartTransactionRequest written to
    friend std::ostream& operator<<(std::ostream& os, const RemoteStartTransactionRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 RemoteStartTransactionResponse message
struct RemoteStartTransactionResponse : public Message {
    RemoteStartStopStatus status;

    /// \brief Provides the type of this RemoteStartTransactionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "RemoteStartTransactionResponse";
    }

    /// \brief Conversion from a given RemoteStartTransactionResponse \p k to a given json object \p j
    friend void to_json(json& j, const RemoteStartTransactionResponse& k) {
        // the required parts of the message
        j = json{
            {"status", conversions::remote_start_stop_status_to_string(k.status)},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given RemoteStartTransactionResponse \p k
    friend void from_json(const json& j, RemoteStartTransactionResponse& k) {
        // the required parts of the message
        k.status = conversions::string_to_remote_start_stop_status(j.at("status"));

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given RemoteStartTransactionResponse \p k to the given output
    /// stream \p os \returns an output stream with the RemoteStartTransactionResponse written to
    friend std::ostream& operator<<(std::ostream& os, const RemoteStartTransactionResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_REMOTESTARTTRANSACTION_HPP
