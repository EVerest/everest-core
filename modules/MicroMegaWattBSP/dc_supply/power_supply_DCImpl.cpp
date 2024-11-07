// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "power_supply_DCImpl.hpp"
#include <fmt/core.h>

namespace module {
namespace dc_supply {

void power_supply_DCImpl::init() {
    mod->serial.signalTelemetry.connect([this](Telemetry t) {
        types::power_supply_DC::VoltageCurrent vc;
        vc.current_A = 0;
        vc.voltage_V = t.voltage;
        publish_voltage_current(vc);

        types::powermeter::Powermeter p;
        p.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
        p.meter_id = "UMWC";
        types::units::Energy e;
        e.total = 0.;
        p.energy_Wh_import = e;
        types::units::Voltage v;
        v.DC = t.voltage;
        p.voltage_V = v;
        mod->p_powermeter->publish_powermeter(p);
    });

    std::thread([this]() {
        float low_pass_voltage = 0.;

        float last_low_pass_voltage = -1;

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // prevent overshoot
            if (low_pass_voltage > req_voltage) {
                // step down immediately
                low_pass_voltage = req_voltage;
            } else {
                float delta = req_voltage - low_pass_voltage;
                if (delta > 500) {
                    low_pass_voltage += 100;
                } else {
                    if (delta > 50) {
                        low_pass_voltage += 25;
                    } else {
                        low_pass_voltage = req_voltage;
                    }
                }
            }

            if (not is_on) {
                low_pass_voltage = 0.;
            }

            if (last_low_pass_voltage not_eq low_pass_voltage) {
                mod->serial.setOutputVoltageCurrent(low_pass_voltage, 0.);
            }

            last_low_pass_voltage = low_pass_voltage;
        }
    }).detach();
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

void power_supply_DCImpl::handle_setMode(types::power_supply_DC::Mode& mode,
                                         types::power_supply_DC::ChargingPhase& phase) {
    // your code for cmd setMode goes here
    if (mode == types::power_supply_DC::Mode::Export) {
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
};

void power_supply_DCImpl::handle_setImportVoltageCurrent(double& voltage, double& current){
    // not supported here
};

} // namespace dc_supply
} // namespace module
