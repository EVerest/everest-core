// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <boost/optional/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp/v201/messages/CustomerInformation.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string CustomerInformationRequest::get_type() const {
    return "CustomerInformation";
}

void to_json(json& j, const CustomerInformationRequest& k) {
    // the required parts of the message
    j = json{
        {"requestId", k.requestId},
        {"report", k.report},
        {"clear", k.clear},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.customerCertificate) {
        j["customerCertificate"] = k.customerCertificate.value();
    }
    if (k.idToken) {
        j["idToken"] = k.idToken.value();
    }
    if (k.customerIdentifier) {
        j["customerIdentifier"] = k.customerIdentifier.value();
    }
}

void from_json(const json& j, CustomerInformationRequest& k) {
    // the required parts of the message
    k.requestId = j.at("requestId");
    k.report = j.at("report");
    k.clear = j.at("clear");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("customerCertificate")) {
        k.customerCertificate.emplace(j.at("customerCertificate"));
    }
    if (j.contains("idToken")) {
        k.idToken.emplace(j.at("idToken"));
    }
    if (j.contains("customerIdentifier")) {
        k.customerIdentifier.emplace(j.at("customerIdentifier"));
    }
}

/// \brief Writes the string representation of the given CustomerInformationRequest \p k to the given output stream \p
/// os \returns an output stream with the CustomerInformationRequest written to
std::ostream& operator<<(std::ostream& os, const CustomerInformationRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string CustomerInformationResponse::get_type() const {
    return "CustomerInformationResponse";
}

void to_json(json& j, const CustomerInformationResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::customer_information_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, CustomerInformationResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_customer_information_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given CustomerInformationResponse \p k to the given output stream \p
/// os \returns an output stream with the CustomerInformationResponse written to
std::ostream& operator<<(std::ostream& os, const CustomerInformationResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
