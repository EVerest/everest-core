// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2023 chargebyte GmbH
// Copyright (C) 2023 Contributors to EVerest

#include <chrono>
#include <mutex>

#include "power_supply_DCImpl.hpp"

namespace module {
namespace main {

void power_supply_DCImpl::init() {
    this->connector_voltage = 0.0;
    this->connector_current = 0.0;

    this->power_supply_thread_handle = std::thread(&power_supply_DCImpl::power_supply_worker, this);
}

void power_supply_DCImpl::ready() {
}

types::power_supply_DC::Capabilities power_supply_DCImpl::handle_getCapabilities() {
    types::power_supply_DC::Capabilities Capabilities = {
        .bidirectional = this->config.bidirectional,
        .current_regulation_tolerance_A = 2.0,
        .peak_current_ripple_A = 2.0,
        .max_export_voltage_V = static_cast<float>(this->config.max_voltage),
        .min_export_voltage_V = static_cast<float>(this->config.min_voltage),
        .max_export_current_A = static_cast<float>(this->config.max_current),
        .min_export_current_A = static_cast<float>(this->config.min_current),
        .max_export_power_W = static_cast<float>(this->config.max_power),
        .max_import_voltage_V = static_cast<float>(this->config.max_voltage),
        .min_import_voltage_V = static_cast<float>(this->config.min_voltage),
        .max_import_current_A = static_cast<float>(this->config.max_current),
        .min_import_current_A = static_cast<float>(this->config.min_current),
        .max_import_power_W = static_cast<float>(this->config.max_power),
        .conversion_efficiency_import = 0.85,
        .conversion_efficiency_export = 0.9,
    };
    return Capabilities;
}

void power_supply_DCImpl::handle_setMode(types::power_supply_DC::Mode& value) {
    this->mode = value;

    std::scoped_lock access_lock(this->power_supply_values_mutex);
    if ((value == types::power_supply_DC::Mode::Off) || (value == types::power_supply_DC::Mode::Fault)) {
        this->connector_voltage = 0.0;
        this->connector_current = 0.0;
    } else if (value == types::power_supply_DC::Mode::Export) {
        this->connector_voltage = this->settings_connector_export_voltage;
        this->connector_current = this->settings_connector_max_export_current;
    } else if (value == types::power_supply_DC::Mode::Import) {
        this->connector_voltage = this->settings_connector_import_voltage;
        this->connector_current = this->settings_connector_max_import_current;
    }

    mod->p_main->publish_mode(value);
}

void power_supply_DCImpl::clampVoltageCurrent(double& voltage, double& current) {
    voltage = voltage < this->config.min_voltage   ? this->config.min_voltage
              : voltage > this->config.max_voltage ? this->config.max_voltage
                                                   : voltage;

    current = current < this->config.min_current   ? this->config.min_current
              : current > this->config.max_current ? this->config.max_current
                                                   : current;
}

void power_supply_DCImpl::handle_setExportVoltageCurrent(double& voltage, double& current) {
    double temp_voltage = voltage;
    double temp_current = current;

    clampVoltageCurrent(temp_voltage, temp_current);

    std::scoped_lock access_lock(this->power_supply_values_mutex);
    this->settings_connector_export_voltage = temp_voltage;
    this->settings_connector_max_export_current = temp_current;

    if (this->mode == types::power_supply_DC::Mode::Export) {
        this->connector_voltage = this->settings_connector_export_voltage;
        this->connector_current = this->settings_connector_max_export_current;
    }
}

void power_supply_DCImpl::handle_setImportVoltageCurrent(double& voltage, double& current) {
    double temp_voltage = voltage;
    double temp_current = current;

    clampVoltageCurrent(temp_voltage, temp_current);

    std::scoped_lock access_lock(this->power_supply_values_mutex);
    this->settings_connector_import_voltage = temp_voltage;
    this->settings_connector_max_import_current = temp_current;

    if (this->mode == types::power_supply_DC::Mode::Import) {
        this->connector_voltage = this->settings_connector_import_voltage;
        this->connector_current = -this->settings_connector_max_import_current;
    }
}

void power_supply_DCImpl::power_supply_worker(void) {
    types::power_supply_DC::VoltageCurrent voltage_current;

    while (true) {
        if (this->power_supply_thread_handle.shouldExit()) {
            break;
        }

        // set interval for publishing
        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_SLEEP_MS));

        std::scoped_lock access_lock(this->power_supply_values_mutex);
        voltage_current.voltage_V = static_cast<float>(this->connector_voltage);
        voltage_current.current_A = static_cast<float>(this->connector_current);

        this->mod->p_main->publish_voltage_current(voltage_current);
    }
}
} // namespace main
} // namespace module
