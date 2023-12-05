// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "YetiDriver.hpp"
#include <fmt/core.h>
#include <utils/date.hpp>

namespace module {

std::string lowlevelstate_to_string(const DebugUpdate_LoLevelState s, bool pwm_on) {
    const std::string pwm = (pwm_on ? "2" : "1");
    switch (s) {
    case DebugUpdate_LoLevelState_DISABLED:
        return "Disabled";
    case DebugUpdate_LoLevelState_A:
        return "A" + pwm;
    case DebugUpdate_LoLevelState_B:
        return "B" + pwm;
    case DebugUpdate_LoLevelState_C:
        return "C" + pwm;
    case DebugUpdate_LoLevelState_D:
        return "D" + pwm;
    case DebugUpdate_LoLevelState_E:
        return "E";
    case DebugUpdate_LoLevelState_F:
        return "F";
    case DebugUpdate_LoLevelState_DF:
        return "DF";
    default:
        return "Unknown";
    }
}

void YetiDriver::init() {

    // initialize serial driver
    if (!serial.openDevice(config.serial_port.c_str(), config.baud_rate)) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestConfigError, "Could not open serial port ", config.serial_port,
                                    " with baud rate ", config.baud_rate));
        return;
    }

    telemetry_power_path_controller_version = {{"timestamp", ""},
                                               {"type", "power_path_controller_version"},
                                               {"hardware_version", 1},
                                               {"software_version", "1.00"},
                                               {"date_manufactured", "N/A"},
                                               {"operating_time_h", 5},
                                               {"operating_time_h_warning", 5000},
                                               {"operating_time_h_error", 6000},
                                               {"software_version", "1.00"},
                                               {"error", false}};

    telemetry_power_path_controller = {{"timestamp", ""},
                                       {"type", "power_path_controller"},
                                       {"cp_voltage_high", 0.0},
                                       {"cp_voltage_low", 0.0},
                                       {"cp_pwm_duty_cycle", 0.0},
                                       {"cp_state", "A1"},
                                       {"pp_ohm", 0.0},
                                       {"supply_voltage_12V", 0.0},
                                       {"supply_voltage_minus_12V", 0.0},
                                       {"temperature_controller", 0.0},
                                       {"temperature_car_connector", 0.0},
                                       {"watchdog_reset_count", 0.0},
                                       {"error", false}};

    telemetry_power_switch = {{"timestamp", ""},
                              {"type", "power_switch"},
                              {"switching_count", 0},
                              {"switching_count_warning", 30000},
                              {"switching_count_error", 50000},
                              {"is_on", false},
                              {"time_to_switch_on_ms", 100},
                              {"time_to_switch_off_ms", 100},
                              {"temperature_C", 0.0},
                              {"error", false},
                              {"error_over_current", false}};

    telemetry_rcd = {{"timestamp", ""},    //
                     {"type", "rcd"},      //
                     {"enabled", true},    //
                     {"current_mA", 0.0},  //
                     {"triggered", false}, //
                     {"error", false}};    //

    invoke_init(*p_powermeter);
    invoke_init(*p_board_support);
}

