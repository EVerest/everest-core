// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/Get15118EVCertificate.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string Get15118EVCertificateRequest::get_type() const {
    return "Get15118EVCertificate";
}

void to_json(json& j, const Get15118EVCertificateRequest& k) {
    // the required parts of the message
    j = json{
        {"iso15118SchemaVersion", k.iso15118SchemaVersion},
        {"action", conversions::certificate_action_enum_to_string(k.action)},
        {"exiRequest", k.exiRequest},
    };
    // the optional parts of the message
    if (k.maximumContractCertificateChains) {
        j["maximumContractCertificateChains"] = k.maximumContractCertificateChains.value();
    }
    if (k.prioritizedEMAIDs) {
        j["prioritizedEMAIDs"] = json::array();
        for (const auto& val : k.prioritizedEMAIDs.value()) {
            j["prioritizedEMAIDs"].push_back(val);
        }
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, Get15118EVCertificateRequest& k) {
    // the required parts of the message
    k.iso15118SchemaVersion = j.at("iso15118SchemaVersion");
    k.action = conversions::string_to_certificate_action_enum(j.at("action"));
    k.exiRequest = j.at("exiRequest");

    // the optional parts of the message
    if (j.contains("maximumContractCertificateChains")) {
        k.maximumContractCertificateChains.emplace(j.at("maximumContractCertificateChains"));
    }
    if (j.contains("prioritizedEMAIDs")) {
        const json& arr = j.at("prioritizedEMAIDs");
        std::vector<CiString<255>> vec;
        for (const auto& val : arr) {
            vec.push_back(val);
        }
        k.prioritizedEMAIDs.emplace(vec);
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given Get15118EVCertificateRequest \p k to the given output stream \p
/// os
/// \returns an output stream with the Get15118EVCertificateRequest written to
std::ostream& operator<<(std::ostream& os, const Get15118EVCertificateRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string Get15118EVCertificateResponse::get_type() const {
    return "Get15118EVCertificateResponse";
}

void to_json(json& j, const Get15118EVCertificateResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::iso15118evcertificate_status_enum_to_string(k.status)},
        {"exiResponse", k.exiResponse},
    };
    // the optional parts of the message
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.remainingContracts) {
        j["remainingContracts"] = k.remainingContracts.value();
    }
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, Get15118EVCertificateResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_iso15118evcertificate_status_enum(j.at("status"));
    k.exiResponse = j.at("exiResponse");

    // the optional parts of the message
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("remainingContracts")) {
        k.remainingContracts.emplace(j.at("remainingContracts"));
    }
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given Get15118EVCertificateResponse \p k to the given output stream
/// \p os
/// \returns an output stream with the Get15118EVCertificateResponse written to
std::ostream& operator<<(std::ostream& os, const Get15118EVCertificateResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
