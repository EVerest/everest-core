// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>

#include <ocpp1_6/enums.hpp>
#include <ocpp1_6/messages/RemoteStopTransaction.hpp>
#include <ocpp1_6/ocpp_types.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

std::string RemoteStopTransactionRequest::get_type() const {
    return "RemoteStopTransaction";
}

void to_json(json& j, const RemoteStopTransactionRequest& k) {
    // the required parts of the message
    j = json{
        {"transactionId", k.transactionId},
    };
    // the optional parts of the message
}

void from_json(const json& j, RemoteStopTransactionRequest& k) {
    // the required parts of the message
    k.transactionId = j.at("transactionId");

    // the optional parts of the message
}

/// \brief Writes the string representation of the given RemoteStopTransactionRequest \p k to the given output stream \p
/// os \returns an output stream with the RemoteStopTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const RemoteStopTransactionRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string RemoteStopTransactionResponse::get_type() const {
    return "RemoteStopTransactionResponse";
}

void to_json(json& j, const RemoteStopTransactionResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::remote_start_stop_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, RemoteStopTransactionResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_remote_start_stop_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given RemoteStopTransactionResponse \p k to the given output stream
/// \p os \returns an output stream with the RemoteStopTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const RemoteStopTransactionResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace ocpp1_6
