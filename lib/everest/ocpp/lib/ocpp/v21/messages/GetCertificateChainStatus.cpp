// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v21/messages/GetCertificateChainStatus.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v21 {

std::string GetCertificateChainStatusRequest::get_type() const {
    return "GetCertificateChainStatus";
}

void to_json(json& j, const GetCertificateChainStatusRequest& k) {
    // the required parts of the message
    j = json{
        {"certificateStatusRequests", k.certificateStatusRequests},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetCertificateChainStatusRequest& k) {
    // the required parts of the message
    for (const auto& val : j.at("certificateStatusRequests")) {
        k.certificateStatusRequests.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetCertificateChainStatusRequest \p k to the given output
/// stream \p os
/// \returns an output stream with the GetCertificateChainStatusRequest written to
std::ostream& operator<<(std::ostream& os, const GetCertificateChainStatusRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetCertificateChainStatusResponse::get_type() const {
    return "GetCertificateChainStatusResponse";
}

void to_json(json& j, const GetCertificateChainStatusResponse& k) {
    // the required parts of the message
    j = json{
        {"certificateStatus", k.certificateStatus},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetCertificateChainStatusResponse& k) {
    // the required parts of the message
    for (const auto& val : j.at("certificateStatus")) {
        k.certificateStatus.push_back(val);
    }

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetCertificateChainStatusResponse \p k to the given output
/// stream \p os
/// \returns an output stream with the GetCertificateChainStatusResponse written to
std::ostream& operator<<(std::ostream& os, const GetCertificateChainStatusResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v21
} // namespace ocpp
