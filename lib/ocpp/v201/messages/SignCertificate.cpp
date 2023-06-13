// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/SignCertificate.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string SignCertificateRequest::get_type() const {
    return "SignCertificate";
}

void to_json(json& j, const SignCertificateRequest& k) {
    // the required parts of the message
    j = json{
        {"csr", k.csr},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.certificateType) {
        j["certificateType"] = conversions::certificate_signing_use_enum_to_string(k.certificateType.value());
    }
}

void from_json(const json& j, SignCertificateRequest& k) {
    // the required parts of the message
    k.csr = j.at("csr");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("certificateType")) {
        k.certificateType.emplace(conversions::string_to_certificate_signing_use_enum(j.at("certificateType")));
    }
}

/// \brief Writes the string representation of the given SignCertificateRequest \p k to the given output stream \p os
/// \returns an output stream with the SignCertificateRequest written to
std::ostream& operator<<(std::ostream& os, const SignCertificateRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string SignCertificateResponse::get_type() const {
    return "SignCertificateResponse";
}

void to_json(json& j, const SignCertificateResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::generic_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, SignCertificateResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_generic_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given SignCertificateResponse \p k to the given output stream \p os
/// \returns an output stream with the SignCertificateResponse written to
std::ostream& operator<<(std::ostream& os, const SignCertificateResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
