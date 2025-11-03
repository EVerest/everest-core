// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "limit_decimal_places.hpp"

#include <cmath>

#include <c4/format.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>

namespace module {

std::string LimitDecimalPlaces::limit(const types::powermeter::Powermeter& powermeter) {
    ryml::Tree tree;
    ryml::NodeRef root = tree.rootref();
    root |= ryml::MAP;

    // add informative power meter entries
    root["timestamp"] << powermeter.timestamp;

    if (powermeter.meter_id.has_value()) {
        root["meter_id"] << powermeter.meter_id.value();
    }

    if (powermeter.phase_seq_error.has_value()) {
        root["phase_seq_error"] << ryml::fmt::boolalpha(powermeter.phase_seq_error.value());
    }

    // limit decimal places

    // energy_Wh_import always exists
    root["energy_Wh_import"] |= ryml::MAP;
    root["energy_Wh_import"]["total"] << ryml::fmt::real(
        this->round_to_nearest_step(powermeter.energy_Wh_import.total, this->config.powermeter_energy_import_round_to),
        this->config.powermeter_energy_import_decimal_places);
    if (powermeter.energy_Wh_import.L1.has_value()) {
        root["energy_Wh_import"]["L1"] << ryml::fmt::real(
            this->round_to_nearest_step(powermeter.energy_Wh_import.L1.value(),
                                        this->config.powermeter_energy_import_round_to),
            this->config.powermeter_energy_import_decimal_places);
    }
    if (powermeter.energy_Wh_import.L2.has_value()) {
        root["energy_Wh_import"]["L2"] << ryml::fmt::real(
            this->round_to_nearest_step(powermeter.energy_Wh_import.L2.value(),
                                        this->config.powermeter_energy_import_round_to),
            this->config.powermeter_energy_import_decimal_places);
    }
    if (powermeter.energy_Wh_import.L3.has_value()) {
        root["energy_Wh_import"]["L3"] << ryml::fmt::real(
            this->round_to_nearest_step(powermeter.energy_Wh_import.L3.value(),
                                        this->config.powermeter_energy_import_round_to),
            this->config.powermeter_energy_import_decimal_places);
    }

    // everything else in the power meter is optional
    if (powermeter.energy_Wh_export.has_value()) {
        auto& energy_Wh_export = powermeter.energy_Wh_export.value();
        root["energy_Wh_export"] |= ryml::MAP;
        root["energy_Wh_export"]["total"] << ryml::fmt::real(
            this->round_to_nearest_step(energy_Wh_export.total, this->config.powermeter_energy_export_round_to),
            this->config.powermeter_energy_export_decimal_places);
        if (energy_Wh_export.L1.has_value()) {
            root["energy_Wh_export"]["L1"]
                << ryml::fmt::real(this->round_to_nearest_step(energy_Wh_export.L1.value(),
                                                               this->config.powermeter_energy_export_round_to),
                                   this->config.powermeter_energy_export_decimal_places);
        }
        if (energy_Wh_export.L2.has_value()) {
            root["energy_Wh_export"]["L2"]
                << ryml::fmt::real(this->round_to_nearest_step(energy_Wh_export.L2.value(),
                                                               this->config.powermeter_energy_export_round_to),
                                   this->config.powermeter_energy_export_decimal_places);
        }
        if (energy_Wh_export.L3.has_value()) {
            root["energy_Wh_export"]["L3"]
                << ryml::fmt::real(this->round_to_nearest_step(energy_Wh_export.L3.value(),
                                                               this->config.powermeter_energy_export_round_to),
                                   this->config.powermeter_energy_export_decimal_places);
        }
    }

    if (powermeter.power_W.has_value()) {
        auto& power_W = powermeter.power_W.value();
        root["power_W"] |= ryml::MAP;
        root["power_W"]["total"] << ryml::fmt::real(
            this->round_to_nearest_step(power_W.total, this->config.powermeter_power_round_to),
            this->config.powermeter_power_decimal_places);
        if (power_W.L1.has_value()) {
            root["power_W"]["L1"] << ryml::fmt::real(
                this->round_to_nearest_step(power_W.L1.value(), this->config.powermeter_power_round_to),
                this->config.powermeter_power_decimal_places);
        }
        if (power_W.L2.has_value()) {
            root["power_W"]["L2"] << ryml::fmt::real(
                this->round_to_nearest_step(power_W.L2.value(), this->config.powermeter_power_round_to),
                this->config.powermeter_power_decimal_places);
        }
        if (power_W.L3.has_value()) {
            root["power_W"]["L3"] << ryml::fmt::real(
                this->round_to_nearest_step(power_W.L3.value(), this->config.powermeter_power_round_to),
                this->config.powermeter_power_decimal_places);
        }
    }

    if (powermeter.voltage_V.has_value()) {
        auto& voltage_V = powermeter.voltage_V.value();
        root["voltage_V"] |= ryml::MAP;
        if (voltage_V.DC.has_value()) {
            root["voltage_V"]["DC"] << ryml::fmt::real(
                this->round_to_nearest_step(voltage_V.DC.value(), this->config.powermeter_voltage_round_to),
                this->config.powermeter_voltage_decimal_places);
        }
        if (voltage_V.L1.has_value()) {
            root["voltage_V"]["L1"] << ryml::fmt::real(
                this->round_to_nearest_step(voltage_V.L1.value(), this->config.powermeter_voltage_round_to),
                this->config.powermeter_voltage_decimal_places);
        }
        if (voltage_V.L2.has_value()) {
            root["voltage_V"]["L2"] << ryml::fmt::real(
                this->round_to_nearest_step(voltage_V.L2.value(), this->config.powermeter_voltage_round_to),
                this->config.powermeter_voltage_decimal_places);
        }
        if (voltage_V.L3.has_value()) {
            root["voltage_V"]["L3"] << ryml::fmt::real(
                this->round_to_nearest_step(voltage_V.L3.value(), this->config.powermeter_voltage_round_to),
                this->config.powermeter_voltage_decimal_places);
        }
    }

    if (powermeter.VAR.has_value()) {
        auto& VAR = powermeter.VAR.value();
        root["VAR"] |= ryml::MAP;
        root["VAR"]["total"] << ryml::fmt::real(
            this->round_to_nearest_step(VAR.total, this->config.powermeter_VAR_round_to),
            this->config.powermeter_VAR_decimal_places);
        if (VAR.L1.has_value()) {
            root["VAR"]["L1"] << ryml::fmt::real(
                this->round_to_nearest_step(VAR.L1.value(), this->config.powermeter_VAR_round_to),
                this->config.powermeter_VAR_decimal_places);
        }
        if (VAR.L2.has_value()) {
            root["VAR"]["L2"] << ryml::fmt::real(
                this->round_to_nearest_step(VAR.L2.value(), this->config.powermeter_VAR_round_to),
                this->config.powermeter_VAR_decimal_places);
        }
        if (VAR.L3.has_value()) {
            root["VAR"]["L3"] << ryml::fmt::real(
                this->round_to_nearest_step(VAR.L3.value(), this->config.powermeter_VAR_round_to),
                this->config.powermeter_VAR_decimal_places);
        }
    }

    if (powermeter.current_A.has_value()) {
        auto& current_A = powermeter.current_A.value();
        root["current_A"] |= ryml::MAP;
        if (current_A.DC.has_value()) {
            root["current_A"]["DC"] << ryml::fmt::real(
                this->round_to_nearest_step(current_A.DC.value(), this->config.powermeter_current_round_to),
                this->config.powermeter_current_decimal_places);
        }
        if (current_A.L1.has_value()) {
            root["current_A"]["L1"] << ryml::fmt::real(
                this->round_to_nearest_step(current_A.L1.value(), this->config.powermeter_current_round_to),
                this->config.powermeter_current_decimal_places);
        }
        if (current_A.L2.has_value()) {
            root["current_A"]["L2"] << ryml::fmt::real(
                this->round_to_nearest_step(current_A.L2.value(), this->config.powermeter_current_round_to),
                this->config.powermeter_current_decimal_places);
        }
        if (current_A.L3.has_value()) {
            root["current_A"]["L3"] << ryml::fmt::real(
                this->round_to_nearest_step(current_A.L3.value(), this->config.powermeter_current_round_to),
                this->config.powermeter_current_decimal_places);
        }

        if (current_A.N.has_value()) {
            root["current_A"]["N"] << ryml::fmt::real(
                this->round_to_nearest_step(current_A.N.value(), this->config.powermeter_current_round_to),
                this->config.powermeter_current_decimal_places);
        }
    }

    if (powermeter.frequency_Hz.has_value()) {
        auto& frequency_Hz = powermeter.frequency_Hz.value();
        root["frequency_Hz"] |= ryml::MAP;
        root["frequency_Hz"]["L1"] << ryml::fmt::real(
            this->round_to_nearest_step(frequency_Hz.L1, this->config.powermeter_frequency_round_to),
            this->config.powermeter_frequency_decimal_places);

        if (frequency_Hz.L2.has_value()) {
            root["frequency_Hz"]["L2"] << ryml::fmt::real(
                this->round_to_nearest_step(frequency_Hz.L2.value(), this->config.powermeter_frequency_round_to),
                this->config.powermeter_frequency_decimal_places);
        }
        if (frequency_Hz.L3.has_value()) {
            root["frequency_Hz"]["L3"] << ryml::fmt::real(
                this->round_to_nearest_step(frequency_Hz.L3.value(), this->config.powermeter_frequency_round_to),
                this->config.powermeter_frequency_decimal_places);
        }
    }

    std::stringstream power_meter_stream;
    power_meter_stream << ryml::as_json(tree);
    return power_meter_stream.str();
}

std::string LimitDecimalPlaces::limit(const types::evse_board_support::HardwareCapabilities& hw_capabilities) {
    ryml::Tree tree;
    ryml::NodeRef root = tree.rootref();
    root |= ryml::MAP;

    // add informative hardware capabilities entries
    root["max_phase_count_import"] << hw_capabilities.max_phase_count_import;
    root["min_phase_count_import"] << hw_capabilities.min_phase_count_import;
    root["max_phase_count_export"] << hw_capabilities.max_phase_count_export;
    root["min_phase_count_export"] << hw_capabilities.min_phase_count_export;
    root["supports_changing_phases_during_charging"]
        << ryml::fmt::boolalpha(hw_capabilities.supports_changing_phases_during_charging);

    // limit decimal places

    root["max_current_A_import"] << ryml::fmt::real(
        this->round_to_nearest_step(hw_capabilities.max_current_A_import,
                                    this->config.hw_caps_max_current_import_round_to),
        this->config.hw_caps_max_current_import_decimal_places);
    root["min_current_A_import"] << ryml::fmt::real(
        this->round_to_nearest_step(hw_capabilities.min_current_A_import,
                                    this->config.hw_caps_min_current_import_round_to),
        this->config.hw_caps_max_current_import_decimal_places);
    root["max_current_A_export"] << ryml::fmt::real(
        this->round_to_nearest_step(hw_capabilities.max_current_A_export,
                                    this->config.hw_caps_max_current_export_round_to),
        this->config.hw_caps_max_current_import_decimal_places);
    root["min_current_A_export"] << ryml::fmt::real(
        this->round_to_nearest_step(hw_capabilities.min_current_A_export,
                                    this->config.hw_caps_min_current_export_round_to),
        this->config.hw_caps_min_current_export_decimal_places);
    if (hw_capabilities.max_plug_temperature_C.has_value()) {
        root["max_plug_temperature_C"] << ryml::fmt::real(
            this->round_to_nearest_step(hw_capabilities.max_plug_temperature_C.value(),
                                        this->config.hw_caps_max_plug_temperature_C_round_to),
            this->config.hw_caps_max_plug_temperature_C_decimal_places);
    }

    root["connector_type"] << types::evse_board_support::connector_type_to_string(hw_capabilities.connector_type);

    std::stringstream hardware_capabilities_stream;
    hardware_capabilities_stream << ryml::as_json(tree);
    return hardware_capabilities_stream.str();
}

std::string LimitDecimalPlaces::limit(const types::evse_manager::Limits& limits) {
    ryml::Tree tree;
    ryml::NodeRef root = tree.rootref();
    root |= ryml::MAP;

    // add informative limits entries
    if (limits.uuid.has_value()) {
        root["uuid"] << limits.uuid.value();
    }
    root["nr_of_phases_available"] << limits.nr_of_phases_available;

    // limit decimal places
    root["max_current"] << ryml::fmt::real(
        this->round_to_nearest_step(limits.max_current, this->config.limits_max_current_round_to),
        this->config.limits_max_current_decimal_places);

    std::stringstream limits_stream;
    limits_stream << ryml::as_json(tree);
    return limits_stream.str();
}

std::string LimitDecimalPlaces::limit(const types::evse_board_support::Telemetry& telemetry) {
    ryml::Tree tree;
    ryml::NodeRef root = tree.rootref();
    root |= ryml::MAP;

    root["phase_seq_error"] << ryml::fmt::boolalpha(telemetry.relais_on);

    // limit decimal places
    root["temperature"] << ryml::fmt::real(
        this->round_to_nearest_step(telemetry.evse_temperature_C, this->config.telemetry_evse_temperature_C_round_to),
        this->config.telemetry_evse_temperature_C_decimal_places);
    root["fan_rpm"] << ryml::fmt::real(
        this->round_to_nearest_step(telemetry.fan_rpm, this->config.telemetry_fan_rpm_round_to),
        this->config.telemetry_fan_rpm_decimal_places);
    root["supply_voltage_12V"] << ryml::fmt::real(
        this->round_to_nearest_step(telemetry.supply_voltage_12V, this->config.telemetry_supply_voltage_12V_round_to),
        this->config.telemetry_supply_voltage_12V_decimal_places);
    root["supply_voltage_minus_12V"] << ryml::fmt::real(
        this->round_to_nearest_step(telemetry.supply_voltage_minus_12V,
                                    this->config.telemetry_supply_voltage_minus_12V_round_to),
        this->config.telemetry_supply_voltage_minus_12V_decimal_places);
    if (telemetry.plug_temperature_C.has_value()) {
        root["plug_temperature_C"] << ryml::fmt::real(
            this->round_to_nearest_step(telemetry.plug_temperature_C.value(),
                                        this->config.telemetry_plug_temperature_C_round_to),
            this->config.telemetry_plug_temperature_C_decimal_places);
    }
    std::stringstream telemetry_stream;
    telemetry_stream << ryml::as_json(tree);
    return telemetry_stream.str();
}

double LimitDecimalPlaces::round_to_nearest_step(double value, double step) {
    if (step <= 0) {
        return value;
    }
    return std::round(value / step) * step;
}

} // namespace module
