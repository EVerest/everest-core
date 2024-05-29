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

static auto get_capabilities_from_config(const Conf& config) {
    types::power_supply_DC::Capabilities cap;

    cap.bidirectional = config.bidirectional;
    cap.current_regulation_tolerance_A = 2.0f;
    cap.peak_current_ripple_A = 2.0f;
    cap.max_export_voltage_V = static_cast<float>(config.max_voltage);
    cap.min_export_voltage_V = static_cast<float>(config.min_voltage);
    cap.max_export_current_A = static_cast<float>(config.max_current);
    cap.min_export_current_A = static_cast<float>(config.min_current);
    cap.max_export_power_W = static_cast<float>(config.max_power);
    cap.max_import_voltage_V = static_cast<float>(config.max_voltage);
    cap.min_import_voltage_V = static_cast<float>(config.min_voltage);
    cap.max_import_current_A = static_cast<float>(config.max_current);
    cap.min_import_current_A = static_cast<float>(config.min_current);
    cap.max_import_power_W = static_cast<float>(config.max_power);
    cap.conversion_efficiency_import = 0.85f;
    cap.conversion_efficiency_export = 0.9f;

    return cap;
}

void power_supply_DCImpl::ready() {
    publish_capabilities(get_capabilities_from_config(this->config));
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
