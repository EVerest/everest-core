// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v16/messages/StartTransaction.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string StartTransactionRequest::get_type() const {
    return "StartTransaction";
}

void to_json(json& j, const StartTransactionRequest& k) {
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

void from_json(const json& j, StartTransactionRequest& k) {
    // the required parts of the message
    k.connectorId = j.at("connectorId");
    k.idTag = j.at("idTag");
    k.meterStart = j.at("meterStart");
    k.timestamp = ocpp::DateTime(std::string(j.at("timestamp")));

    // the optional parts of the message
    if (j.contains("reservationId")) {
        k.reservationId.emplace(j.at("reservationId"));
    }
}

/// \brief Writes the string representation of the given StartTransactionRequest \p k to the given output stream \p os
/// \returns an output stream with the StartTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const StartTransactionRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string StartTransactionResponse::get_type() const {
    return "StartTransactionResponse";
}

void to_json(json& j, const StartTransactionResponse& k) {
    // the required parts of the message
    j = json{
        {"idTagInfo", k.idTagInfo},
        {"transactionId", k.transactionId},
    };
    // the optional parts of the message
}

void from_json(const json& j, StartTransactionResponse& k) {
    // the required parts of the message
    k.idTagInfo = j.at("idTagInfo");
    k.transactionId = j.at("transactionId");

    // the optional parts of the message
}

/// \brief Writes the string representation of the given StartTransactionResponse \p k to the given output stream \p os
/// \returns an output stream with the StartTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const StartTransactionResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
