// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v16/messages/StopTransaction.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string StopTransactionRequest::get_type() const {
    return "StopTransaction";
}

void to_json(json& j, const StopTransactionRequest& k) {
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

void from_json(const json& j, StopTransactionRequest& k) {
    // the required parts of the message
    k.meterStop = j.at("meterStop");
    k.timestamp = ocpp::DateTime(std::string(j.at("timestamp")));
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

/// \brief Writes the string representation of the given StopTransactionRequest \p k to the given output stream \p os
/// \returns an output stream with the StopTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const StopTransactionRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string StopTransactionResponse::get_type() const {
    return "StopTransactionResponse";
}

void to_json(json& j, const StopTransactionResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.idTagInfo) {
        j["idTagInfo"] = k.idTagInfo.value();
    }
}

void from_json(const json& j, StopTransactionResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("idTagInfo")) {
        k.idTagInfo.emplace(j.at("idTagInfo"));
    }
}

/// \brief Writes the string representation of the given StopTransactionResponse \p k to the given output stream \p os
/// \returns an output stream with the StopTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const StopTransactionResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
