#include "YetiDriver.hpp"

namespace module {

void YetiDriver::init() {
    // initialize serial driver
    if (!serial.openDevice(config.serial_port.c_str(), config.baud_rate)) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestConfigError, "Could not open serial port ", config.serial_port,
                                    " with baud rate ", config.baud_rate));
        return;
    }

    invoke_init(*p_powermeter);
    invoke_init(*p_yeti_extras);
    invoke_init(*p_debug_yeti);
    invoke_init(*p_debug_powermeter);
    invoke_init(*p_debug_state);
    invoke_init(*p_debug_keepalive);
    invoke_init(*p_yeti_simulation_control);
    invoke_init(*p_board_support);

    serial.signalSpuriousReset.connect([this]() { EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti uC spurious reset!"));});
    serial.signalConnectionTimeout.connect([this]() { EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti UART timeout!"));});
}


void YetiDriver::ready() {
    serial.run();

    if (!serial.reset()) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestInternalError, "Yeti reset not successful."));
    }

    serial.setControlMode(str_to_control_mode(config.control_mode));
    
    invoke_ready(*p_powermeter);
    invoke_ready(*p_yeti_extras);
    invoke_ready(*p_debug_yeti);
    invoke_ready(*p_debug_powermeter);
    invoke_ready(*p_debug_state);
    invoke_ready(*p_debug_keepalive);
    invoke_ready(*p_yeti_simulation_control);
    invoke_ready(*p_board_support);
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
    if (data == "low") return InterfaceControlMode_LOW;
    else if (data == "high") return InterfaceControlMode_HIGH;
    else return InterfaceControlMode_NONE;
    }

} // namespace module
