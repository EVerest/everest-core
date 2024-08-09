// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v201/messages/DeleteCertificate.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string DeleteCertificateRequest::get_type() const {
    return "DeleteCertificate";
}

void to_json(json& j, const DeleteCertificateRequest& k) {
    // the required parts of the message
    j = json{
        {"certificateHashData", k.certificateHashData},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, DeleteCertificateRequest& k) {
    // the required parts of the message
    k.certificateHashData = j.at("certificateHashData");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given DeleteCertificateRequest \p k to the given output stream \p os
/// \returns an output stream with the DeleteCertificateRequest written to
std::ostream& operator<<(std::ostream& os, const DeleteCertificateRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string DeleteCertificateResponse::get_type() const {
    return "DeleteCertificateResponse";
}

void to_json(json& j, const DeleteCertificateResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::delete_certificate_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, DeleteCertificateResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_delete_certificate_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given DeleteCertificateResponse \p k to the given output stream \p os
/// \returns an output stream with the DeleteCertificateResponse written to
std::ostream& operator<<(std::ostream& os, const DeleteCertificateResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
