// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <boost/optional/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp1_6/messages/GetDiagnostics.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/types.hpp>

using json = nlohmann::json;

namespace ocpp1_6 {

std::string GetDiagnosticsRequest::get_type() const {
    return "GetDiagnostics";
}

void to_json(json& j, const GetDiagnosticsRequest& k) {
    // the required parts of the message
    j = json{
        {"location", k.location},
    };
    // the optional parts of the message
    if (k.retries) {
        j["retries"] = k.retries.value();
    }
    if (k.retryInterval) {
        j["retryInterval"] = k.retryInterval.value();
    }
    if (k.startTime) {
        j["startTime"] = k.startTime.value().to_rfc3339();
    }
    if (k.stopTime) {
        j["stopTime"] = k.stopTime.value().to_rfc3339();
    }
}

void from_json(const json& j, GetDiagnosticsRequest& k) {
    // the required parts of the message
    k.location = j.at("location");

    // the optional parts of the message
    if (j.contains("retries")) {
        k.retries.emplace(j.at("retries"));
    }
    if (j.contains("retryInterval")) {
        k.retryInterval.emplace(j.at("retryInterval"));
    }
    if (j.contains("startTime")) {
        k.startTime.emplace(DateTime(std::string(j.at("startTime"))));
    }
    if (j.contains("stopTime")) {
        k.stopTime.emplace(DateTime(std::string(j.at("stopTime"))));
    }
}

/// \brief Writes the string representation of the given GetDiagnosticsRequest \p k to the given output stream \p os
/// \returns an output stream with the GetDiagnosticsRequest written to
std::ostream& operator<<(std::ostream& os, const GetDiagnosticsRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetDiagnosticsResponse::get_type() const {
    return "GetDiagnosticsResponse";
}

void to_json(json& j, const GetDiagnosticsResponse& k) {
    // the required parts of the message
    j = json({});
    // the optional parts of the message
    if (k.fileName) {
        j["fileName"] = k.fileName.value();
    }
}

void from_json(const json& j, GetDiagnosticsResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("fileName")) {
        k.fileName.emplace(j.at("fileName"));
    }
}

/// \brief Writes the string representation of the given GetDiagnosticsResponse \p k to the given output stream \p os
/// \returns an output stream with the GetDiagnosticsResponse written to
std::ostream& operator<<(std::ostream& os, const GetDiagnosticsResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace ocpp1_6
