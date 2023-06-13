// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/GetMonitoringReport.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string GetMonitoringReportRequest::get_type() const {
    return "GetMonitoringReport";
}

void to_json(json& j, const GetMonitoringReportRequest& k) {
    // the required parts of the message
    j = json{
        {"requestId", k.requestId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.componentVariable) {
        j["componentVariable"] = json::array();
        for (auto val : k.componentVariable.value()) {
            j["componentVariable"].push_back(val);
        }
    }
    if (k.monitoringCriteria) {
        j["monitoringCriteria"] = json::array();
        for (auto val : k.monitoringCriteria.value()) {
            j["monitoringCriteria"].push_back(conversions::monitoring_criterion_enum_to_string(val));
        }
    }
}

void from_json(const json& j, GetMonitoringReportRequest& k) {
    // the required parts of the message
    k.requestId = j.at("requestId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("componentVariable")) {
        json arr = j.at("componentVariable");
        std::vector<ComponentVariable> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.componentVariable.emplace(vec);
    }
    if (j.contains("monitoringCriteria")) {
        json arr = j.at("monitoringCriteria");
        std::vector<MonitoringCriterionEnum> vec;
        for (auto val : arr) {
            vec.push_back(conversions::string_to_monitoring_criterion_enum(val));
        }
        k.monitoringCriteria.emplace(vec);
    }
}

/// \brief Writes the string representation of the given GetMonitoringReportRequest \p k to the given output stream \p
/// os \returns an output stream with the GetMonitoringReportRequest written to
std::ostream& operator<<(std::ostream& os, const GetMonitoringReportRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetMonitoringReportResponse::get_type() const {
    return "GetMonitoringReportResponse";
}

void to_json(json& j, const GetMonitoringReportResponse& k) {
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

void from_json(const json& j, GetMonitoringReportResponse& k) {
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

/// \brief Writes the string representation of the given GetMonitoringReportResponse \p k to the given output stream \p
/// os \returns an output stream with the GetMonitoringReportResponse written to
std::ostream& operator<<(std::ostream& os, const GetMonitoringReportResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
