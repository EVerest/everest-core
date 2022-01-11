// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_STARTTRANSACTION_HPP
#define OCPP1_6_STARTTRANSACTION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 StartTransaction message
struct StartTransactionRequest : public Message {
    int32_t connectorId;
    CiString20Type idTag;
    int32_t meterStart;
    DateTime timestamp;
    boost::optional<int32_t> reservationId;

    /// \brief Provides the type of this StartTransaction message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "StartTransaction";
    }

    /// \brief Conversion from a given StartTransactionRequest \p k to a given json object \p j
    friend void to_json(json& j, const StartTransactionRequest& k) {
        // the required parts of the message
        j = json{
            {"connectorId", k.connectorId},
            {"idTag", k.idTag},
            {"meterStart", k.meterStart},
            {"timestamp", k.timestamp.to_rfc3339()},
        };
        // the optional parts of the message
        if (k.reservationId) {
            j["reservationId"] = k.reservationId.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given StartTransactionRequest \p k
    friend void from_json(const json& j, StartTransactionRequest& k) {
        // the required parts of the message
        k.connectorId = j.at("connectorId");
        k.idTag = j.at("idTag");
        k.meterStart = j.at("meterStart");
        k.timestamp = DateTime(std::string(j.at("timestamp")));
        ;

        // the optional parts of the message
        if (j.contains("reservationId")) {
            k.reservationId.emplace(j.at("reservationId"));
        }
    }

    /// \brief Writes the string representation of the given StartTransactionRequest \p k to the given output stream \p
    /// os \returns an output stream with the StartTransactionRequest written to
    friend std::ostream& operator<<(std::ostream& os, const StartTransactionRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 StartTransactionResponse message
struct StartTransactionResponse : public Message {
    IdTagInfo idTagInfo;
    int32_t transactionId;

    /// \brief Provides the type of this StartTransactionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "StartTransactionResponse";
    }

    /// \brief Conversion from a given StartTransactionResponse \p k to a given json object \p j
    friend void to_json(json& j, const StartTransactionResponse& k) {
        // the required parts of the message
        j = json{
            {"idTagInfo", k.idTagInfo},
            {"transactionId", k.transactionId},
        };
        // the optional parts of the message
    }

    /// \brief Conversion from a given json object \p j to a given StartTransactionResponse \p k
    friend void from_json(const json& j, StartTransactionResponse& k) {
        // the required parts of the message
        k.idTagInfo = j.at("idTagInfo");
        k.transactionId = j.at("transactionId");

        // the optional parts of the message
    }

    /// \brief Writes the string representation of the given StartTransactionResponse \p k to the given output stream \p
    /// os \returns an output stream with the StartTransactionResponse written to
    friend std::ostream& operator<<(std::ostream& os, const StartTransactionResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_STARTTRANSACTION_HPP
