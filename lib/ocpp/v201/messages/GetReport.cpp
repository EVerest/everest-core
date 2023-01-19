// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <boost/optional/optional.hpp>
#include <nlohmann/json.hpp>

#include <ocpp/v201/messages/GetReport.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string GetReportRequest::get_type() const {
    return "GetReport";
}

void to_json(json& j, const GetReportRequest& k) {
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
    if (k.componentCriteria) {
        j["componentCriteria"] = json::array();
        for (auto val : k.componentCriteria.value()) {
            //FIXME(piet): Fix this in code generator
            j["componentCriteria"].push_back(conversions::component_criterion_enum_to_string(val));
        }
    }
}

void from_json(const json& j, GetReportRequest& k) {
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
    if (j.contains("componentCriteria")) {
        json arr = j.at("componentCriteria");
        std::vector<ComponentCriterionEnum> vec;
        for (auto val : arr) {
            //FIXME(piet): Fix this in code generator
            vec.push_back(conversions::string_to_component_criterion_enum(val));
        }
        k.componentCriteria.emplace(vec);
    }
}

/// \brief Writes the string representation of the given GetReportRequest \p k to the given output stream \p os
/// \returns an output stream with the GetReportRequest written to
std::ostream& operator<<(std::ostream& os, const GetReportRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetReportResponse::get_type() const {
    return "GetReportResponse";
}

void to_json(json& j, const GetReportResponse& k) {
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

void from_json(const json& j, GetReportResponse& k) {
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

/// \brief Writes the string representation of the given GetReportResponse \p k to the given output stream \p os
/// \returns an output stream with the GetReportResponse written to
std::ostream& operator<<(std::ostream& os, const GetReportResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