void YetiDriver::ready() {
    serial.run();

    if (!serial.reset(config.reset_gpio)) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti reset not successful."));
    }

    serial.signalSpuriousReset.connect(
        [this]() { EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti uC spurious reset!")); });
    serial.signalConnectionTimeout.connect(
        [this]() { EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti UART timeout!")); });

    serial.setControlMode(str_to_control_mode(config.control_mode));

    invoke_ready(*p_powermeter);
    invoke_ready(*p_board_support);

    serial.signalKeepAliveLo.connect([this](const KeepAliveLo& k) {
        auto k_json = keep_alive_lo_to_json(k);
        mqtt.publish("/external/keepalive_json", k_json.dump());
    });

    telemetryThreadHandle = std::thread([this]() {
        while (!telemetryThreadHandle.shouldExit()) {
            sleep(10);
            {
                std::scoped_lock lock(telemetry_mutex);
                publish_external_telemetry_livedata("power_path_controller", telemetry_power_path_controller);
                publish_external_telemetry_livedata("rcd", telemetry_rcd);
                publish_external_telemetry_livedata("power_path_controller_version",
                                                    telemetry_power_path_controller_version);
            }
        }
    });

    serial.signalDebugUpdate.connect([this](const DebugUpdate& d) {
        static bool relay_was_on = true;
        auto d_json = debug_update_to_json(d);
        mqtt.publish("/external/debug_json", d_json.dump());
        {
            std::scoped_lock lock(telemetry_mutex);
            // update external telemetry data
            telemetry_power_path_controller.at("timestamp") = Everest::Date::to_rfc3339(date::utc_clock::now());
            telemetry_power_path_controller.at("cp_voltage_high") = d.evse_pwm_voltage_hi;
            telemetry_power_path_controller.at("cp_voltage_low") = d.evse_pwm_voltage_lo;
            telemetry_power_path_controller.at("cp_pwm_duty_cycle") = 0.; // FIXME this should be included
            telemetry_power_path_controller.at("cp_state") =
                lowlevelstate_to_string(d.lowlevel_state, d.evse_pwm_running);
            telemetry_power_path_controller.at("pp_ohm") = 0.; // FIXME this should be included
            telemetry_power_path_controller.at("supply_voltage_12V") = d.supply_voltage_12V;
            telemetry_power_path_controller.at("supply_voltage_minus_12V") = d.supply_voltage_N12V;
            telemetry_power_path_controller.at("temperature_controller") = d.cpu_temperature;
            telemetry_power_path_controller.at("temperature_car_connector") = 0.;
            telemetry_power_path_controller.at("watchdog_reset_count") = d.watchdog_reset_count;

            telemetry_rcd.at("timestamp") = Everest::Date::to_rfc3339(date::utc_clock::now());
            telemetry_rcd.at("current_mA") = d.rcd_current;
            telemetry_rcd.at("enabled") = d.rcd_enabled;

            telemetry_power_switch.at("timestamp") = Everest::Date::to_rfc3339(date::utc_clock::now());
            telemetry_power_switch.at("is_on") = d.relais_on;
            if (relay_was_on != d.relais_on) {
                publish_external_telemetry_livedata("power_switch", telemetry_power_switch);
            }
            relay_was_on = d.relais_on;
        }
    });
}

Everest::json power_meter_data_to_json(const PowerMeter& p) {
    Everest::json j;
    j["time_stamp"] = p.time_stamp;
    j["vrmsL1"] = p.vrmsL1;
    j["vrmsL2"] = p.vrmsL2;
    j["vrmsL3"] = p.vrmsL3;

    j["irmsL1"] = p.irmsL1;
    j["irmsL2"] = p.irmsL2;
    j["irmsL3"] = p.irmsL3;
    j["irmsN"] = p.irmsN;
    j["wattHrL1"] = p.wattHrL1;
    j["wattHrL2"] = p.wattHrL2;
    j["wattHrL3"] = p.wattHrL3;
    j["totalWattHr_Out"] = p.totalWattHr;
    j["tempL1"] = p.tempL1;
    j["tempL2"] = p.tempL2;
    j["tempL3"] = p.tempL3;
    j["wattL1"] = p.wattL1;
    j["wattL2"] = p.wattL2;
    j["wattL3"] = p.wattL3;
    j["freqL1"] = p.freqL1;
    j["freqL2"] = p.freqL2;
    j["freqL3"] = p.freqL3;
    j["phaseSeqError"] = p.phaseSeqError;
    return j;
}

std::string state_to_string(const StateUpdate& s) {
    switch (s.state) {
    case StateUpdate_State_DISABLED:
        return "Disabled";
    case StateUpdate_State_IDLE:
        return "Idle";
    case StateUpdate_State_WAITING_FOR_AUTHENTICATION:
        return "Waiting for Auth";
    case StateUpdate_State_CHARGING:
        return "Charging";
    case StateUpdate_State_CHARGING_PAUSED_EV:
        return "Car Paused";
    case StateUpdate_State_CHARGING_PAUSED_EVSE:
        return "EVSE Paused";
    case StateUpdate_State_CHARGING_FINSIHED:
        return "Finished";
    case StateUpdate_State_ERROR:
        return "Error";
    case StateUpdate_State_FAULTED:
        return "Faulted";
    default:
        return "Unknown";
    }
}

std::string error_type_to_string(ErrorFlags s) {
    switch (s.type) {
    case ErrorFlags_ErrorType_ERROR_F:
        return "EVSE Fault";
    case ErrorFlags_ErrorType_ERROR_E:
        return "Car Fault";
    case ErrorFlags_ErrorType_ERROR_DF:
        return "Diode Fault";
    case ErrorFlags_ErrorType_ERROR_RELAIS:
        return "Relais Fault";
    case ErrorFlags_ErrorType_ERROR_VENTILATION_NOT_AVAILABLE:
        return "Ventilation n/a";
    case ErrorFlags_ErrorType_ERROR_RCD:
        return "RCD Fault";
    default:
        return "Unknown";
    }
}

Everest::json state_update_to_json(const StateUpdate& s) {
    Everest::json j;
    j["time_stamp"] = (int)s.time_stamp;

    j["state"] = s.state;
    j["state_string"] = state_to_string(s);

    if (s.which_state_flags == StateUpdate_error_type_tag) {
        j["error_type"] = static_cast<int>(s.state_flags.error_type.type);
        j["error_string"] = error_type_to_string(s.state_flags.error_type);
    }

    return j;
}

Everest::json debug_update_to_json(const DebugUpdate& d) {
    Everest::json j;
    j["time_stamp"] = static_cast<int>(d.time_stamp);
    j["evse_pwm_voltage_hi"] = d.evse_pwm_voltage_hi;
    j["evse_pwm_voltage_lo"] = d.evse_pwm_voltage_lo;
    j["supply_voltage_12V"] = d.supply_voltage_12V;
    j["supply_voltage_N12V"] = d.supply_voltage_N12V;
    j["lowlevel_state"] = d.lowlevel_state;
    j["evse_pwm_running"] = d.evse_pwm_running;
    j["ev_simplified_mode"] = d.ev_simplified_mode;
    j["has_ventilation"] = d.has_ventilation;
    j["ventilated_charging_active"] = d.ventilated_charging_active;
    j["rcd_reclosing_allowed"] = d.rcd_reclosing_allowed;
    j["control_mode"] = d.control_mode;
    j["authorized"] = d.authorized;
    j["cpu_temperature"] = d.cpu_temperature;
    j["rcd_enabled"] = d.rcd_enabled;
    j["evse_pp_voltage"] = d.evse_pp_voltage;
    j["max_current_cable"] = d.max_current_cable;
    j["watchdog_reset_count"] = d.watchdog_reset_count;
    j["simulation"] = d.simulation;
    j["max_current"] = d.max_current;
    j["use_three_phases"] = d.use_three_phases;
    j["rcd_current"] = d.rcd_current;
    j["relais_on"] = d.relais_on;
    return j;
}

Everest::json keep_alive_lo_to_json(const KeepAliveLo& k) {
    Everest::json j;
    j["time_stamp"] = static_cast<int>(k.time_stamp);
    j["hw_type"] = static_cast<int>(k.hw_type);
    j["hw_revision"] = static_cast<int>(k.hw_revision);
    j["protocol_version_major"] = static_cast<int>(k.protocol_version_major);
    j["protocol_version_minor"] = static_cast<int>(k.protocol_version_minor);
    j["sw_version_string"] = std::string(k.sw_version_string);
    return j;
}

InterfaceControlMode str_to_control_mode(std::string data) {
    if (data == "low")
        return InterfaceControlMode_LOW;
    else if (data == "high")
        return InterfaceControlMode_HIGH;
    else
        return InterfaceControlMode_NONE;
}

void YetiDriver::publish_external_telemetry_livedata(const std::string& topic, const Everest::TelemetryMap& data) {
    if (info.telemetry_enabled) {
        telemetry.publish("livedata", topic, data);
    }
}

} // namespace module
