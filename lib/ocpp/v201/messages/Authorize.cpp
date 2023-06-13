// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/Authorize.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string AuthorizeRequest::get_type() const {
    return "Authorize";
}

void to_json(json& j, const AuthorizeRequest& k) {
    // the required parts of the message
    j = json{
        {"idToken", k.idToken},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.certificate) {
        j["certificate"] = k.certificate.value();
    }
    if (k.iso15118CertificateHashData) {
        j["iso15118CertificateHashData"] = json::array();
        for (auto val : k.iso15118CertificateHashData.value()) {
            j["iso15118CertificateHashData"].push_back(val);
        }
    }
}

void from_json(const json& j, AuthorizeRequest& k) {
    // the required parts of the message
    k.idToken = j.at("idToken");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("certificate")) {
        k.certificate.emplace(j.at("certificate"));
    }
    if (j.contains("iso15118CertificateHashData")) {
        json arr = j.at("iso15118CertificateHashData");
        std::vector<OCSPRequestData> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.iso15118CertificateHashData.emplace(vec);
    }
}

/// \brief Writes the string representation of the given AuthorizeRequest \p k to the given output stream \p os
/// \returns an output stream with the AuthorizeRequest written to
std::ostream& operator<<(std::ostream& os, const AuthorizeRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string AuthorizeResponse::get_type() const {
    return "AuthorizeResponse";
}

void to_json(json& j, const AuthorizeResponse& k) {
    // the required parts of the message
    j = json{
        {"idTokenInfo", k.idTokenInfo},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.certificateStatus) {
        j["certificateStatus"] = conversions::authorize_certificate_status_enum_to_string(k.certificateStatus.value());
    }
}

void from_json(const json& j, AuthorizeResponse& k) {
    // the required parts of the message
    k.idTokenInfo = j.at("idTokenInfo");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("certificateStatus")) {
        k.certificateStatus.emplace(
            conversions::string_to_authorize_certificate_status_enum(j.at("certificateStatus")));
    }
}

/// \brief Writes the string representation of the given AuthorizeResponse \p k to the given output stream \p os
/// \returns an output stream with the AuthorizeResponse written to
std::ostream& operator<<(std::ostream& os, const AuthorizeResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
