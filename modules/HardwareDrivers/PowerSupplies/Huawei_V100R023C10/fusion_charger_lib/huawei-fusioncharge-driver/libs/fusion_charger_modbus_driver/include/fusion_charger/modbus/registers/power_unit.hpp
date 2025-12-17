// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include "raw.hpp"

namespace fusion_charger::modbus_driver {
using namespace modbus::registers::data_providers;

struct PowerUnitRegisters {
    using PSURunningMode = raw_registers::SettingPowerUnitRegisters::PSURunningMode;

    DataProviderHolding<std::uint16_t> manufacturer;
    DataProviderHolding<std::uint16_t> protocol_version;
    DataProviderHolding<std::uint16_t> hardware_version;
    DataProviderStringHolding<48> software_version;
    DataProviderStringHolding<32> esn_control_board;

    DataProviderHolding<PSURunningMode> psu_running_mode;
    DataProviderMemoryHolding<6> psu_mac;
    DataProviderHolding<float> ac_input_voltage_a;
    DataProviderHolding<float> ac_input_voltage_b;
    DataProviderHolding<float> ac_input_voltage_c;
    DataProviderHolding<float> ac_input_current_a;
    DataProviderHolding<float> ac_input_current_b;
    DataProviderHolding<float> ac_input_current_c;
    DataProviderHolding<double> total_historic_input_energy;

    PowerUnitRegisters() :
        manufacturer(0),
        protocol_version(0),
        hardware_version(0),
        software_version(""),
        esn_control_board(""),
        psu_running_mode(PSURunningMode::STARTING_UP), // default value
        ac_input_voltage_a(0.0),
        ac_input_voltage_b(0.0),
        ac_input_voltage_c(0.0),
        ac_input_current_a(0.0),
        ac_input_current_b(0.0),
        ac_input_current_c(0.0),
        total_historic_input_energy(0.0) {
    }

    void add_to_registry(modbus::registers::registry::ComplexRegisterRegistry& registry) {
        raw_registers::CommonPowerUnitRegisters::DataProviders common_data_providers{
            manufacturer, protocol_version, hardware_version, software_version, esn_control_board};

        raw_registers::SettingPowerUnitRegisters::DataProviders setting_data_providers{
            psu_running_mode,           psu_mac,
            ac_input_voltage_a,         ac_input_voltage_b,
            ac_input_voltage_c,         ac_input_current_a,
            ac_input_current_b,         ac_input_current_c,
            total_historic_input_energy};

        registry.add(std::make_unique<raw_registers::CommonPowerUnitRegisters>(common_data_providers));

        registry.add(std::make_unique<raw_registers::SettingPowerUnitRegisters>(setting_data_providers));
    }
};

}; // namespace fusion_charger::modbus_driver
