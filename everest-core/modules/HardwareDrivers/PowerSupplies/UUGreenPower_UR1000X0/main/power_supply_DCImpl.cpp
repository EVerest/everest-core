// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "power_supply_DCImpl.hpp"

namespace module {
namespace main {

void power_supply_DCImpl::init() {

    int number_of_modules = mod->acdc.get_number_of_modules();
    caps.bidirectional = false;

    caps.current_regulation_tolerance_A = 1;
    caps.peak_current_ripple_A = 0.5;

    caps.min_export_current_A = 0;
    caps.max_export_current_A = number_of_modules * mod->config.max_export_current_A;
    caps.min_export_voltage_V = 150;
    caps.max_export_voltage_V = 1000;
    caps.max_export_power_W = number_of_modules * mod->config.max_export_power_W;

    caps.max_import_current_A = 25;
    caps.min_import_current_A = 0;
    caps.max_import_power_W = 10000;
    caps.min_import_voltage_V = 200;
    caps.max_import_voltage_V = 1000;

    mod->acdc.signal_voltage_current.connect([this](float voltage, float current) {
        static uint8_t throttle_cnt = 0;
        if (throttle_cnt++ % 10 == 0) {
            types::power_supply_DC::VoltageCurrent vc;
            vc.current_A = current;
            vc.voltage_V = voltage;
            publish_voltage_current(vc);
        }
    });

    mod->acdc.signal_serial_number.connect([this](int module_id, const std::string& serial_number) {
        EVLOG_info << "Module ID " << module_id << ": " << serial_number;
    });
}

void power_supply_DCImpl::ready() {
    publish_capabilities(caps);
    mod->acdc.switch_on(false);
}

void power_supply_DCImpl::handle_setMode(types::power_supply_DC::Mode& mode,
                                         types::power_supply_DC::ChargingPhase& phase) {
    std::scoped_lock lock(settings_mutex);

    if (mode == types::power_supply_DC::Mode::Off) {
        mod->acdc.switch_on(false);
    } else if (mode == types::power_supply_DC::Mode::Export) {
        mod->acdc.switch_on(true);
    } else if (mode == types::power_supply_DC::Mode::Import) {
        mod->acdc.switch_on(false);
    } else if (mode == types::power_supply_DC::Mode::Fault) {
        mod->acdc.switch_on(false);
    }
};

void power_supply_DCImpl::handle_setExportVoltageCurrent(double& voltage, double& current) {

    if (voltage > caps.max_export_voltage_V)
        voltage = caps.max_export_voltage_V;
    else if (voltage < caps.min_export_voltage_V)
        voltage = caps.min_export_voltage_V;

    if (current > caps.max_export_current_A)
        current = caps.max_export_current_A;
    else if (current < caps.min_export_current_A)
        current = caps.min_export_current_A;

    std::scoped_lock lock(settings_mutex);

    export_voltage = voltage;
    export_current_limit = current;

    EVLOG_info << "Updating voltage/current via CAN: " << export_voltage << "V / " << export_current_limit << "A";
    mod->acdc.set_voltage_current(export_voltage, export_current_limit);
};

void power_supply_DCImpl::handle_setImportVoltageCurrent(double& voltage, double& current) {
    EVLOG_error << "Not implemented";
}

} // namespace main
} // namespace module
