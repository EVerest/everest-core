// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "YetiSimulator.hpp"
#include "board_support/evse_board_supportImpl.hpp"
#include "util/util.hpp"

namespace module {

void YetiSimulator::init() {
    invoke_init(*p_powermeter);
    invoke_init(*p_board_support);
    invoke_init(*p_ev_board_support);
    invoke_init(*p_rcd);
    invoke_init(*p_connector_lock);

    clear_data();

    mqtt_handler = std::make_unique<MqttHandler>(p_board_support.get(), p_rcd.get(), p_connector_lock.get());
    mqtt.subscribe("everest_external/nodered/" + std::to_string(config.connector_id) + "/carsim/error",
                   [this](const std::string& payload) { mqtt_handler->handle_mqtt_payload(payload); });
}

void YetiSimulator::ready() {
    invoke_ready(*p_powermeter);
    invoke_ready(*p_board_support);
    invoke_ready(*p_ev_board_support);
    invoke_ready(*p_rcd);
    invoke_ready(*p_connector_lock);

    module_state->pubCnt = 0;

    Simulator<YetiSimulator, board_support::evse_board_supportImpl>(250, this);
    if (info.telemetry_enabled) {
        std::thread(&YetiSimulator::run_telemetry_slow, this).detach();
        std::thread(&YetiSimulator::run_telemetry_fast, this).detach();
    }
}

state::ModuleState& YetiSimulator::get_module_state() {
    return *module_state;
}

evse_board_supportImplBase& YetiSimulator::get_board_support() {
    return *p_board_support;
};

ev_board_supportImplBase& YetiSimulator::get_ev_board_support() {
    return *p_ev_board_support;
}

ac_rcdImplBase& YetiSimulator::get_ac_rcd() {
    return *p_rcd;
}

connector_lockImplBase& YetiSimulator::get_connector_lock() {
    return *p_connector_lock;
}

Everest::MqttProvider& YetiSimulator::get_mqtt() {
    return mqtt;
}
const ModuleInfo& YetiSimulator::get_info() const {
    return info;
}
powermeterImplBase& YetiSimulator::get_powermeter() {
    return *p_powermeter;
}

void YetiSimulator::clear_data() {
    module_state = std::make_unique<state::ModuleState>();
}

void YetiSimulator::run_telemetry_slow() {
    const auto current_iso_time_string = util::get_current_iso_time_string();

    auto& p_p_c_v = module_state->telemetry_data.power_path_controller_version;
    p_p_c_v.timestamp = current_iso_time_string;

    telemetry.publish("livedata", "power_path_controller_version",
                      {{"timestamp", p_p_c_v.timestamp},
                       {"type", p_p_c_v.type},
                       {"hardware_version", p_p_c_v.hardware_version},
                       {"software_version", p_p_c_v.software_version},
                       {"date_manufactured", p_p_c_v.date_manufactured},
                       {"operating_time_h", p_p_c_v.operating_time_h},
                       {"operating_time_warning", p_p_c_v.operating_time_h_warning},
                       {"operating_time_error", p_p_c_v.operating_time_h_error},
                       {"error", p_p_c_v.error}});
}

void YetiSimulator::run_telemetry_fast() {
    const auto current_iso_time_string = util::get_current_iso_time_string();
    auto& p_p_c = module_state->telemetry_data.power_path_controller;
    p_p_c.timestamp = current_iso_time_string;
    p_p_c.cp_voltage_high = module_state->pwm_voltage_hi; // TODO: check if this is the correct value
    p_p_c.cp_voltage_low = module_state->pwm_voltage_lo;  // TODO: same here
    p_p_c.cp_pwm_duty_cycle = module_state->pwm_duty_cycle * 100.0;
    p_p_c.cp_state = state_to_string(*module_state);

    p_p_c.temperature_controller = module_state->powermeter_data.tempL1;
    p_p_c.temperature_car_connector = module_state->powermeter_data.tempL1 * 2.0;
    p_p_c.watchdog_reset_count = 0;
    p_p_c.error = false;

    auto& p_s = module_state->telemetry_data.power_switch;
    p_s.timestamp = current_iso_time_string;
    p_s.is_on = module_state->relais_on;
    p_s.time_to_switch_on_ms = 110;
    p_s.time_to_switch_off_ms = 100;
    p_s.temperature_C = 20;
    p_s.error = false;
    p_s.error_over_current = false;

    auto& rcd = module_state->telemetry_data.rcd;
    rcd.timestamp = current_iso_time_string;
    rcd.current_mA = module_state->simulation_data.rcd_current;

    telemetry.publish("livedata", "power_path_controller",
                      {{"timestamp", p_p_c.timestamp},
                       {"type", p_p_c.type},
                       {"cp_voltage_high", p_p_c.cp_voltage_high},
                       {"cp_voltage_low", p_p_c.cp_voltage_low},
                       {"cp_pwm_duty_cycle", p_p_c.cp_pwm_duty_cycle},
                       {"cp_state", p_p_c.cp_state},
                       {"pp_ohm", p_p_c.pp_ohm},
                       {"supply_voltage_12V", p_p_c.supply_voltage_12V},
                       {"supply_voltage_minus_12V", p_p_c.supply_voltage_minus_12V},
                       {"temperature_controller", p_p_c.temperature_controller},
                       {"temperature_car_connector", p_p_c.temperature_car_connector},
                       {"watchdog_reset_count", p_p_c.watchdog_reset_count},
                       {"error", p_p_c.error}});
    telemetry.publish("livedata", "power_switch",
                      {{"timestamp", p_s.timestamp},
                       {"type", p_s.type},
                       {"switching_count", p_s.switching_count},
                       {"switching_count_warning", p_s.switching_count_warning},
                       {"switching_count_error", p_s.switching_count_error},
                       {"is_on", p_s.is_on},
                       {"time_to_switch_on_ms", p_s.time_to_switch_on_ms},
                       {"time_to_switch_off_ms", p_s.time_to_switch_off_ms},
                       {"temperature_C", p_s.temperature_C},
                       {"error", p_s.error},
                       {"error_over_current", p_s.error_over_current}});
    telemetry.publish("livedata", "rcd",
                      {{"timestamp", rcd.timestamp},
                       {"type", rcd.type},
                       {"enabled", rcd.enabled},
                       {"current_mA", rcd.current_mA},
                       {"triggered", rcd.triggered},
                       {"error", rcd.error}});
}

} // namespace module
