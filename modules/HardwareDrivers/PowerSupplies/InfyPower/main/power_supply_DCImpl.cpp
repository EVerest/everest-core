// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "power_supply_DCImpl.hpp"
#include <iomanip>
#include <sstream>

namespace module {
namespace main {

void power_supply_DCImpl::init() {
    constexpr int telemetry_update_interval = 5;
    mod->acdc->initial_ping();
    mod->acdc->signalVoltageCurrent.connect([this](InfyCanDevice::TelemetryMap telemetries) {
        if (throttle_cnt++ % telemetry_update_interval == 0) {
            float total_current = 0;
            float ext_voltage = 0;
            for (const auto& telemetry : telemetries) {
                total_current += telemetry.second.current;
                ext_voltage = telemetry.second.v_ext;
            }

            types::power_supply_DC::VoltageCurrent vc;
            vc.current_A = total_current;
            vc.voltage_V = ext_voltage;
            publish_voltage_current(vc);
        }
    });

    mod->acdc->signalModuleStatus.connect([this](can_packet_acdc::PowerModuleStatus status) {
        // Publish mode changes
        types::power_supply_DC::Mode mode;

        if (status.fault_alarm) {
            mode = types::power_supply_DC::Mode::Fault;
        } else if (status.dc_side_off) {
            mode = types::power_supply_DC::Mode::Off;
        } else {
            mode = types::power_supply_DC::Mode::Export;
        }

        if (this->mode.load() != mode || firsttime) {
            publish_mode(mode);
            firsttime = false;
        }
    });

    mod->acdc->signalCapabilitiesUpdate.connect([this](InfyCanDevice::TelemetryMap telemetries) {
        types::power_supply_DC::Capabilities new_caps;
        new_caps.bidirectional = false;
        // since the power supply is not bidirectional, we set the import capabilities to 0
        new_caps.min_import_voltage_V = 0;
        new_caps.max_import_voltage_V = 0;
        new_caps.min_import_current_A = 0;
        new_caps.max_import_current_A = 0;
        // not working correctly if set to 0, the device will not allow setting the current to 0
        new_caps.min_export_current_A = 1;
        if (telemetries.size() == 0) {
            EVLOG_info << "Infy: No telemetries received, setting default capabilities";
            publish_capabilities(new_caps);
            return;
        }
        float min_max_output_voltage = std::numeric_limits<float>::max();
        float max_min_output_voltage = std::numeric_limits<float>::min();
        for (const auto& telemetry : telemetries) {
            // If the telemetry has a value for dc_max_output_voltage, we can use the values to update the capabilities
            if (telemetry.second.valid_caps) {
                if (telemetry.second.dc_max_output_voltage < min_max_output_voltage) {
                    min_max_output_voltage = telemetry.second.dc_max_output_voltage;
                }
                if (telemetry.second.dc_min_output_voltage > max_min_output_voltage) {
                    max_min_output_voltage = telemetry.second.dc_min_output_voltage;
                }
                new_caps.max_export_current_A += telemetry.second.dc_max_output_current;
                new_caps.max_export_power_W += telemetry.second.dc_rated_output_power;
            }
        }
        new_caps.max_export_voltage_V = min_max_output_voltage;
        new_caps.min_export_voltage_V = max_min_output_voltage;
        new_caps.conversion_efficiency_export = mod->config.conversion_efficiency_export;
        caps = new_caps;
        EVLOG_info << "Infy: Capabilities updated: " << new_caps.max_export_voltage_V << "V / "
                   << new_caps.min_export_voltage_V << "V, " << new_caps.max_export_current_A << "A, power "
                   << new_caps.max_export_power_W << "W";
        publish_capabilities(new_caps);
        if (last_module_count != telemetries.size() && telemetries.size() > 0) {
            double voltage = exportVoltage.load();
            double current = exportCurrentLimit.load();
            types::power_supply_DC::Mode mode = this->mode.load();
            types::power_supply_DC::ChargingPhase phase = this->phase.load();

            if (telemetries.size() > last_module_count) {
                EVLOG_info << "Infy: Hot plug detected - module count increased from "
                           << static_cast<int>(last_module_count) << " to " << telemetries.size()
                           << " modules, redistributing " << current << "A from " << static_cast<int>(last_module_count)
                           << " to " << telemetries.size() << " modules";
            } else if (telemetries.size() < last_module_count) {
                EVLOG_info << "Infy: Hot unplug detected - module count decreased from "
                           << static_cast<int>(last_module_count) << " to " << telemetries.size()
                           << " modules, redistributing " << current << "A from " << static_cast<int>(last_module_count)
                           << " to " << telemetries.size() << " modules";
            }
            EVLOG_info << "Infy: Restoring last settings: voltage=" << voltage << "V, current=" << current
                       << "A, mode=" << mode << ", phase=" << phase;
            last_module_count = telemetries.size();
            handle_setExportVoltageCurrent(voltage, current);
            handle_setMode(mode, phase);
        }
        last_module_count = telemetries.size();
    });

    mod->acdc->signalError.connect([this](uint8_t address, InfyCanDevice::Error error, bool active) {
        const std::string error_type = map_infy_error_to_power_supply_dc(error);
        const std::string error_message = create_error_message(address, error, active);
        const bool is_error_active = error_state_monitor->is_error_active(error_type, "");

        if (error == InfyCanDevice::Error::CommunicationFault && active) {
            EVLOG_info << "Infy: Communication fault detected - all " << static_cast<int>(last_module_count)
                       << " modules unresponsive, forcing system OFF for safety";
            this->mode.store(types::power_supply_DC::Mode::Off);
        }

        if (active && !is_error_active) {
            // New error detected - raise it
            EVLOG_error << error_message;
            auto severity = (error == InfyCanDevice::Error::FanFault) ? Everest::error::Severity::Medium
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
    if (mode == types::power_supply_DC::Mode::Off) {
        mod->acdc->switch_on_off(false);
    } else if (mode == types::power_supply_DC::Mode::Export) {
        mod->acdc->switch_on_off(true);
    } else if (mode == types::power_supply_DC::Mode::Import) {
        mod->acdc->switch_on_off(true);
    } else if (mode == types::power_supply_DC::Mode::Fault) {
        mod->acdc->switch_on_off(false);
    }
    this->mode.store(mode);
    this->phase.store(phase);
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

    exportVoltage.store(voltage);
    exportCurrentLimit.store(current);

    const size_t active_module_count = last_module_count;
    if (active_module_count > 0) {
        const double current_per_module = exportCurrentLimit.load() / static_cast<double>(active_module_count);
        EVLOG_info << "Infy: Updating voltage/current via CAN: " << exportVoltage.load() << "V / "
                   << exportCurrentLimit.load() << "A total → " << current_per_module
                   << "A per module - active modules: " << active_module_count;
    } else {
        EVLOG_info << "Infy: Updating voltage/current via CAN: " << exportVoltage.load() << "V / "
                   << exportCurrentLimit.load() << "A (but no active modules detected)";
    }
    mod->acdc->set_voltage_current(exportVoltage.load(), exportCurrentLimit.load());
};

void power_supply_DCImpl::handle_setImportVoltageCurrent(double& voltage, double& current) {
    if (caps.min_import_voltage_V.has_value() && caps.max_import_voltage_V.has_value() &&
        caps.min_import_current_A.has_value() && caps.max_import_current_A.has_value()) {
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

        EVLOG_info << "Infy: Updating voltage/current via CAN: " << minImportVoltage.load() << "V / "
                   << importCurrentLimit.load() << "A";
        mod->acdc->set_voltage_current(minImportVoltage.load(), importCurrentLimit.load());
    }
}

std::string power_supply_DCImpl::map_infy_error_to_power_supply_dc(InfyCanDevice::Error error) {
    switch (error) {
    case InfyCanDevice::Error::OverVoltage:
        return "power_supply_DC/OverVoltageDC";
    case InfyCanDevice::Error::UnderVoltage:
        return "power_supply_DC/UnderVoltageDC";
    case InfyCanDevice::Error::OverTemperature:
        return "power_supply_DC/OverTemperature";
    case InfyCanDevice::Error::OverCurrent:
        return "power_supply_DC/OverCurrentDC";
    case InfyCanDevice::Error::InternalFault:
        return "power_supply_DC/HardwareFault";
    case InfyCanDevice::Error::CommunicationFault:
        return "power_supply_DC/CommunicationFault";
    case InfyCanDevice::Error::InputVoltage:
        return "power_supply_DC/UnderVoltageAC"; // Most common case for input voltage issues
    case InfyCanDevice::Error::FanFault:
        return "power_supply_DC/VendorWarning"; // Non-critical vendor-specific warning
    case InfyCanDevice::Error::InputPhaseLoss:
        return "power_supply_DC/VendorError"; // Critical vendor-specific error
    default:
        return "power_supply_DC/VendorError"; // Fallback for unknown errors
    }
}

std::string power_supply_DCImpl::create_error_message(uint8_t module_address, InfyCanDevice::Error error,
                                                      bool active) const {
    std::string action = active ? "detected" : "cleared";
    std::string error_name;

    switch (error) {
    case InfyCanDevice::Error::OverVoltage:
        error_name = "overvoltage fault";
        break;
    case InfyCanDevice::Error::UnderVoltage:
        error_name = "undervoltage fault";
        break;
    case InfyCanDevice::Error::OverTemperature:
        error_name = "overtemperature fault";
        break;
    case InfyCanDevice::Error::OverCurrent:
        error_name = "overcurrent fault";
        break;
    case InfyCanDevice::Error::InternalFault:
        error_name = "internal fault";
        break;
    case InfyCanDevice::Error::CommunicationFault:
        error_name = "communication fault";
        break;
    case InfyCanDevice::Error::InputVoltage:
        error_name = "input voltage fault";
        break;
    case InfyCanDevice::Error::FanFault:
        error_name = "fan fault";
        break;
    case InfyCanDevice::Error::InputPhaseLoss:
        error_name = "input phase loss fault";
        break;
    default:
        error_name = "unknown fault";
        break;
    }

    std::stringstream ss;
    ss << "Infy[0x" << std::hex << std::uppercase << static_cast<int>(module_address) << "]: " << error_name << " "
       << action;
    return ss.str();
}

} // namespace main
} // namespace module
