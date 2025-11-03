// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v16/messages/SignCertificate.hpp>

#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string SignCertificateRequest::get_type() const {
    return "SignCertificate";
}

void to_json(json& j, const SignCertificateRequest& k) {
    // the required parts of the message
    j = json{
        {"csr", k.csr},
    };
    // the optional parts of the message
}

void from_json(const json& j, SignCertificateRequest& k) {
    // the required parts of the message
    k.csr = j.at("csr");

    // the optional parts of the message
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
        {"status", conversions::generic_status_enum_type_to_string(k.status)},
    };
    // the optional parts of the message
}

void from_json(const json& j, SignCertificateResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_generic_status_enum_type(j.at("status"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given SignCertificateResponse \p k to the given output stream \p os
/// \returns an output stream with the SignCertificateResponse written to
std::ostream& operator<<(std::ostream& os, const SignCertificateResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
