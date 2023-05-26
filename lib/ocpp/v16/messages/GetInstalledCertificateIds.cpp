// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/v16/messages/GetInstalledCertificateIds.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v16 {

std::string GetInstalledCertificateIdsRequest::get_type() const {
    return "GetInstalledCertificateIds";
}

void to_json(json& j, const GetInstalledCertificateIdsRequest& k) {
    // the required parts of the message
    j = json{
        {"certificateType", conversions::certificate_use_enum_type_to_string(k.certificateType)},
    };
    // the optional parts of the message
}

void from_json(const json& j, GetInstalledCertificateIdsRequest& k) {
    // the required parts of the message
    k.certificateType = conversions::string_to_certificate_use_enum_type(j.at("certificateType"));

    // the optional parts of the message
}

/// \brief Writes the string representation of the given GetInstalledCertificateIdsRequest \p k to the given output
/// stream \p os \returns an output stream with the GetInstalledCertificateIdsRequest written to
std::ostream& operator<<(std::ostream& os, const GetInstalledCertificateIdsRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetInstalledCertificateIdsResponse::get_type() const {
    return "GetInstalledCertificateIdsResponse";
}

void to_json(json& j, const GetInstalledCertificateIdsResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::get_installed_certificate_status_enum_type_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.certificateHashData) {
        j["certificateHashData"] = json::array();
        for (auto val : k.certificateHashData.value()) {
            j["certificateHashData"].push_back(val);
        }
    }
}

void from_json(const json& j, GetInstalledCertificateIdsResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_get_installed_certificate_status_enum_type(j.at("status"));

    // the optional parts of the message
    if (j.contains("certificateHashData")) {
        json arr = j.at("certificateHashData");
        std::vector<CertificateHashDataType> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.certificateHashData.emplace(vec);
    }
}

/// \brief Writes the string representation of the given GetInstalledCertificateIdsResponse \p k to the given output
/// stream \p os \returns an output stream with the GetInstalledCertificateIdsResponse written to
std::ostream& operator<<(std::ostream& os, const GetInstalledCertificateIdsResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v16
} // namespace ocpp
