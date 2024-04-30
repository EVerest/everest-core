// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "power_supply_DCImpl.hpp"
#include <fmt/core.h>

namespace module {
namespace dc_supply {

void power_supply_DCImpl::init() {

    mod->serial.signalPowerMeter.connect([this](const PowerMeter& p) {
        types::power_supply_DC::VoltageCurrent vc;
        vc.voltage_V = p.voltage;
        vc.current_A = 0.;
        publish_voltage_current(vc);
    });
}

void power_supply_DCImpl::ready() {
    types::power_supply_DC::Capabilities caps;
    caps.bidirectional = false;
    caps.conversion_efficiency_export = 0.9;
    caps.max_export_current_A = 25;
    caps.max_export_voltage_V = mod->config.dc_max_voltage;
    caps.min_export_current_A = 0;
    caps.min_export_voltage_V = 50;
    caps.max_export_power_W = 10000;
    caps.current_regulation_tolerance_A = 1;
    caps.peak_current_ripple_A = 0;

    publish_capabilities(caps);
}

void power_supply_DCImpl::handle_setMode(types::power_supply_DC::Mode& value) {
    // your code for cmd setMode goes here
    if (value == types::power_supply_DC::Mode::Export) {
        mod->serial.setOutputVoltageCurrent(req_voltage, req_current);
        is_on = true;
    } else {
        mod->serial.setOutputVoltageCurrent(0, 0);
        is_on = false;
    }
};

void power_supply_DCImpl::handle_setExportVoltageCurrent(double& voltage, double& current) {
    req_voltage = voltage;
    req_current = current;

    if (is_on) {
        mod->serial.setOutputVoltageCurrent(req_voltage, req_current);
    }
};

void power_supply_DCImpl::handle_setImportVoltageCurrent(double& voltage, double& current){
    // not supported here
};

} // namespace dc_supply
} // namespace module
