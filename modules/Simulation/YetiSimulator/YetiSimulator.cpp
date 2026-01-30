// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "board_support/evse_board_supportImpl.hpp"
#include "util/state.hpp"
#include "util/util.hpp"

#include "util/errors.hpp"

namespace module {

namespace {

types::powermeter::Powermeter power_meter_external(const state::PowermeterData& powermeter_data) {
    const auto current_iso_time_string = util::get_current_iso_time_string();
    const auto& [time_stamp, import_totalWattHr, export_totalWattHr, wattL1, vrmsL1, irmsL1, import_wattHrL1,
                 export_wattHrL1, tempL1, freqL1, wattL2, vrmsL2, irmsL2, import_wattHrL2, export_wattHrL2, tempL2,
                 freqL2, wattL3, vrmsL3, irmsL3, import_wattHrL3, export_wattHrL3, tempL3, freqL3, irmsN] =
        powermeter_data;

    const std::vector<types::temperature::Temperature> temperatures = {
        {static_cast<float>(powermeter_data.tempL1), std::nullopt, "Body"}};

    return {current_iso_time_string, // timestamp
            {
                static_cast<float>(import_totalWattHr),
                static_cast<float>(import_wattHrL1),
                static_cast<float>(import_wattHrL2),
                static_cast<float>(import_wattHrL3),
            },                 // energy_Wh_import
            "YETI_POWERMETER", // meter_id
            false,             // phase_seq_error
            std::make_optional<types::units::Energy>({
                static_cast<float>(export_totalWattHr),
                static_cast<float>(export_wattHrL1),
                static_cast<float>(export_wattHrL2),
                static_cast<float>(export_wattHrL3),
            }), // energy_Wh_export
            types::units::Power{
                static_cast<float>(wattL1 + wattL2 + wattL3),
                static_cast<float>(wattL1),
                static_cast<float>(wattL2),
                static_cast<float>(wattL3),
            },                                                                  // power_W
            types::units::Voltage{std::nullopt, vrmsL1, vrmsL2, vrmsL3},        // voltage_V
            std::nullopt,                                                       // VAR
            types::units::Current{std::nullopt, irmsL1, irmsL2, irmsL3, irmsN}, // current_A
            types::units::Frequency{static_cast<float>(freqL1), static_cast<float>(freqL2),
                                    static_cast<float>(freqL3)}, // frequency_Hz
            std::nullopt,                                        // energy_Wh_import_signed
            std::nullopt,                                        // energy_Wh_export_signed
            std::nullopt,                                        // power_W_signed
            std::nullopt,                                        // voltage_V_signed
            std::nullopt,                                        // VAR_signed
            std::nullopt,                                        // current_A_signed
            std::nullopt,                                        // frequency_Hz_signed
            std::nullopt,                                        // signed_meter_value
            temperatures};                                       // temperatures
}

double duty_cycle_to_amps(const double dc) {
    if (dc < 8.0 / 100.0)
        return 0;
    if (dc < 85.0 / 100.0)
        return dc * 100.0 * 0.6;
    if (dc < 96.0 / 100.0)
        return (dc * 100.0 - 64) * 2.5;
    if (dc < 97.0 / 100.0)
        return 80;
    return 0;
}

bool is_voltage_in_range(const double voltage, const double center) {
    constexpr auto interval = 1.1;
    return voltage > center - interval and voltage < center + interval;
}

std::string event_to_string(state::State state) {
    using state::State;

    switch (state) {
    case State::STATE_A:
        return "A";
    case State::STATE_B:
        return "B";
    case State::STATE_C:
        return "C";
    case State::STATE_D:
        return "D";
    case State::STATE_E:
        return "E";
    case State::STATE_F:
        return "F";
    case State::STATE_DISABLED:
        return "F";
    case State::Event_PowerOn:
        return "PowerOn";
    case State::Event_PowerOff:
        return "PowerOff";
    default:
        return "invalid";
    }
}

types::board_support_common::BspEvent event_to_enum(state::State event) {
    using state::State;
    using types::board_support_common::Event;

    switch (event) {
    case State::STATE_A:
        return {Event::A};
    case State::STATE_B:
        return {Event::B};
    case State::STATE_C:
        return {Event::C};
    case State::STATE_D:
        return {Event::D};
    case State::STATE_E:
        return {Event::E};
    case State::STATE_F:
        return {Event::F};
    case State::STATE_DISABLED:
        return {Event::F};
    case State::Event_PowerOn:
        return {Event::PowerOn};
    case State::Event_PowerOff:
        return {Event::PowerOff};
    default:
        EVLOG_error << "Invalid event : " << event_to_string(event);
        return {Event::F};
    }
}

} // namespace

void YetiSimulator::init() {
    invoke_init(*p_powermeter);
    invoke_init(*p_board_support);
    invoke_init(*p_ev_board_support);
    invoke_init(*p_rcd);
    invoke_init(*p_connector_lock);

    reset_module_state();

    mqtt.subscribe(
        "everest_external/nodered/" + std::to_string(config.connector_id) + "/carsim/error",
        [this](const std::string& payload) {
            const auto [raise, error_definition] = parse_error_type(payload);

            if (not error_definition.has_value()) {
                return;
            }
            if (error_definition->error_target == ErrorTarget::BoardSupport) {
                const auto error =
                    p_board_support->error_factory->create_error(error_definition->type, error_definition->sub_type,
                                                                 error_definition->message, error_definition->severity);
                forward_error(p_board_support, error, raise);
            } else if (error_definition->error_target == ErrorTarget::ConnectorLock) {
                const auto error = p_connector_lock->error_factory->create_error(
                    error_definition->type, error_definition->sub_type, error_definition->message,
                    error_definition->severity);
            } else if (error_definition->error_target == ErrorTarget::Rcd) {
                const auto error =
                    p_rcd->error_factory->create_error(error_definition->type, error_definition->sub_type,
                                                       error_definition->message, error_definition->severity);
            } else if (error_definition->error_target == ErrorTarget::Powermeter) {
                const auto error =
                    p_powermeter->error_factory->create_error(error_definition->type, error_definition->sub_type,
                                                              error_definition->message, error_definition->severity);
                forward_error(p_powermeter, error, raise);
            } else {
                EVLOG_error << "No known ErrorTarget";
            }
        });
}

void YetiSimulator::ready() {
    invoke_ready(*p_powermeter);
    invoke_ready(*p_board_support);
    invoke_ready(*p_ev_board_support);
    invoke_ready(*p_rcd);
    invoke_ready(*p_connector_lock);

    module_state->pubCnt = 0;

    std::thread(&YetiSimulator::run_simulation, this, 250).detach();

    if (info.telemetry_enabled) {
        std::thread(&YetiSimulator::run_telemetry_slow, this).detach();
        std::thread(&YetiSimulator::run_telemetry_fast, this).detach();
    }
}

void YetiSimulator::run_telemetry_slow() const {

    while (true) {
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

        std::this_thread::sleep_for(std::chrono::milliseconds{15000});
    }
}

void YetiSimulator::run_telemetry_fast() const {

    while (true) {
        const auto current_iso_time_string = util::get_current_iso_time_string();
        auto& p_p_c = module_state->telemetry_data.power_path_controller;
        p_p_c.timestamp = current_iso_time_string;
        p_p_c.cp_voltage_high = module_state->pwm_voltage_hi;
        p_p_c.cp_voltage_low = module_state->pwm_voltage_lo;
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
        std::this_thread::sleep_for(std::chrono::milliseconds{1000});
    }
}

[[noreturn]] void YetiSimulator::run_simulation(const int sleep_time_ms) {
    while (true) {
        if (module_state->simulation_enabled) {
            simulation_step();
        }

        module_state->pubCnt++;
        switch (module_state->pubCnt) {
        case 1:
            publish_powermeter();
            publish_telemetry();
            break;
        case 3:
            publish_keepalive();
            break;
        default:
            module_state->pubCnt = 0;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds{sleep_time_ms});
    }
}

void YetiSimulator::simulation_step() {
    check_error_rcd();
    read_from_car();
    simulation_statemachine();
    add_noise();
    simulate_powermeter();
    publish_ev_board_support();
}

void YetiSimulator::check_error_rcd() {
    using Everest::error::Severity;

    const auto rcd_error = error_definitions::ac_rcd_DC;

    if (module_state->simulation_data.rcd_current > 5.0) {
        if (not p_rcd->error_state_monitor->is_error_active(rcd_error.type, rcd_error.sub_type)) {
            const auto error = p_rcd->error_factory->create_error(rcd_error.type, rcd_error.sub_type, rcd_error.message,
                                                                  Severity::High);
            p_rcd->raise_error(error);
        }
    } else {
        if (p_rcd->error_state_monitor->is_error_active(rcd_error.type, rcd_error.sub_type)) {
            p_rcd->clear_error(rcd_error.type, rcd_error.sub_type);
        }
    }
    p_rcd->publish_rcd_current_mA(module_state->simulation_data.rcd_current);
}

void YetiSimulator::read_from_car() {

    const auto diode_fault = error_definitions::evse_board_support_DiodeFault;

    double amps{0.0};

    // 5% dutycycle: Getting current from HLC
    if (module_state->pwm_duty_cycle >= 0.03 and module_state->pwm_duty_cycle <= 0.07) {
        amps = module_state->ev_max_current;
    } else if (module_state->ev_max_current == 0.0) { // Basic charging: only pwm dutycycle
        amps = duty_cycle_to_amps(module_state->pwm_duty_cycle);
    } else { // Nominal dutycycle: Smallest amp between hlc and pwm dutycycle, ev_max_current can be negative
        amps = std::min(duty_cycle_to_amps(module_state->pwm_duty_cycle), std::fabs(module_state->ev_max_current));
        if (module_state->ev_max_current < 0) {
            amps = -amps;
        }
    }

    const auto amps1 = (module_state->relais_on and module_state->ev_phases > 0) ? amps : 0.0;
    const auto amps2 =
        (module_state->relais_on and module_state->ev_phases > 1 and module_state->use_three_phases_confirmed) ? amps
                                                                                                               : 0.0;
    const auto amps3 =
        (module_state->relais_on and module_state->ev_phases > 2 and module_state->use_three_phases_confirmed) ? amps
                                                                                                               : 0.0;

    if (module_state->pwm_running) {
        module_state->pwm_voltage_hi = module_state->simulation_data.cp_voltage;
        module_state->pwm_voltage_lo = -12.0;
    } else {
        module_state->pwm_voltage_hi = module_state->simulation_data.cp_voltage;
        module_state->pwm_voltage_lo = module_state->pwm_voltage_hi;
    }

    if (module_state->pwm_error_f) {
        module_state->pwm_voltage_hi = -12.0;
        module_state->pwm_voltage_lo = -12.0;
    }
    if (module_state->simulation_data.error_e) {
        module_state->pwm_voltage_hi = 0.0;
        module_state->pwm_voltage_lo = 0.0;
    }
    if (module_state->simulation_data.diode_fail) {
        module_state->pwm_voltage_lo = -module_state->pwm_voltage_hi;
    }

    const auto cpLo = module_state->pwm_voltage_lo;
    const auto cpHi = module_state->pwm_voltage_hi;

    // sth is wrong with negative signal
    if (module_state->pwm_running and not is_voltage_in_range(cpLo, -12.0)) {
        // CP-PE short or signal somehow gone
        if (is_voltage_in_range(cpLo, 0.0) and is_voltage_in_range(cpHi, 0.0)) {
            module_state->current_state = state::State::STATE_E;
            drawPower(0, 0, 0, 0);
        } else if (is_voltage_in_range(cpHi + cpLo, 0.0)) { // Diode fault
            const auto error = p_board_support->error_factory->create_error(diode_fault.type, diode_fault.sub_type,
                                                                            diode_fault.message, diode_fault.severity);
            forward_error(p_board_support, error, true);

            drawPower(0, 0, 0, 0);
        }
    } else if (is_voltage_in_range(cpHi, 12.0)) {
        // +12V State A IDLE (open circuit)
        // clear all errors that clear on disconnection
        if (p_board_support->error_state_monitor->is_error_active(diode_fault.type, diode_fault.sub_type)) {
            p_board_support->clear_error(diode_fault.type);
        }

        module_state->current_state = state::State::STATE_A;
        drawPower(0, 0, 0, 0);
    } else if (is_voltage_in_range(cpHi, 9.0)) {
        module_state->current_state = state::State::STATE_B;
        drawPower(0, 0, 0, 0);
    } else if (is_voltage_in_range(cpHi, 6.0)) {
        module_state->current_state = state::State::STATE_C;
        drawPower(amps1, amps2, amps3, 0.2);
    } else if (is_voltage_in_range(cpHi, 3.0)) {
        module_state->current_state = state::State::STATE_D;
        drawPower(amps1, amps2, amps3, 0.2);
    } else if (is_voltage_in_range(cpHi, -12.0)) {
        module_state->current_state = state::State::STATE_F;
        drawPower(0, 0, 0, 0);
    }
}

void YetiSimulator::simulation_statemachine() {
    if (module_state->republish_state or module_state->last_state not_eq module_state->current_state) {
        publish_event(module_state->current_state);
        module_state->republish_state = false;
    }

    switch (module_state->current_state) {
    case state::State::STATE_DISABLED:
        powerOff();
        module_state->power_on_allowed = false;
        break;

    case state::State::STATE_A: {
        module_state->use_three_phases_confirmed = module_state->use_three_phases;
        cp_state_x1();
        module_state->simplified_mode = false;

        if (module_state->last_state not_eq state::State::STATE_A and
            module_state->last_state not_eq state::State::STATE_DISABLED and
            module_state->last_state not_eq state::State::STATE_F) {
            powerOff();

            // If car was unplugged, reset RCD flag.
            module_state->simdata_setting.rcd_current = 0.1;
            module_state->rcd_error = false;
        }
        break;
    }
    case state::State::STATE_B:
        // Table A.6: Sequence 7 EV stops charging
        // Table A.6: Sequence 8.2 EV supply equipment
        // responds to EV opens S2 (w/o PWM)

        if (module_state->last_state not_eq state::State::STATE_A and
            module_state->last_state not_eq state::State::STATE_B) {
            // Need to switch off according to Table A.6 Sequence 8.1 within
            powerOff();
        }

        // Table A.6: Sequence 1.1 Plug-in
        if (module_state->last_state == state::State::STATE_A) {
            module_state->simplified_mode = false;
            // Fix a race-condition between resetting powermeter parameters and reporting powermeter to the EvseManager
            // back.
            // The EvseManager reports in the transaction_finished event the total charged kWh.
            // With resetting the powermeter too quickly, sometimes the EvseManager reports 0.00 kWh back.
            reset_powermeter();
        }

        break;
    case state::State::STATE_C:
        // Table A.6: Sequence 1.2 Plug-in
        if (module_state->last_state == state::State::STATE_A) {
            module_state->simplified_mode = true;
        }

        if (not module_state->pwm_running) { // C1
            // Table A.6 Sequence 10.2: EV does not stop drawing power even
            // if PWM stops. Stop within 6 seconds (E.g. Kona1!)
            // This is implemented in EvseManager
            if (not module_state->power_on_allowed)
                powerOff();
        } else if (module_state->power_on_allowed) { // C2
            // Table A.6: Sequence 4 EV ready to charge.
            // Must enable power within 3 seconds.
            powerOn();
        }
        break;
    case state::State::STATE_D:
        // Table A.6: Sequence 1.2 Plug-in (w/ventilation)
        if (module_state->last_state == state::State::STATE_A) {
            module_state->simplified_mode = true;
        }

        if (not module_state->pwm_running) {
            // Table A.6 Sequence 10.2: EV does not stop drawing power
            // even if PWM stops. Stop within 6 seconds (E.g. Kona1!)
            /* if (mod.last_pwm_running) // FIMXE implement 6 second timer
                startTimer(6000);
            if (timerElapsed()) { */
            // force power off under load
            powerOff();
            // }
        } else if (module_state->power_on_allowed and not module_state->relais_on and module_state->has_ventilation) {
            // Table A.6: Sequence 4 EV ready to charge.
            // Must enable power within 3 seconds.
            powerOn();
        }
        break;
    case state::State::STATE_E: {
        powerOff();
        cp_state_x1();
        break;
    }
    case state::State::STATE_F:
        powerOff();
        break;
    default:
        break;
    }
    module_state->last_state = module_state->current_state;
    module_state->last_pwm_running = module_state->pwm_running;
}

void YetiSimulator::add_noise() {
    const auto random_number_between_0_and_1 = [] {
        return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    };
    const auto noise = 1 + (random_number_between_0_and_1() - 0.5) * 0.02;
    const auto lonoise = 1 + (random_number_between_0_and_1() - 0.5) * 0.005;
    const auto impedance = module_state->simdata_setting.impedance / 1000.0;

    module_state->simulation_data.currents.L1 = module_state->simdata_setting.currents.L1 * noise;
    module_state->simulation_data.currents.L2 = module_state->simdata_setting.currents.L2 * noise;
    module_state->simulation_data.currents.L3 = module_state->simdata_setting.currents.L3 * noise;
    module_state->simulation_data.currents.N = module_state->simdata_setting.currents.N * noise;

    module_state->simulation_data.voltages.L1 =
        module_state->simdata_setting.voltages.L1 * noise - impedance * module_state->simulation_data.currents.L1;
    module_state->simulation_data.voltages.L2 =
        module_state->simdata_setting.voltages.L2 * noise - impedance * module_state->simulation_data.currents.L2;
    module_state->simulation_data.voltages.L3 =
        module_state->simdata_setting.voltages.L3 * noise - impedance * module_state->simulation_data.currents.L3;

    module_state->simulation_data.frequencies.L1 = module_state->simdata_setting.frequencies.L1 * lonoise;
    module_state->simulation_data.frequencies.L2 = module_state->simdata_setting.frequencies.L2 * lonoise;
    module_state->simulation_data.frequencies.L3 = module_state->simdata_setting.frequencies.L3 * lonoise;

    module_state->simulation_data.cp_voltage = module_state->simdata_setting.cp_voltage * noise;
    module_state->simulation_data.rcd_current = module_state->simdata_setting.rcd_current * noise;
    module_state->simulation_data.pp_resistor = module_state->simdata_setting.pp_resistor * noise;

    module_state->simulation_data.diode_fail = module_state->simdata_setting.diode_fail;
    module_state->simulation_data.error_e = module_state->simdata_setting.error_e;
}

void YetiSimulator::simulate_powermeter() {
    using namespace std::chrono;

    const auto time_stamp = time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count();
    if (module_state->powermeter_sim_last_time_stamp == 0)
        module_state->powermeter_sim_last_time_stamp = time_stamp;
    const auto deltat = time_stamp - module_state->powermeter_sim_last_time_stamp;
    module_state->powermeter_sim_last_time_stamp = time_stamp;

    const auto wattL1 = module_state->simulation_data.voltages.L1 * module_state->simulation_data.currents.L1 *
                        (module_state->relais_on ? 1 : 0);
    const auto wattL2 = module_state->simulation_data.voltages.L2 * module_state->simulation_data.currents.L2 *
                        (module_state->relais_on and module_state->use_three_phases_confirmed ? 1 : 0);
    const auto wattL3 = module_state->simulation_data.voltages.L3 * module_state->simulation_data.currents.L3 *
                        (module_state->relais_on and module_state->use_three_phases_confirmed ? 1 : 0);

    if (module_state->simulation_data.currents.L1 >= 0) {
        module_state->import_watt_hr.L1 += wattL1 * deltat / 1000.0 / 3600.0;
        module_state->import_watt_hr.L2 += wattL2 * deltat / 1000.0 / 3600.0;
        module_state->import_watt_hr.L3 += wattL3 * deltat / 1000.0 / 3600.0;
    } else {
        module_state->export_watt_hr.L1 += wattL1 * deltat / 1000.0 / 3600.0;
        module_state->export_watt_hr.L2 += wattL2 * deltat / 1000.0 / 3600.0;
        module_state->export_watt_hr.L3 += wattL3 * deltat / 1000.0 / 3600.0;
    }

    module_state->powermeter_data.time_stamp = round(time_stamp / 1000);
    module_state->powermeter_data.export_totalWattHr =
        round(module_state->export_watt_hr.L1 + module_state->export_watt_hr.L2 + module_state->export_watt_hr.L3);
    module_state->powermeter_data.import_totalWattHr =
        round(module_state->import_watt_hr.L1 + module_state->import_watt_hr.L2 + module_state->import_watt_hr.L3);

    module_state->powermeter_data.wattL1 = round(wattL1);
    module_state->powermeter_data.vrmsL1 = module_state->simulation_data.voltages.L1;
    module_state->powermeter_data.irmsL1 = module_state->simulation_data.currents.L1;
    module_state->powermeter_data.import_wattHrL1 = round(module_state->import_watt_hr.L1);
    module_state->powermeter_data.export_wattHrL1 = round(module_state->export_watt_hr.L1);
    module_state->powermeter_data.tempL1 = 25.0 + fabs(wattL1 + wattL2 + wattL3) * 0.003;
    module_state->powermeter_data.freqL1 = module_state->simulation_data.frequencies.L1;

    module_state->powermeter_data.wattL2 = round(wattL2);
    module_state->powermeter_data.vrmsL2 = module_state->simulation_data.voltages.L2;
    module_state->powermeter_data.irmsL2 = module_state->simulation_data.currents.L2;
    module_state->powermeter_data.import_wattHrL2 = round(module_state->import_watt_hr.L2);
    module_state->powermeter_data.export_wattHrL2 = round(module_state->export_watt_hr.L2);
    module_state->powermeter_data.tempL2 = 25.0 + fabs(wattL1 + wattL2 + wattL3) * 0.003;
    module_state->powermeter_data.freqL2 = module_state->simulation_data.frequencies.L2;

    module_state->powermeter_data.wattL3 = round(wattL3);
    module_state->powermeter_data.vrmsL3 = module_state->simulation_data.voltages.L3;
    module_state->powermeter_data.irmsL3 = module_state->simulation_data.currents.L3;
    module_state->powermeter_data.import_wattHrL3 = round(module_state->import_watt_hr.L3);
    module_state->powermeter_data.export_wattHrL3 = round(module_state->export_watt_hr.L3);
    module_state->powermeter_data.tempL3 = 25.0 + fabs(wattL1 + wattL2 + wattL3) * 0.003;
    module_state->powermeter_data.freqL3 = module_state->simulation_data.frequencies.L3;

    module_state->powermeter_data.irmsN = module_state->simulation_data.currents.N;
}

void YetiSimulator::publish_ev_board_support() const {
    const auto pp = read_pp_ampacity();

    p_ev_board_support->publish_bsp_measurement(
        {pp, static_cast<float>(module_state->pwm_duty_cycle * 100), module_state->simulation_data.rcd_current});
}

void YetiSimulator::publish_powermeter() {
    p_powermeter->publish_powermeter(power_meter_external(module_state->powermeter_data));

    // Deprecated external stuff
    const auto totalKWattHr =
        module_state->powermeter_data.irmsL1 >= 0
            ? (module_state->powermeter_data.import_wattHrL1 + module_state->powermeter_data.import_wattHrL2 +
               module_state->powermeter_data.import_wattHrL3) /
                  1000.0
            : (module_state->powermeter_data.export_wattHrL1 + module_state->powermeter_data.export_wattHrL2 +
               module_state->powermeter_data.export_wattHrL3) /
                  1000.0;
    mqtt.publish("/external/powermeter/vrmsL1", module_state->powermeter_data.vrmsL1);
    mqtt.publish("/external/powermeter/phaseSeqError", false);
    mqtt.publish("/external/powermeter/time_stamp", std::to_string(module_state->powermeter_data.time_stamp));
    mqtt.publish("/external/powermeter/tempL1", module_state->powermeter_data.tempL1);
    mqtt.publish("/external/powermeter/totalKw",
                 (module_state->powermeter_data.wattL1 + module_state->powermeter_data.wattL2 +
                  module_state->powermeter_data.wattL3) /
                     1000.0);
    mqtt.publish("/external/powermeter/totalKWattHr", totalKWattHr);
    mqtt.publish("/external/powermeter_json", json{module_state->powermeter_data}.dump());

    mqtt.publish("/external/" + info.id + "/powermeter/tempL1", module_state->powermeter_data.tempL1);
    mqtt.publish("/external/" + info.id + "/powermeter/totalKw",
                 (module_state->powermeter_data.wattL1 + module_state->powermeter_data.wattL2 +
                  module_state->powermeter_data.wattL3) /
                     1000.0);
    mqtt.publish("/external/" + info.id + "/powermeter/totalKWattHr", totalKWattHr);
}

void YetiSimulator::publish_telemetry() {
    p_board_support->publish_telemetry({static_cast<float>(module_state->powermeter_data.tempL1), // evse_temperature_C
                                        1500.,                                                    // fan_rpm
                                        12.01,                                                    // supply_voltage_12V
                                        -11.8,                     // supply_voltage_minus_12V
                                        module_state->relais_on}); // relais_on
}

void YetiSimulator::publish_keepalive() {
    using namespace std::chrono;

    mqtt.publish(
        "/external/keepalive_json",
        json{
            {"hw_revision", 0},
            {"hw_type", 0},
            {"protocol_version_major", 0},
            {"protocol_version_minor", 1},
            {"sw_version_string", "simulation"},
            {"time_stamp", {time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count() / 1000}},
        }
            .dump());
}

void YetiSimulator::drawPower(const double l1, const double l2, const double l3, const double n) const {
    module_state->simdata_setting.currents.L1 = l1;
    module_state->simdata_setting.currents.L2 = l2;
    module_state->simdata_setting.currents.L3 = l3;
    module_state->simdata_setting.currents.N = n;
}

void YetiSimulator::powerOn() {
    if (not module_state->relais_on) {
        publish_event(state::State::Event_PowerOn);
        module_state->relais_on = true;
        module_state->telemetry_data.power_switch.switching_count += 1;
    }
}

void YetiSimulator::powerOff() {
    if (module_state->relais_on) {
        publish_event(state::State::Event_PowerOff);
        module_state->telemetry_data.power_switch.switching_count++;
        module_state->relais_on = false;
    }
}

void YetiSimulator::publish_event(state::State event) {
    p_board_support->publish_event(event_to_enum(event));
}

void YetiSimulator::pwm_on(const double dutycycle) {
    if (dutycycle > 0.0) {
        module_state->pwm_duty_cycle = dutycycle;
        module_state->pwm_running = true;
        module_state->pwm_error_f = false;
    } else {
        cp_state_x1();
    }
}
void YetiSimulator::cp_state_x1() {
    module_state->pwm_duty_cycle = 1.0;
    module_state->pwm_running = false;
    module_state->pwm_error_f = false;
}
void YetiSimulator::cp_state_f() {
    module_state->pwm_duty_cycle = 1.0;
    module_state->pwm_running = false;
    module_state->pwm_error_f = true;
}

void YetiSimulator::reset_powermeter() const {
    if (config.reset_powermeter_on_session_start) {
        module_state->import_watt_hr.L1 = 0;
        module_state->import_watt_hr.L2 = 0;
        module_state->import_watt_hr.L3 = 0;
        module_state->export_watt_hr.L1 = 0;
        module_state->export_watt_hr.L2 = 0;
        module_state->export_watt_hr.L3 = 0;
    }
    module_state->powermeter_sim_last_time_stamp = 0;
}

types::board_support_common::ProximityPilot YetiSimulator::read_pp_ampacity() const {
    const auto pp_resistor = module_state->simdata_setting.pp_resistor;

    if (pp_resistor < 80.0 or pp_resistor > 2460) {
        EVLOG_error << "PP resistor value " << pp_resistor << " Ohm seems to be outside the allowed range.";
        return {types::board_support_common::Ampacity::None};
    }

    // PP resistor value in spec, use a conservative interpretation of the resistance ranges
    if (pp_resistor > 936.0 and pp_resistor <= 2460.0) {
        return {types::board_support_common::Ampacity::A_13};
    }
    if (pp_resistor > 308.0 && pp_resistor <= 936.0) {
        return {types::board_support_common::Ampacity::A_20};
    }
    if (pp_resistor > 140.0 && pp_resistor <= 308.0) {
        return {types::board_support_common::Ampacity::A_32};
    }
    if (pp_resistor > 80.0 && pp_resistor <= 140.0) {
        return {types::board_support_common::Ampacity::A_63_3ph_70_1ph};
    }
    return {types::board_support_common::Ampacity::None};
}

} // namespace module
