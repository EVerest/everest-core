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
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::power_supply_DC::ErrorEnum";
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

void to_json(json& j, const Error& k) noexcept {
    j = json{
        {"type", k.type},
    };
    if (k.sub_type) {
        j["sub_type"] = k.sub_type.value();
    }
    if (k.message) {
        j["message"] = k.message.value();
    };
}

void from_json(const json& j, Error& k) {
    k.type = j.at("type");
    if (j.contains("sub_type")) {
        k.sub_type.emplace(j.at("sub_type"));
    }
    if (j.contains("message")) {
        k.message.emplace(j.at("message"));
    }
}

} // namespace everest::lib::API::V1_0::types::over_voltage_monitor
