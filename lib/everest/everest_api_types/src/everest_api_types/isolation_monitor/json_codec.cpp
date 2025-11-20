// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "isolation_monitor/json_codec.hpp"
#include "isolation_monitor/API.hpp"
#include "isolation_monitor/codec.hpp"
#include "nlohmann/json.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::isolation_monitor {

void to_json(json& j, const IsolationMeasurement& k) noexcept {
    j = json{{"resistance_F_Ohm", k.resistance_F_Ohm}};
    if (k.voltage_V) {
        j["voltage_V"] = k.voltage_V.value();
    }
    if (k.voltage_to_earth_l1e_V) {
        j["voltage_to_earth_l1e_V"] = k.voltage_to_earth_l1e_V.value();
    }
    if (k.voltage_to_earth_l2e_V) {
        j["voltage_to_earth_l2e_V"] = k.voltage_to_earth_l2e_V.value();
    }
}

void from_json(const json& j, IsolationMeasurement& k) {
    k.resistance_F_Ohm = j.at("resistance_F_Ohm");
    if (j.contains("voltage_V")) {
        k.voltage_V.emplace(j.at("voltage_V"));
    }
    if (j.contains("voltage_to_earth_l1e_V")) {
        k.voltage_to_earth_l1e_V.emplace(j.at("voltage_to_earth_l1e_V"));
    }
    if (j.contains("voltage_to_earth_l2e_V")) {
        k.voltage_to_earth_l2e_V.emplace(j.at("voltage_to_earth_l2e_V"));
    }
}

void to_json(json& j, ErrorEnum const& k) noexcept {
    switch (k) {
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
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::isolation_monitor::error";
}

void from_json(json const& j, ErrorEnum& k) {
    std::string s = j;
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
    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type isolation_monitor_ErrorEnum_API_1_0");
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

} // namespace everest::lib::API::V1_0::types::isolation_monitor
