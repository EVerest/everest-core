// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v201/messages/InstallCertificate.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string InstallCertificateRequest::get_type() const {
    return "InstallCertificate";
}

void to_json(json& j, const InstallCertificateRequest& k) {
    // the required parts of the message
    j = json{
        {"certificateType", conversions::install_certificate_use_enum_to_string(k.certificateType)},
        {"certificate", k.certificate},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, InstallCertificateRequest& k) {
    // the required parts of the message
    k.certificateType = conversions::string_to_install_certificate_use_enum(j.at("certificateType"));
    k.certificate = j.at("certificate");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given InstallCertificateRequest \p k to the given output stream \p os
/// \returns an output stream with the InstallCertificateRequest written to
std::ostream& operator<<(std::ostream& os, const InstallCertificateRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string InstallCertificateResponse::get_type() const {
    return "InstallCertificateResponse";
}

void to_json(json& j, const InstallCertificateResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::install_certificate_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, InstallCertificateResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_install_certificate_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given InstallCertificateResponse \p k to the given output stream \p
/// os \returns an output stream with the InstallCertificateResponse written to
std::ostream& operator<<(std::ostream& os, const InstallCertificateResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
