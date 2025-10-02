// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH
// Copyright Pionix GmbH and Contributors to EVerest

#include <chrono>
#include <mutex>

#include "power_supply_DCImpl.hpp"

namespace module {
namespace main {

void power_supply_DCImpl::init() {
    connector_voltage = 0.0;
    connector_current = 0.0;
    energy_import_total = 0.0;
    energy_export_total = 0.0;

    power_supply_thread_handle = std::thread(&power_supply_DCImpl::power_supply_worker, this);
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
    publish_capabilities(get_capabilities_from_config(config));
}

void power_supply_DCImpl::handle_setMode(types::power_supply_DC::Mode& _mode,
                                         types::power_supply_DC::ChargingPhase& phase) {
    mode = _mode;

    EVLOG_info << "Set mode: " << types::power_supply_DC::mode_to_string(mode)
               << " ChargingPhase: " << types::power_supply_DC::charging_phase_to_string(phase);

    std::scoped_lock access_lock(power_supply_values_mutex);
    if ((mode == types::power_supply_DC::Mode::Off) || (mode == types::power_supply_DC::Mode::Fault)) {
        connector_voltage = 0.0;
        connector_current = 0.0;
    } else if (mode == types::power_supply_DC::Mode::Export) {
        connector_voltage = settings_connector_export_voltage;
        connector_current = settings_connector_max_export_current;
    } else if (mode == types::power_supply_DC::Mode::Import) {
        connector_voltage = settings_connector_import_voltage;
        connector_current = settings_connector_max_import_current;
    }

    mod->p_main->publish_mode(mode);
}

void power_supply_DCImpl::clampVoltageCurrent(double& voltage, double& current) {
    voltage = voltage < config.min_voltage   ? config.min_voltage
              : voltage > config.max_voltage ? config.max_voltage
                                             : voltage;

    current = current < config.min_current   ? config.min_current
              : current > config.max_current ? config.max_current
                                             : current;
}

void power_supply_DCImpl::handle_setExportVoltageCurrent(double& voltage, double& current) {
    double temp_voltage = voltage;
    double temp_current = current;

    clampVoltageCurrent(temp_voltage, temp_current);

    std::scoped_lock access_lock(power_supply_values_mutex);
    settings_connector_export_voltage = temp_voltage;
    settings_connector_max_export_current = temp_current;

    if (mode == types::power_supply_DC::Mode::Export) {
        connector_voltage = settings_connector_export_voltage;
        connector_current = settings_connector_max_export_current;
    }
}

void power_supply_DCImpl::handle_setImportVoltageCurrent(double& voltage, double& current) {
    double temp_voltage = voltage;
    double temp_current = current;

    clampVoltageCurrent(temp_voltage, temp_current);

    std::scoped_lock access_lock(power_supply_values_mutex);
    settings_connector_import_voltage = temp_voltage;
    settings_connector_max_import_current = temp_current;

    if (mode == types::power_supply_DC::Mode::Import) {
        connector_voltage = settings_connector_import_voltage;
        connector_current = -settings_connector_max_import_current;
    }
}

types::powermeter::Powermeter power_supply_DCImpl::power_meter_external() {
    types::powermeter::Powermeter powermeter;

    powermeter.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    powermeter.meter_id = "DC_POWERMETER";

    if (connector_current > 0) {
        energy_import_total += (connector_voltage * connector_current * LOOP_SLEEP_MS / 1000) / 3600;
    }
    if (connector_current < 0) {
        energy_export_total += (connector_voltage * -connector_current * LOOP_SLEEP_MS / 1000) / 3600;
    }

    powermeter.energy_Wh_import = {static_cast<float>(energy_import_total)};
    powermeter.energy_Wh_export = {static_cast<float>(energy_export_total)};

    powermeter.power_W = {static_cast<float>(connector_current * connector_voltage)};
    powermeter.current_A = {static_cast<float>(connector_current)};
    powermeter.voltage_V = {static_cast<float>(connector_voltage)};

    return powermeter;
}

void power_supply_DCImpl::power_supply_worker(void) {
    types::power_supply_DC::VoltageCurrent voltage_current;

    while (true) {
        if (power_supply_thread_handle.shouldExit()) {
            break;
        }

        // set interval for publishing
        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_SLEEP_MS));

        std::scoped_lock access_lock(power_supply_values_mutex);
        voltage_current.voltage_V = static_cast<float>(connector_voltage);
        voltage_current.current_A = static_cast<float>(connector_current);

        mod->p_main->publish_voltage_current(voltage_current);
        mod->p_powermeter->publish_powermeter(power_meter_external());
    }
}
} // namespace main
} // namespace module
