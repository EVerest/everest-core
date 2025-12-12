// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/RequestStartTransaction.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string RequestStartTransactionRequest::get_type() const {
    return "RequestStartTransaction";
}

void to_json(json& j, const RequestStartTransactionRequest& k) {
    // the required parts of the message
    j = json{
        {"idToken", k.idToken},
        {"remoteStartId", k.remoteStartId},
    };
    // the optional parts of the message
    if (k.evseId) {
        j["evseId"] = k.evseId.value();
    }
    if (k.groupIdToken) {
        j["groupIdToken"] = k.groupIdToken.value();
    }
    if (k.chargingProfile) {
        j["chargingProfile"] = k.chargingProfile.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, RequestStartTransactionRequest& k) {
    // the required parts of the message
    k.idToken = j.at("idToken");
    k.remoteStartId = j.at("remoteStartId");

    // the optional parts of the message
    if (j.contains("evseId")) {
        k.evseId.emplace(j.at("evseId"));
    }
    if (j.contains("groupIdToken")) {
        k.groupIdToken.emplace(j.at("groupIdToken"));
    }
    if (j.contains("chargingProfile")) {
        k.chargingProfile.emplace(j.at("chargingProfile"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given RequestStartTransactionRequest \p k to the given output stream
/// \p os
/// \returns an output stream with the RequestStartTransactionRequest written to
std::ostream& operator<<(std::ostream& os, const RequestStartTransactionRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string RequestStartTransactionResponse::get_type() const {
    return "RequestStartTransactionResponse";
}

void to_json(json& j, const RequestStartTransactionResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::request_start_stop_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.transactionId) {
        j["transactionId"] = k.transactionId.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, RequestStartTransactionResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_request_start_stop_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("transactionId")) {
        k.transactionId.emplace(j.at("transactionId"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given RequestStartTransactionResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the RequestStartTransactionResponse written to
std::ostream& operator<<(std::ostream& os, const RequestStartTransactionResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
