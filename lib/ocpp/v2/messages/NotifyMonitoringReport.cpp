// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
// This code is generated using the generator in 'src/code_generator/common`, please do not edit manually

#include <ocpp/v2/messages/NotifyMonitoringReport.hpp>

#include <optional>
#include <ostream>
#include <string>

using json = nlohmann::json;

namespace ocpp {
namespace v2 {

std::string NotifyMonitoringReportRequest::get_type() const {
    return "NotifyMonitoringReport";
}

void to_json(json& j, const NotifyMonitoringReportRequest& k) {
    // the required parts of the message
    j = json{
        {"requestId", k.requestId},
        {"seqNo", k.seqNo},
        {"generatedAt", k.generatedAt.to_rfc3339()},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.monitor) {
        j["monitor"] = json::array();
        for (auto val : k.monitor.value()) {
            j["monitor"].push_back(val);
        }
    }
    if (k.tbc) {
        j["tbc"] = k.tbc.value();
    }
}

void from_json(const json& j, NotifyMonitoringReportRequest& k) {
    // the required parts of the message
    k.requestId = j.at("requestId");
    k.seqNo = j.at("seqNo");
    k.generatedAt = ocpp::DateTime(std::string(j.at("generatedAt")));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("monitor")) {
        json arr = j.at("monitor");
        std::vector<MonitoringData> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.monitor.emplace(vec);
    }
    if (j.contains("tbc")) {
        k.tbc.emplace(j.at("tbc"));
    }
}

/// \brief Writes the string representation of the given NotifyMonitoringReportRequest \p k to the given output stream
/// \p os \returns an output stream with the NotifyMonitoringReportRequest written to
std::ostream& operator<<(std::ostream& os, const NotifyMonitoringReportRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string NotifyMonitoringReportResponse::get_type() const {
    return "NotifyMonitoringReportResponse";
}

void to_json(json& j, const NotifyMonitoringReportResponse& k) {
    // the required parts of the message
    j = json({}, true);
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
}

void from_json(const json& j, NotifyMonitoringReportResponse& k) {
    // the required parts of the message

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
}

/// \brief Writes the string representation of the given NotifyMonitoringReportResponse \p k to the given output stream
/// \p os \returns an output stream with the NotifyMonitoringReportResponse written to
std::ostream& operator<<(std::ostream& os, const NotifyMonitoringReportResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v2
} // namespace ocpp
