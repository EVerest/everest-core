/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "power_supply_DCImpl.hpp"
#include <iomanip>
#include <sstream>

namespace module {
namespace main {

void power_supply_DCImpl::init() {
    constexpr int telemetry_update_interval = 5;
    mod->acdc->initial_ping();
    mod->acdc->signalVoltageCurrent.connect([this](WinlineCanDevice::TelemetryMap telemetries) {
        if (throttle_cnt++ % telemetry_update_interval == 0) {
            float total_current = 0;
            float module_voltage = 0;

            for (const auto& telemetry : telemetries) {
                total_current += telemetry.second.current;
                // Use the proper Winline voltage reading from register 0x0001
                module_voltage = telemetry.second.voltage; // Changed from v_ext to voltage
            }

            types::power_supply_DC::VoltageCurrent vc;
            vc.current_A = total_current;
            vc.voltage_V = module_voltage;
            publish_voltage_current(vc);
        }
    });

    mod->acdc->signalModuleStatus.connect([this](can_packet_acdc::PowerModuleStatus status) {
        // Publish mode changes
        types::power_supply_DC::Mode mode;

        if (status.module_fault) {
            mode = types::power_supply_DC::Mode::Fault;
        } else if (status.dcdc_on_off_status) {
            mode = types::power_supply_DC::Mode::Off;
        } else {
            mode = types::power_supply_DC::Mode::Export;
        }

        if (this->mode.load() != mode || firsttime) {
            publish_mode(mode);
            firsttime = false;
        }
    });

    mod->acdc->signalCapabilitiesUpdate.connect([this](WinlineCanDevice::TelemetryMap telemetries) {
        types::power_supply_DC::Capabilities new_caps;
        new_caps.bidirectional = false;
        new_caps.min_export_current_A = 1;
        if (telemetries.size() == 0) {
            EVLOG_info << "Winline: No telemetries received, setting default capabilities";
            publish_capabilities(new_caps);
            return;
        }
        // Start with config limits as base
        new_caps.min_export_voltage_V = mod->config.min_export_voltage_V;
        new_caps.max_export_voltage_V = mod->config.max_export_voltage_V;
        new_caps.min_export_current_A = mod->config.min_export_current_A;
        new_caps.max_export_current_A = 0.0;  // Will be updated with device rated values

        // Update with device rated values (current and power from protocol)
        for (const auto& telemetry : telemetries) {
            if (telemetry.second.valid_caps) {
                // Replace max current with device rated current (sum of all modules)
                new_caps.max_export_current_A += telemetry.second.dc_max_output_current;
                // Sum up total power from all modules
                new_caps.max_export_power_W += telemetry.second.dc_rated_output_power;
            }
        }
        new_caps.conversion_efficiency_export = mod->config.conversion_efficiency_export;
        caps = new_caps;
        EVLOG_info << "Winline: Capabilities updated: " << new_caps.max_export_voltage_V << "V / "
                   << new_caps.min_export_voltage_V << "V, " << new_caps.max_export_current_A << "A, power "
                   << new_caps.max_export_power_W << "W";
        publish_capabilities(new_caps);
        if (last_module_count != telemetries.size() && telemetries.size() > 0) {
            double voltage = exportVoltage.load();
            double current = exportCurrentLimit.load();
            types::power_supply_DC::Mode mode = this->mode.load();
            types::power_supply_DC::ChargingPhase phase = this->phase.load();

            if (telemetries.size() > last_module_count) {
                EVLOG_info << "Winline: Hot plug detected - module count increased from "
                           << static_cast<int>(last_module_count) << " to " << telemetries.size()
                           << " modules, redistributing " << current << "A from " << static_cast<int>(last_module_count)
                           << " to " << telemetries.size() << " modules";
            } else if (telemetries.size() < last_module_count) {
                EVLOG_info << "Winline: Hot unplug detected - module count decreased from "
                           << static_cast<int>(last_module_count) << " to " << telemetries.size()
                           << " modules, redistributing " << current << "A from " << static_cast<int>(last_module_count)
                           << " to " << telemetries.size() << " modules";
            }
            EVLOG_info << "Winline: Restoring last settings: voltage=" << voltage << "V, current=" << current
                       << "A, mode=" << mode << ", phase=" << phase;
            last_module_count = telemetries.size();
            handle_setExportVoltageCurrent(voltage, current);
            handle_setMode(mode, phase);
        }
        last_module_count = telemetries.size();
    });

    mod->acdc->signalError.connect([this](uint8_t address, WinlineCanDevice::Error error, bool active) {
        const std::string error_type = map_winline_error_to_power_supply_dc(error);
        const std::string error_message = create_error_message(address, error, active);
        const bool is_error_active = error_state_monitor->is_error_active(error_type, "");

        if (error == WinlineCanDevice::Error::CommunicationFault && active) {
            EVLOG_info << "Winline: Communication fault detected - all " << static_cast<int>(last_module_count)
                       << " modules unresponsive, forcing system OFF for safety";
            this->mode.store(types::power_supply_DC::Mode::Off);
        }

        if (active && !is_error_active) {
            // New error detected - raise it
            EVLOG_error << error_message;
            auto severity = (error == WinlineCanDevice::Error::FanFault) ? Everest::error::Severity::Medium
                                                                         : Everest::error::Severity::High;
            raise_error(error_factory->create_error(error_type, "", error_message, severity));
        } else if (!active && is_error_active) {
            // Error cleared - clear it
            EVLOG_info << error_message;
            clear_error(error_type);
        }
    });
}

void power_supply_DCImpl::ready() {
}

void power_supply_DCImpl::handle_setMode(types::power_supply_DC::Mode& mode,
                                         types::power_supply_DC::ChargingPhase& phase) {
    EVLOG_info << "Set mode via CAN: " << mode << " with phase " << phase;

    // Enhanced power control with verification (Task 12)
    bool power_result = false;
    if (mode == types::power_supply_DC::Mode::Off) {
        power_result = mod->acdc->handle_power_transition(false);
    } else if (mode == types::power_supply_DC::Mode::Export) {
        power_result = mod->acdc->handle_power_transition(true);
    } else if (mode == types::power_supply_DC::Mode::Import) {
        power_result = mod->acdc->handle_power_transition(true);
    } else if (mode == types::power_supply_DC::Mode::Fault) {
        power_result = mod->acdc->handle_power_transition(false);
    }

    if (power_result) {
        EVLOG_info << "Winline: Mode change to " << mode << " initiated successfully";
        this->mode.store(mode);
        this->phase.store(phase);
    } else {
        EVLOG_error << "Winline: Mode change to " << mode << " failed - keeping current mode";
        // Don't update stored mode/phase on failure
    }
};

void power_supply_DCImpl::handle_setExportVoltageCurrent(double& voltage, double& current) {
    EVLOG_info << "Winline: request setting voltage/current: " << voltage << "V / " << current << "A";
    if (voltage > caps.max_export_voltage_V)
        voltage = caps.max_export_voltage_V;
    else if (voltage < caps.min_export_voltage_V)
        voltage = caps.min_export_voltage_V;

    if (current > caps.max_export_current_A)
        current = caps.max_export_current_A;
    else if (current < caps.min_export_current_A)
        current = caps.min_export_current_A;

    // Validate power limits: voltage * current must not exceed max power
    double requested_power = voltage * current;
    if (requested_power > caps.max_export_power_W) {
        EVLOG_warning << "Winline: Requested power " << requested_power << "W exceeds max power "
                      << caps.max_export_power_W << "W. Reducing current to stay within power limit.";
        // Reduce current to stay within power limit
        current = caps.max_export_power_W / voltage;
        if (current < caps.min_export_current_A) {
            EVLOG_error << "Winline: Cannot reduce current below minimum " << caps.min_export_current_A
                        << "A while staying within power limit. Setting to minimum.";
            current = caps.min_export_current_A;
        }
    }

    EVLOG_info << "Winline: request setting voltage/current: " << voltage << "V / " << current << "A (power: " << voltage * current << "W)";
    exportVoltage.store(voltage);
    exportCurrentLimit.store(current);

    const size_t active_module_count = last_module_count;
    if (active_module_count > 0) {
        const double current_per_module = exportCurrentLimit.load() / static_cast<double>(active_module_count);
        EVLOG_info << "Winline: Updating voltage/current via CAN: " << exportVoltage.load() << "V / "
                   << exportCurrentLimit.load() << "A total → " << current_per_module
                   << "A per module - active modules: " << active_module_count;
    } else {
        EVLOG_info << "Winline: Updating voltage/current via CAN: " << exportVoltage.load() << "V / "
                   << exportCurrentLimit.load() << "A (but no active modules detected)";
    }
    mod->acdc->set_voltage_current(exportVoltage.load(), exportCurrentLimit.load());
};

void power_supply_DCImpl::handle_setImportVoltageCurrent(double& voltage, double& current) {
    if (caps.min_import_voltage_V.has_value() && caps.max_import_current_A.has_value()) {
        if (voltage > caps.max_import_voltage_V.value())
            voltage = caps.max_import_voltage_V.value();
        else if (voltage < caps.min_import_voltage_V.value())
            voltage = caps.min_import_voltage_V.value();

        if (current > caps.max_import_current_A.value())
            current = caps.max_import_current_A.value();
        else if (current < caps.min_import_current_A.value())
            current = caps.min_import_current_A.value();

        minImportVoltage.store(voltage);
        importCurrentLimit.store(current);

        EVLOG_info << "Winline: Updating voltage/current via CAN: " << minImportVoltage.load() << "V / "
                   << importCurrentLimit.load() << "A";
        mod->acdc->set_voltage_current(minImportVoltage.load(), importCurrentLimit.load());
    }
}

std::string power_supply_DCImpl::map_winline_error_to_power_supply_dc(WinlineCanDevice::Error error) {
    switch (error) {
    case WinlineCanDevice::Error::OverVoltage:
        return "power_supply_DC/OverVoltageDC";
    case WinlineCanDevice::Error::UnderVoltage:
        return "power_supply_DC/UnderVoltageDC";
    case WinlineCanDevice::Error::OverTemperature:
        return "power_supply_DC/OverTemperature";
    case WinlineCanDevice::Error::OverCurrent:
        return "power_supply_DC/OverCurrentDC";
    case WinlineCanDevice::Error::InternalFault:
        return "power_supply_DC/HardwareFault";
    case WinlineCanDevice::Error::CommunicationFault:
        return "power_supply_DC/CommunicationFault";
    case WinlineCanDevice::Error::InputVoltage:
        return "power_supply_DC/UnderVoltageAC"; // Most common case for input voltage issues
    case WinlineCanDevice::Error::FanFault:
        return "power_supply_DC/VendorWarning"; // Non-critical vendor-specific warning
    case WinlineCanDevice::Error::InputPhaseLoss:
        return "power_supply_DC/VendorError"; // Critical vendor-specific error
    default:
        return "power_supply_DC/VendorError"; // Fallback for unknown errors
    }
}

std::string power_supply_DCImpl::create_error_message(uint8_t module_address, WinlineCanDevice::Error error,
                                                      bool active) const {
    std::string action = active ? "detected" : "cleared";
    std::string error_name;

    switch (error) {
    case WinlineCanDevice::Error::OverVoltage:
        error_name = "overvoltage fault";
        break;
    case WinlineCanDevice::Error::UnderVoltage:
        error_name = "undervoltage fault";
        break;
    case WinlineCanDevice::Error::OverTemperature:
        error_name = "overtemperature fault";
        break;
    case WinlineCanDevice::Error::OverCurrent:
        error_name = "overcurrent fault";
        break;
    case WinlineCanDevice::Error::InternalFault:
        error_name = "internal fault";
        break;
    case WinlineCanDevice::Error::CommunicationFault:
        error_name = "communication fault";
        break;
    case WinlineCanDevice::Error::InputVoltage:
        error_name = "input voltage fault";
        break;
    case WinlineCanDevice::Error::FanFault:
        error_name = "fan fault";
        break;
    case WinlineCanDevice::Error::InputPhaseLoss:
        error_name = "input phase loss fault";
        break;
    default:
        error_name = "unknown fault";
        break;
    }

    std::stringstream ss;
    ss << "Winline[0x" << std::hex << std::uppercase << static_cast<int>(module_address) << "]: " << error_name << " "
       << action;
    return ss.str();
}

} // namespace main
} // namespace module
