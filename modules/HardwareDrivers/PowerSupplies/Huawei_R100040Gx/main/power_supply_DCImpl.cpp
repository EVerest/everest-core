// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "power_supply_DCImpl.hpp"

namespace module {
namespace main {

void power_supply_DCImpl::init() {

    caps.bidirectional = false;

    caps.current_regulation_tolerance_A = 1;
    caps.peak_current_ripple_A = 0.5;

    caps.min_export_current_A = 0;
    caps.max_export_current_A = mod->config.max_export_current_A;
    caps.min_export_voltage_V = 150;
    caps.max_export_voltage_V = 1000;
    caps.max_export_power_W = mod->config.max_export_power_W;

    caps.max_import_current_A = 0;
    caps.min_import_current_A = 0;
    caps.max_import_power_W = 0;
    caps.min_import_voltage_V = 0;
    caps.max_import_voltage_V = 0;

    mod->acdc.signal_capabilities.connect([this](const types::power_supply_DC::Capabilities& c) {
        caps = c;
        // limit by config values
        if (caps.max_export_current_A > mod->config.max_export_current_A) {
            caps.max_export_current_A = mod->config.max_export_current_A;
        }
        if (caps.max_export_power_W > mod->config.max_export_power_W) {
            caps.max_export_power_W = mod->config.max_export_power_W;
        }
        publish_capabilities(caps);
    });

    mod->acdc.signal_voltage_current.connect([this](float voltage, float current) {
        types::power_supply_DC::VoltageCurrent vc;
        vc.current_A = current;
        vc.voltage_V = voltage;
        publish_voltage_current(vc);
    });

    mod->acdc.signal_serial_number.connect([this](int module_id, const std::string& serial_number) {
        EVLOG_info << "Module ID " << module_id << ": " << serial_number;
    });

    mod->acdc.signal_on_off.connect([this](bool on) {
        if (on) {
            publish_mode(types::power_supply_DC::Mode::Import);
        } else {
            publish_mode(types::power_supply_DC::Mode::Off);
        }
    });

    mod->acdc.signal_communication_error.connect([this](bool err, const std::string& desc) {
        if (err) {
            Everest::error::Error error_object = error_factory->create_error("power_supply_DC/CommunicationFault", "",
                                                                             desc, Everest::error::Severity::High);
            raise_error(error_object);
            comm_fault_error_raised = true;
        } else {
            if (comm_fault_error_raised) {
                clear_error("power_supply_DC/CommunicationFault");
                comm_fault_error_raised = false;
            }
        }
    });
}

void power_supply_DCImpl::ready() {
    mod->p_main->publish_capabilities(caps);
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
