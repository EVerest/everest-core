// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <boost/optional/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp/v16/messages/RemoteStartTransaction.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string RemoteStartTransactionRequest::get_type() const {
    return "RemoteStartTransaction";
}

void to_json(json& j, const RemoteStartTransactionRequest& k) {
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

void from_json(const json& j, RemoteStartTransactionRequest& k) {
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

/// \brief Writes the string representation of the given RemoteStartTransactionRequest \p k to the given output stream
/// \p os \returns an output stream with the RemoteStartTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const RemoteStartTransactionRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string RemoteStartTransactionResponse::get_type() const {
    return "RemoteStartTransactionResponse";
}

void to_json(json& j, const RemoteStartTransactionResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::remote_start_stop_status_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, RemoteStartTransactionResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_remote_start_stop_status(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given RemoteStartTransactionResponse \p k to the given output stream
/// \p os \returns an output stream with the RemoteStartTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const RemoteStartTransactionResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
