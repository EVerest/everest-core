// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_STOPTRANSACTION_HPP
#define OCPP1_6_STOPTRANSACTION_HPP

#include <ocpp1_6/ocpp_types.hpp>

namespace ocpp1_6 {

/// \brief Contains a OCPP 1.6 StopTransaction message
struct StopTransactionRequest : public Message {
    int32_t meterStop;
    DateTime timestamp;
    int32_t transactionId;
    boost::optional<CiString20Type> idTag;
    boost::optional<Reason> reason;
    boost::optional<std::vector<TransactionData>> transactionData;

    /// \brief Provides the type of this StopTransaction message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "StopTransaction";
    }

    /// \brief Conversion from a given StopTransactionRequest \p k to a given json object \p j
    friend void to_json(json& j, const StopTransactionRequest& k) {
        // the required parts of the message
        j = json{
            {"meterStop", k.meterStop},
            {"timestamp", k.timestamp.to_rfc3339()},
            {"transactionId", k.transactionId},
        };
        // the optional parts of the message
        if (k.idTag) {
            j["idTag"] = k.idTag.value();
        }
        if (k.reason) {
            j["reason"] = conversions::reason_to_string(k.reason.value());
        }
        if (k.transactionData) {
            j["transactionData"] = json::array();
            for (auto val : k.transactionData.value()) {
                j["transactionData"].push_back(val);
            }
        }
    }

    /// \brief Conversion from a given json object \p j to a given StopTransactionRequest \p k
    friend void from_json(const json& j, StopTransactionRequest& k) {
        // the required parts of the message
        k.meterStop = j.at("meterStop");
        k.timestamp = DateTime(std::string(j.at("timestamp")));
        ;
        k.transactionId = j.at("transactionId");

        // the optional parts of the message
        if (j.contains("idTag")) {
            k.idTag.emplace(j.at("idTag"));
        }
        if (j.contains("reason")) {
            k.reason.emplace(conversions::string_to_reason(j.at("reason")));
        }
        if (j.contains("transactionData")) {
            json arr = j.at("transactionData");
            std::vector<TransactionData> vec;
            for (auto val : arr) {
                vec.push_back(val);
            }
            k.transactionData.emplace(vec);
        }
    }

    /// \brief Writes the string representation of the given StopTransactionRequest \p k to the given output stream \p
    /// os \returns an output stream with the StopTransactionRequest written to
    friend std::ostream& operator<<(std::ostream& os, const StopTransactionRequest& k) {
        os << json(k).dump(4);
        return os;
    }
};

/// \brief Contains a OCPP 1.6 StopTransactionResponse message
struct StopTransactionResponse : public Message {
    boost::optional<IdTagInfo> idTagInfo;

    /// \brief Provides the type of this StopTransactionResponse message as a human readable string
    /// \returns the message type as a human readable string
    std::string get_type() const {
        return "StopTransactionResponse";
    }

    /// \brief Conversion from a given StopTransactionResponse \p k to a given json object \p j
    friend void to_json(json& j, const StopTransactionResponse& k) {
        // the required parts of the message
        j = json({});
        // the optional parts of the message
        if (k.idTagInfo) {
            j["idTagInfo"] = k.idTagInfo.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given StopTransactionResponse \p k
    friend void from_json(const json& j, StopTransactionResponse& k) {
        // the required parts of the message

        // the optional parts of the message
        if (j.contains("idTagInfo")) {
            k.idTagInfo.emplace(j.at("idTagInfo"));
        }
    }

    /// \brief Writes the string representation of the given StopTransactionResponse \p k to the given output stream \p
    /// os \returns an output stream with the StopTransactionResponse written to
    friend std::ostream& operator<<(std::ostream& os, const StopTransactionResponse& k) {
        os << json(k).dump(4);
        return os;
    }
};

} // namespace ocpp1_6

#endif // OCPP1_6_STOPTRANSACTION_HPP
