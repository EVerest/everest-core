// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <ocpp/v16/messages/CertificateSigned.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string CertificateSignedRequest::get_type() const {
    return "CertificateSigned";
}

void to_json(json& j, const CertificateSignedRequest& k) {
    // the required parts of the message
    j = json{
        {"certificateChain", k.certificateChain},
    };
    // the optional parts of the message
}

void from_json(const json& j, CertificateSignedRequest& k) {
    // the required parts of the message
    k.certificateChain = j.at("certificateChain");

    // the optional parts of the message
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
        {"status", conversions::certificate_signed_status_enum_type_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, CertificateSignedResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_certificate_signed_status_enum_type(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given CertificateSignedResponse \p k to the given output stream \p os
/// \returns an output stream with the CertificateSignedResponse written to
std::ostream& operator<<(std::ostream& os, const CertificateSignedResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
