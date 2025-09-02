// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::power_supply_DC {

struct Capabilities {
    bool bidirectional;
    float current_regulation_tolerance_A;
    float peak_current_ripple_A;
    float max_export_voltage_V;
    float min_export_voltage_V;
    float max_export_current_A;
    float min_export_current_A;
    float max_export_power_W;
    std::optional<float> max_import_voltage_V;
    std::optional<float> min_import_voltage_V;
    std::optional<float> max_import_current_A;
    std::optional<float> min_import_current_A;
    std::optional<float> max_import_power_W;
    std::optional<float> conversion_efficiency_import;
    std::optional<float> conversion_efficiency_export;
};

enum class Mode {
    Off,
    Export,
    Import,
    Fault,
};

enum class ChargingPhase {
    Other,
    CableCheck,
    PreCharge,
    Charging,
};

struct ModeRequest {
    Mode mode;
    ChargingPhase charging_phase;
};

struct VoltageCurrent {
    float voltage_V; ///< Voltage in V
    float current_A; ///< Current in A
};

enum class ErrorEnum {
    CommunicationFault,
    HardwareFault,
    OverTemperature,
    UnderTemperature,
    UnderVoltageAC,
    OverVoltageAC,
    UnderVoltageDC,
    OverVoltageDC,
    OverCurrentAC,
    OverCurrentDC,
    VendorError,
    VendorWarning
};

struct Error {
    ErrorEnum type;
    std::optional<std::string> sub_type;
    std::optional<std::string> message;
};

} // namespace everest::lib::API::V1_0::types::power_supply_DC
