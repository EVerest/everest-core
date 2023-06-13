// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/CertificateSigned.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string CertificateSignedRequest::get_type() const {
    return "CertificateSigned";
}

void to_json(json& j, const CertificateSignedRequest& k) {
    // the required parts of the message
    j = json{
        {"certificateChain", k.certificateChain},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.certificateType) {
        j["certificateType"] = conversions::certificate_signing_use_enum_to_string(k.certificateType.value());
    }
}

void from_json(const json& j, CertificateSignedRequest& k) {
    // the required parts of the message
    k.certificateChain = j.at("certificateChain");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("certificateType")) {
        k.certificateType.emplace(conversions::string_to_certificate_signing_use_enum(j.at("certificateType")));
    }
}

/// \brief Writes the string representation of the given CertificateSignedRequest \p k to the given output stream \p os
/// \returns an output stream with the CertificateSignedRequest written to
std::ostream& operator<<(std::ostream& os, const CertificateSignedRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string CertificateSignedResponse::get_type() const {
    return "CertificateSignedResponse";
}

void to_json(json& j, const CertificateSignedResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::certificate_signed_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, CertificateSignedResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_certificate_signed_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given CertificateSignedResponse \p k to the given output stream \p os
/// \returns an output stream with the CertificateSignedResponse written to
std::ostream& operator<<(std::ostream& os, const CertificateSignedResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
