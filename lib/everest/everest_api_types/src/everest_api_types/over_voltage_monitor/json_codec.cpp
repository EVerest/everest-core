// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "over_voltage_monitor/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "over_voltage_monitor/API.hpp"

namespace everest::lib::API::V1_0::types::over_voltage_monitor {

void to_json(json& j, ErrorEnum const& k) noexcept {
    switch (k) {
    case ErrorEnum::MREC5OverVoltage:
        j = "MREC5OverVoltage";
        return;
    case ErrorEnum::DeviceFault:
        j = "DeviceFault";
        return;
    case ErrorEnum::CommunicationFault:
        j = "CommunicationFault";
        return;
    case ErrorEnum::VendorError:
        j = "VendorError";
        return;
    case ErrorEnum::VendorWarning:
        j = "VendorWarning";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::over_voltage_monitor::ErrorEnum";
}

void from_json(json const& j, ErrorEnum& k) {
    std::string s = j;
    if (s == "MREC5OverVoltage") {
        k = ErrorEnum::MREC5OverVoltage;
        return;
    }
    if (s == "DeviceFault") {
        k = ErrorEnum::DeviceFault;
        return;
    }
    if (s == "CommunicationFault") {
        k = ErrorEnum::CommunicationFault;
        return;
    }
    if (s == "VendorError") {
        k = ErrorEnum::VendorError;
        return;
    }
    if (s == "VendorWarning") {
        k = ErrorEnum::VendorWarning;
        return;
    }
    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type ErrorEnum_API_1_0");
}

void to_json(json& j, ErrorSeverityEnum const& k) noexcept {
    switch (k) {
    case ErrorSeverityEnum::Low:
        j = "Low";
        return;
    case ErrorSeverityEnum::Medium:
        j = "Medium";
        return;
    case ErrorSeverityEnum::High:
        j = "High";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::over_voltage_monitor::ErrorSeverityEnum";
}

void from_json(json const& j, ErrorSeverityEnum& k) {
    std::string s = j;
    if (s == "Low") {
        k = ErrorSeverityEnum::Low;
        return;
    }
    if (s == "Medium") {
        k = ErrorSeverityEnum::Medium;
        return;
    }
    if (s == "High") {
        k = ErrorSeverityEnum::High;
        return;
    }
    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type ErrorSeverityEnum_API_1_0");
}

void to_json(json& j, const Error& k) noexcept {
    j = json{
        {"type", k.type},
    };
    if (k.sub_type) {
        j["sub_type"] = k.sub_type.value();
    }
    if (k.message) {
        j["message"] = k.message.value();
    }
    if (k.severity) {
        j["severity"] = k.severity.value();
    }
}

void from_json(const json& j, Error& k) {
    k.type = j.at("type");
    if (j.contains("sub_type")) {
        k.sub_type.emplace(j.at("sub_type"));
    }
    if (j.contains("message")) {
        k.message.emplace(j.at("message"));
    }
    if (j.contains("severity")) {
        k.severity.emplace(j.at("severity"));
    }
}

void to_json(json& j, const OverVoltageLimits& k) noexcept {
    j = json{{"emergency_limit_V", k.emergency_limit_V}, {"error_limit_V", k.error_limit_V}};
}

void from_json(const json& j, OverVoltageLimits& k) {
    k.emergency_limit_V = j.at("emergency_limit_V");
    k.error_limit_V = j.at("error_limit_V");
}

} // namespace everest::lib::API::V1_0::types::over_voltage_monitor
