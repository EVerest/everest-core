// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/NotifyReport.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string NotifyReportRequest::get_type() const {
    return "NotifyReport";
}

void to_json(json& j, const NotifyReportRequest& k) {
    // the required parts of the message
    j = json{
        {"requestId", k.requestId},
        {"generatedAt", k.generatedAt.to_rfc3339()},
        {"seqNo", k.seqNo},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.reportData) {
        j["reportData"] = json::array();
        for (auto val : k.reportData.value()) {
            j["reportData"].push_back(val);
        }
    }
    if (k.tbc) {
        j["tbc"] = k.tbc.value();
    }
}

void from_json(const json& j, NotifyReportRequest& k) {
    // the required parts of the message
    k.requestId = j.at("requestId");
    k.generatedAt = ocpp::DateTime(std::string(j.at("generatedAt")));
    k.seqNo = j.at("seqNo");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("reportData")) {
        json arr = j.at("reportData");
        std::vector<ReportData> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.reportData.emplace(vec);
    }
    if (j.contains("tbc")) {
        k.tbc.emplace(j.at("tbc"));
    }
}

/// \brief Writes the string representation of the given NotifyReportRequest \p k to the given output stream \p os
/// \returns an output stream with the NotifyReportRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyReportRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyReportResponse::get_type() const {
    return "NotifyReportResponse";
}

void to_json(json& j, const NotifyReportResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyReportResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyReportResponse \p k to the given output stream \p os
/// \returns an output stream with the NotifyReportResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyReportResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
