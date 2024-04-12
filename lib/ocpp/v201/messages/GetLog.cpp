// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <ostream>
#include <string>

#include <optional>

#include <ocpp/v201/messages/GetLog.hpp>

using json = nlohmann::json;

namespace ocpp {
namespace v201 {

std::string GetLogRequest::get_type() const {
    return "GetLog";
}

void to_json(json& j, const GetLogRequest& k) {
    // the required parts of the message
    j = json{
        {"log", k.log},
        {"logType", conversions::log_enum_to_string(k.logType)},
        {"requestId", k.requestId},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.retries) {
        j["retries"] = k.retries.value();
    }
    if (k.retryInterval) {
        j["retryInterval"] = k.retryInterval.value();
    }
}

void from_json(const json& j, GetLogRequest& k) {
    // the required parts of the message
    k.log = j.at("log");
    k.logType = conversions::string_to_log_enum(j.at("logType"));
    k.requestId = j.at("requestId");

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("retries")) {
        k.retries.emplace(j.at("retries"));
    }
    if (j.contains("retryInterval")) {
        k.retryInterval.emplace(j.at("retryInterval"));
    }
}

/// \brief Writes the string representation of the given GetLogRequest \p k to the given output stream \p os
/// \returns an output stream with the GetLogRequest written to
std::ostream& operator<<(std::ostream& os, const GetLogRequest& k) {
    os << json(k).dump(4);
    return os;
}

std::string GetLogResponse::get_type() const {
    return "GetLogResponse";
}

void to_json(json& j, const GetLogResponse& k) {
    // the required parts of the message
    j = json{
        {"status", conversions::log_status_enum_to_string(k.status)},
    };
    // the optional parts of the message
    if (k.customData) {
        j["customData"] = k.customData.value();
    }
    if (k.statusInfo) {
        j["statusInfo"] = k.statusInfo.value();
    }
    if (k.filename) {
        j["filename"] = k.filename.value();
    }
}

void from_json(const json& j, GetLogResponse& k) {
    // the required parts of the message
    k.status = conversions::string_to_log_status_enum(j.at("status"));

    // the optional parts of the message
    if (j.contains("customData")) {
        k.customData.emplace(j.at("customData"));
    }
    if (j.contains("statusInfo")) {
        k.statusInfo.emplace(j.at("statusInfo"));
    }
    if (j.contains("filename")) {
        k.filename.emplace(j.at("filename"));
    }
}

/// \brief Writes the string representation of the given GetLogResponse \p k to the given output stream \p os
/// \returns an output stream with the GetLogResponse written to
std::ostream& operator<<(std::ostream& os, const GetLogResponse& k) {
    os << json(k).dump(4);
    return os;
}

} // namespace v201
} // namespace ocpp
