// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "power_supply_DC/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "power_supply_DC/API.hpp"
#include "power_supply_DC/codec.hpp"

namespace everest::lib::API::V1_0::types::power_supply_DC {

void to_json(json& j, const Capabilities& k) noexcept {
    // the required parts of the type
    j = json{
        {"bidirectional", k.bidirectional},
        {"current_regulation_tolerance_A", k.current_regulation_tolerance_A},
        {"peak_current_ripple_A", k.peak_current_ripple_A},
        {"max_export_voltage_V", k.max_export_voltage_V},
        {"min_export_voltage_V", k.min_export_voltage_V},
        {"max_export_current_A", k.max_export_current_A},
        {"min_export_current_A", k.min_export_current_A},
        {"max_export_power_W", k.max_export_power_W},
    };
    // the optional parts of the type
    if (k.max_import_voltage_V) {
        j["max_import_voltage_V"] = k.max_import_voltage_V.value();
    }
    if (k.min_import_voltage_V) {
        j["min_import_voltage_V"] = k.min_import_voltage_V.value();
    }
    if (k.max_import_current_A) {
        j["max_import_current_A"] = k.max_import_current_A.value();
    }
    if (k.min_import_current_A) {
        j["min_import_current_A"] = k.min_import_current_A.value();
    }
    if (k.max_import_power_W) {
        j["max_import_power_W"] = k.max_import_power_W.value();
    }
    if (k.conversion_efficiency_import) {
        j["conversion_efficiency_import"] = k.conversion_efficiency_import.value();
    }
    if (k.conversion_efficiency_export) {
        j["conversion_efficiency_export"] = k.conversion_efficiency_export.value();
    }
}

void from_json(const json& j, Capabilities& k) {
    // the required parts of the type
    k.bidirectional = j.at("bidirectional");
    k.current_regulation_tolerance_A = j.at("current_regulation_tolerance_A");
    k.peak_current_ripple_A = j.at("peak_current_ripple_A");
    k.max_export_voltage_V = j.at("max_export_voltage_V");
    k.min_export_voltage_V = j.at("min_export_voltage_V");
    k.max_export_current_A = j.at("max_export_current_A");
    k.min_export_current_A = j.at("min_export_current_A");
    k.max_export_power_W = j.at("max_export_power_W");

    // the optional parts of the type
    if (j.contains("max_import_voltage_V")) {
        k.max_import_voltage_V.emplace(j.at("max_import_voltage_V"));
    }
    if (j.contains("min_import_voltage_V")) {
        k.min_import_voltage_V.emplace(j.at("min_import_voltage_V"));
    }
    if (j.contains("max_import_current_A")) {
        k.max_import_current_A.emplace(j.at("max_import_current_A"));
    }
    if (j.contains("min_import_current_A")) {
        k.min_import_current_A.emplace(j.at("min_import_current_A"));
    }
    if (j.contains("max_import_power_W")) {
        k.max_import_power_W.emplace(j.at("max_import_power_W"));
    }
    if (j.contains("conversion_efficiency_import")) {
        k.conversion_efficiency_import.emplace(j.at("conversion_efficiency_import"));
    }
    if (j.contains("conversion_efficiency_export")) {
        k.conversion_efficiency_export.emplace(j.at("conversion_efficiency_export"));
    }
}

void to_json(json& j, Mode const& k) noexcept {
    switch (k) {
    case Mode::Off:
        j = "Off";
        return;
    case Mode::Export:
        j = "Export";
        return;
    case Mode::Import:
        j = "Import";
        return;
    case Mode::Fault:
        j = "Fault";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::power_supply_DC::Mode";
}

void from_json(json const& j, Mode& k) {
    std::string s = j;
    if (s == "Off") {
        k = Mode::Off;
        return;
    }
    if (s == "Export") {
        k = Mode::Export;
        return;
    }
    if (s == "Import") {
        k = Mode::Import;
        return;
    }
    if (s == "Fault") {
        k = Mode::Fault;
        return;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type Mode_API_1_0");
}

void to_json(json& j, ChargingPhase const& k) noexcept {
    switch (k) {
    case ChargingPhase::Other:
        j = "Other";
        return;
    case ChargingPhase::CableCheck:
        j = "CableCheck";
        return;
    case ChargingPhase::PreCharge:
        j = "PreCharge";
        return;
    case ChargingPhase::Charging:
        j = "Charging";
        return;
        j = "INVALID_VALUE__everest::lib::API::V1_0::types::power_supply_DC::ChargingPhase";
    }
}

void from_json(json const& j, ChargingPhase& k) {
    std::string s = j;
    if (s == "Other") {
        k = ChargingPhase::Other;
        return;
    }
    if (s == "CableCheck") {
        k = ChargingPhase::CableCheck;
        return;
    }
    if (s == "PreCharge") {
        k = ChargingPhase::PreCharge;
        return;
    }
    if (s == "Charging") {
        k = ChargingPhase::Charging;
        return;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type ChargingPhase_API_1_0");
}

void to_json(json& j, const ModeRequest& k) noexcept {
    j = json{
        {"mode", k.mode},
        {"charging_phase", k.charging_phase},
    };
}

void from_json(const json& j, ModeRequest& k) {
    k.mode = j.at("mode");
    k.charging_phase = j.at("charging_phase");
}

void to_json(json& j, const VoltageCurrent& k) noexcept {
    j = json{
        {"voltage_V", k.voltage_V},
        {"current_A", k.current_A},
    };
}

void from_json(const json& j, VoltageCurrent& k) {
    k.voltage_V = j.at("voltage_V");
    k.current_A = j.at("current_A");
}

void to_json(json& j, ErrorEnum const& k) noexcept {
    switch (k) {
    case ErrorEnum::CommunicationFault:
        j = "CommunicationFault";
        return;
    case ErrorEnum::HardwareFault:
        j = "HardwareFault";
        return;
    case ErrorEnum::OverTemperature:
        j = "OverTemperature";
        return;
    case ErrorEnum::UnderTemperature:
        j = "UnderTemperature";
        return;
    case ErrorEnum::UnderVoltageAC:
        j = "UnderVoltageAC";
        return;
    case ErrorEnum::OverVoltageAC:
        j = "OverVoltageAC";
        return;
    case ErrorEnum::UnderVoltageDC:
        j = "UnderVoltageDC";
        return;
    case ErrorEnum::OverVoltageDC:
        j = "OverVoltageDC";
        return;
    case ErrorEnum::OverCurrentAC:
        j = "OverCurrentAC";
        return;
    case ErrorEnum::OverCurrentDC:
        j = "OverCurrentDC";
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
    if (s == "CommunicationFault") {
        k = ErrorEnum::CommunicationFault;
        return;
    }
    if (s == "HardwareFault") {
        k = ErrorEnum::HardwareFault;
        return;
    }
    if (s == "OverTemperature") {
        k = ErrorEnum::OverTemperature;
        return;
    }
    if (s == "UnderTemperature") {
        k = ErrorEnum::UnderTemperature;
        return;
    }
    if (s == "UnderVoltageAC") {
        k = ErrorEnum::UnderVoltageAC;
        return;
    }
    if (s == "OverVoltageAC") {
        k = ErrorEnum::OverVoltageAC;
        return;
    }
    if (s == "UnderVoltageDC") {
        k = ErrorEnum::UnderVoltageDC;
        return;
    }
    if (s == "OverVoltageDC") {
        k = ErrorEnum::OverVoltageDC;
        return;
    }
    if (s == "OverCurrentAC") {
        k = ErrorEnum::OverCurrentAC;
        return;
    }
    if (s == "OverCurrentDC") {
        k = ErrorEnum::OverCurrentDC;
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

} // namespace everest::lib::API::V1_0::types::power_supply_DC
