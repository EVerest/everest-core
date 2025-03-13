// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/GetTransactionStatus.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string GetTransactionStatusRequest::get_type() const {
    return "GetTransactionStatus";
}

void to_json(json& j, const GetTransactionStatusRequest& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.transactionId) {
        j["transactionId"] = k.transactionId.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetTransactionStatusRequest& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("transactionId")) {
        k.transactionId.emplace(j.at("transactionId"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetTransactionStatusRequest \p k to the given output stream \p
/// os \returns an output stream with the GetTransactionStatusRequest written to
std::ostream& operator<<(std::ostream& os, const GetTransactionStatusRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetTransactionStatusResponse::get_type() const {
    return "GetTransactionStatusResponse";
}

void to_json(json& j, const GetTransactionStatusResponse& k) {
    // the required parts of the message
    j = json{
        {"messagesInQueue", k.messagesInQueue},
    };
    // the optional parts of the message
    if (k.ongoingIndicator) {
        j["ongoingIndicator"] = k.ongoingIndicator.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetTransactionStatusResponse& k) {
    // the required parts of the message
    k.messagesInQueue = j.at("messagesInQueue");

    // the optional parts of the message
    if (j.contains("ongoingIndicator")) {
        k.ongoingIndicator.emplace(j.at("ongoingIndicator"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetTransactionStatusResponse \p k to the given output stream \p
/// os \returns an output stream with the GetTransactionStatusResponse written to
std::ostream& operator<<(std::ostream& os, const GetTransactionStatusResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
