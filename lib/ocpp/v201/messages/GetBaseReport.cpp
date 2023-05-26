// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <nlohmann/json.hpp>
#include <optional>

#include <ocpp/v201/messages/GetBaseReport.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string GetBaseReportRequest::get_type() const {
    return "GetBaseReport";
}

void to_json(json& j, const GetBaseReportRequest& k) {
    // the required parts of the message
    j = json{
        {"requestId", k.requestId},
        {"reportBase", conversions::report_base_enum_to_string(k.reportBase)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, GetBaseReportRequest& k) {
    // the required parts of the message
    k.requestId = j.at("requestId");
    k.reportBase = conversions::string_to_report_base_enum(j.at("reportBase"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given GetBaseReportRequest \p k to the given output stream \p os
/// \returns an output stream with the GetBaseReportRequest written to
std::ostream& operator<<(std::ostream& os, const GetBaseReportRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetBaseReportResponse::get_type() const {
    return "GetBaseReportResponse";
}

void to_json(json& j, const GetBaseReportResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::generic_device_model_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
}

void from_json(const json& j, GetBaseReportResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_generic_device_model_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
}

/// \brief Writes the string representation of the given GetBaseReportResponse \p k to the given output stream \p os
/// \returns an output stream with the GetBaseReportResponse written to
std::ostream& operator<<(std::ostream& os, const GetBaseReportResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
