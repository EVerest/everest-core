// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "error_handler.hpp"
#include "state.hpp"
#include "util.hpp"
#include <cmath>

namespace module {

template <typename YetiSimulatorT, typename BoardSupportT> class Simulator {
public:
    Simulator(int new_sleep_time_ms, YetiSimulatorT* new_yeti_simulator) {
        start(new_sleep_time_ms, new_yeti_simulator);
    }

private:
    void start(int sleep_time_ms, YetiSimulatorT* yeti_simulator) {
        std::thread(&Simulator::run_simulation, this, sleep_time_ms, yeti_simulator).detach();
    }

    [[noreturn]] void run_simulation(int sleep_time_ms, YetiSimulatorT* yeti_simulator) {
        auto& module_state = yeti_simulator->get_module_state();
        while (true) {
            if (module_state.simulation_enabled) {
                simulation_step(yeti_simulator);
            }

            module_state.pubCnt++;
            switch (module_state.pubCnt) {
            case 1:
                publish_powermeter(yeti_simulator);
                publish_telemetry(yeti_simulator);
                break;
            case 3:
                publish_keepalive(yeti_simulator);
                break;
            default:
                module_state.pubCnt = 0;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds{sleep_time_ms});
        }
    }

    void simulation_step(YetiSimulatorT* yeti_simulator) {
        check_error_rcd(yeti_simulator);
        read_from_car(yeti_simulator);
        simulation_statemachine(yeti_simulator);
        add_noise(yeti_simulator);
        simulate_powermeter(yeti_simulator);
        publish_ev_board_support(yeti_simulator);
    }

    void check_error_rcd(YetiSimulatorT* yeti_simulator) {
        using Everest::error::Severity;
        auto& module_state = yeti_simulator->get_module_state();
        auto& board_support = yeti_simulator->get_board_support();
        auto& rcd = yeti_simulator->get_ac_rcd();

        if (module_state.simulation_data.rcd_current > 5.0) {
            if (!module_state.rcd_error_reported) {
                const auto error =
                    board_support.error_factory->create_error("ac_rcd/DC", "", "Simulated fault event", Severity::High);
                board_support.raise_error(error);
                module_state.rcd_error_reported = true;
            }
        } else {
            if (module_state.rcd_error_reported) {
                board_support.clear_error("ac_rcd/DC");
            }
            module_state.rcd_error_reported = false;
        }
        rcd.publish_rcd_current_mA(module_state.simulation_data.rcd_current);
    }

    void read_from_car(YetiSimulatorT* yeti_simulator) {
        auto error_handler = ErrorHandler{&yeti_simulator->get_board_support(), &yeti_simulator->get_ac_rcd(),
                                          &yeti_simulator->get_connector_lock()};

        auto& module_state = yeti_simulator->get_module_state();

        auto amps1 = double{};
        auto amps2 = double{};
        auto amps3 = double{};

        auto hlc_active = false;
        if (module_state.pwm_duty_cycle >= 0.03 && module_state.pwm_duty_cycle <= 0.07)
            hlc_active = true;

        auto amps = duty_cycle_to_amps(module_state.pwm_duty_cycle);
        if (amps > module_state.ev_max_current || hlc_active == true)
            amps = module_state.ev_max_current;

        if (module_state.relais_on == true && module_state.ev_three_phases > 0)
            amps1 = amps;
        else
            amps1 = 0;
        if (module_state.relais_on == true && module_state.ev_three_phases > 1 &&
            module_state.use_three_phases_confirmed)
            amps2 = amps;
        else
            amps2 = 0;
        if (module_state.relais_on == true && module_state.ev_three_phases > 2 &&
            module_state.use_three_phases_confirmed)
            amps3 = amps;
        else
            amps3 = 0;

        if (module_state.pwm_running) {
            module_state.pwm_voltage_hi = module_state.simulation_data.cp_voltage;
            module_state.pwm_voltage_lo = -12.0;
        } else {
            module_state.pwm_voltage_hi = module_state.simulation_data.cp_voltage;
            module_state.pwm_voltage_lo = module_state.pwm_voltage_hi;
        }

        if (module_state.pwm_error_f) {
            module_state.pwm_voltage_hi = -12.0;
            module_state.pwm_voltage_lo = -12.0;
        }
        if (module_state.simulation_data.error_e) {
            module_state.pwm_voltage_hi = 0.0;
            module_state.pwm_voltage_lo = 0.0;
        }
        if (module_state.simulation_data.diode_fail) {
            module_state.pwm_voltage_lo = -module_state.pwm_voltage_hi;
        }

        const auto cpLo = module_state.pwm_voltage_lo;
        const auto cpHi = module_state.pwm_voltage_hi;

        // sth is wrong with negative signal
        if (module_state.pwm_running && !is_voltage_in_range(cpLo, -12.0)) {
            // CP-PE short or signal somehow gone
            if (is_voltage_in_range(cpLo, 0.0) && is_voltage_in_range(cpHi, 0.0)) {
                module_state.current_state = state::State::STATE_E;
                drawPower(module_state, 0, 0, 0, 0);
            } else if (is_voltage_in_range(cpHi + cpLo, 0.0)) { // Diode fault
                error_handler.error_DiodeFault(true);
                drawPower(module_state, 0, 0, 0, 0);
            }
        } else if (is_voltage_in_range(cpHi, 12.0)) {
            // +12V State A IDLE (open circuit)
            // clear all errors that clear on disconnection
            clear_disconnect_errors(error_handler, yeti_simulator->get_board_support());
            module_state.current_state = state::State::STATE_A;
            drawPower(module_state, 0, 0, 0, 0);
        } else if (is_voltage_in_range(cpHi, 9.0)) {
            module_state.current_state = state::State::STATE_B;
            drawPower(module_state, 0, 0, 0, 0);
        } else if (is_voltage_in_range(cpHi, 6.0)) {
            module_state.current_state = state::State::STATE_C;
            drawPower(module_state, amps1, amps2, amps3, 0.2);
        } else if (is_voltage_in_range(cpHi, 3.0)) {
            module_state.current_state = state::State::STATE_D;
            drawPower(module_state, amps1, amps2, amps3, 0.2);
        } else if (is_voltage_in_range(cpHi, -12.0)) {
            module_state.current_state = state::State::STATE_F;
            drawPower(module_state, 0, 0, 0, 0);
        }
    }

    void simulation_statemachine(YetiSimulatorT* yeti_simulator) {
        auto& module_state = yeti_simulator->get_module_state();
        auto& board_support = yeti_simulator->get_board_support();

        if (module_state.last_state != module_state.current_state) {
            publish_event(board_support, module_state.current_state);
        }

        switch (module_state.current_state) {
        case state::State::STATE_DISABLED:
            powerOff(module_state, board_support);
            module_state.power_on_allowed = false;
            break;

        case state::State::STATE_A: {
            module_state.use_three_phases_confirmed = module_state.use_three_phases;
            auto* _board_support = dynamic_cast<BoardSupportT*>(&board_support);
            _board_support->pwm_off();
            reset_powermeter(module_state);
            module_state.simplified_mode = false;

            if (module_state.last_state != state::State::STATE_A &&
                module_state.last_state != state::State::STATE_DISABLED &&
                module_state.last_state != state::State::STATE_F) {
                powerOff(module_state, board_support);

                // If car was unplugged, reset RCD flag.
                module_state.simdata_setting.rcd_current = 0.1;
                module_state.rcd_error = false;
            }
            break;
        }
        case state::State::STATE_B:
            // Table A.6: Sequence 7 EV stops charging
            // Table A.6: Sequence 8.2 EV supply equipment
            // responds to EV opens S2 (w/o PWM)

            if (module_state.last_state != state::State::STATE_A && module_state.last_state != state::State::STATE_B) {
                // Need to switch off according to Table A.6 Sequence 8.1 within
                powerOff(module_state, board_support);
            }

            // Table A.6: Sequence 1.1 Plug-in
            if (module_state.last_state == state::State::STATE_A) {
                module_state.simplified_mode = false;
            }

            break;
        case state::State::STATE_C:
            // Table A.6: Sequence 1.2 Plug-in
            if (module_state.last_state == state::State::STATE_A) {
                module_state.simplified_mode = true;
            }

            if (!module_state.pwm_running) { // C1
                // Table A.6 Sequence 10.2: EV does not stop drawing power even
                // if PWM stops. Stop within 6 seconds (E.g. Kona1!)
                // This is implemented in EvseManager
                if (!module_state.power_on_allowed)
                    powerOff(module_state, board_support);
            } else if (module_state.power_on_allowed) { // C2
                // Table A.6: Sequence 4 EV ready to charge.
                // Must enable power within 3 seconds.
                powerOn(module_state, board_support);
            }
            break;
        case state::State::STATE_D:
            // Table A.6: Sequence 1.2 Plug-in (w/ventilation)
            if (module_state.last_state == state::State::STATE_A) {
                module_state.simplified_mode = true;
            }

            if (!module_state.pwm_running) {
                // Table A.6 Sequence 10.2: EV does not stop drawing power
                // even if PWM stops. Stop within 6 seconds (E.g. Kona1!)
                /* if (mod.last_pwm_running) // FIMXE implement 6 second timer
                    startTimer(6000);
                if (timerElapsed()) { */
                // force power off under load
                powerOff(module_state, board_support);
                // }
            } else if (module_state.power_on_allowed && !module_state.relais_on && module_state.has_ventilation) {
                // Table A.6: Sequence 4 EV ready to charge.
                // Must enable power within 3 seconds.
                powerOn(module_state, board_support);
            }
            break;
        case state::State::STATE_E: {
            powerOff(module_state, board_support);
            auto* _board_support = dynamic_cast<BoardSupportT*>(&board_support);
            _board_support->pwm_off();
            break;
        }
        case state::State::STATE_F:
            powerOff(module_state, board_support);
            break;
        default:
            break;
        }
        module_state.last_state = module_state.current_state;
        module_state.last_pwm_running = module_state.pwm_running;
    }

    void add_noise(YetiSimulatorT* yeti_simulator) {
        auto& module_state = yeti_simulator->get_module_state();

        const auto random_number_between_0_and_1 = []() {
            return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
        };
        const auto noise = (1 + (random_number_between_0_and_1() - 0.5) * 0.02);
        const auto lonoise = (1 + (random_number_between_0_and_1() - 0.5) * 0.005);
        const auto impedance = module_state.simdata_setting.impedance / 1000.0;

        module_state.simulation_data.currents.L1 = module_state.simdata_setting.currents.L1 * noise;
        module_state.simulation_data.currents.L2 = module_state.simdata_setting.currents.L2 * noise;
        module_state.simulation_data.currents.L3 = module_state.simdata_setting.currents.L3 * noise;
        module_state.simulation_data.currents.N = module_state.simdata_setting.currents.N * noise;

        module_state.simulation_data.voltages.L1 =
            module_state.simdata_setting.voltages.L1 * noise - impedance * module_state.simulation_data.currents.L1;
        module_state.simulation_data.voltages.L2 =
            module_state.simdata_setting.voltages.L2 * noise - impedance * module_state.simulation_data.currents.L2;
        module_state.simulation_data.voltages.L3 =
            module_state.simdata_setting.voltages.L3 * noise - impedance * module_state.simulation_data.currents.L3;

        module_state.simulation_data.frequencies.L1 = module_state.simdata_setting.frequencies.L1 * lonoise;
        module_state.simulation_data.frequencies.L2 = module_state.simdata_setting.frequencies.L2 * lonoise;
        module_state.simulation_data.frequencies.L3 = module_state.simdata_setting.frequencies.L3 * lonoise;

        module_state.simulation_data.cp_voltage = module_state.simdata_setting.cp_voltage * noise;
        module_state.simulation_data.rcd_current = module_state.simdata_setting.rcd_current * noise;
        module_state.simulation_data.pp_resistor = module_state.simdata_setting.pp_resistor * noise;

        module_state.simulation_data.diode_fail = module_state.simdata_setting.diode_fail;
        module_state.simulation_data.error_e = module_state.simdata_setting.error_e;
    }

    void simulate_powermeter(YetiSimulatorT* yeti_simulator) {
        using namespace std::chrono;

        auto& module_state = yeti_simulator->get_module_state();

        const auto time_stamp = time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count();
        if (module_state.powermeter_sim_last_time_stamp == 0)
            module_state.powermeter_sim_last_time_stamp = time_stamp;
        const auto deltat = time_stamp - module_state.powermeter_sim_last_time_stamp;
        module_state.powermeter_sim_last_time_stamp = time_stamp;

        const auto wattL1 = module_state.simulation_data.voltages.L1 * module_state.simulation_data.currents.L1 *
                            (module_state.relais_on ? 1 : 0);
        const auto wattL2 = module_state.simulation_data.voltages.L2 * module_state.simulation_data.currents.L2 *
                            (module_state.relais_on && module_state.use_three_phases_confirmed ? 1 : 0);
        const auto wattL3 = module_state.simulation_data.voltages.L3 * module_state.simulation_data.currents.L3 *
                            (module_state.relais_on && module_state.use_three_phases_confirmed ? 1 : 0);

        module_state.watt_hr.L1 += (wattL1 * deltat) / 1000.0 / 3600.0;
        module_state.watt_hr.L2 += (wattL2 * deltat) / 1000.0 / 3600.0;
        module_state.watt_hr.L3 += (wattL3 * deltat) / 1000.0 / 3600.0;

        module_state.powermeter_data.time_stamp = round(time_stamp / 1000);
        module_state.powermeter_data.totalWattHr =
            round(module_state.watt_hr.L1 + module_state.watt_hr.L2 + module_state.watt_hr.L3);

        module_state.powermeter_data.wattL1 = round(wattL1);
        module_state.powermeter_data.vrmsL1 = module_state.simulation_data.voltages.L1;
        module_state.powermeter_data.irmsL1 = module_state.simulation_data.currents.L1;
        module_state.powermeter_data.wattHrL1 = round(module_state.watt_hr.L1);
        module_state.powermeter_data.tempL1 = 25.0 + (wattL1 + wattL2 + wattL3) * 0.003;
        module_state.powermeter_data.freqL1 = module_state.simulation_data.frequencies.L1;

        module_state.powermeter_data.wattL2 = round(wattL2);
        module_state.powermeter_data.vrmsL2 = module_state.simulation_data.voltages.L2;
        module_state.powermeter_data.irmsL2 = module_state.simulation_data.currents.L1;
        module_state.powermeter_data.wattHrL2 = round(module_state.watt_hr.L2);
        module_state.powermeter_data.tempL2 = 25.0 + (wattL1 + wattL2 + wattL3) * 0.003;
        module_state.powermeter_data.freqL2 = module_state.simulation_data.frequencies.L2;

        module_state.powermeter_data.wattL3 = round(wattL3);
        module_state.powermeter_data.vrmsL3 = module_state.simulation_data.voltages.L3;
        module_state.powermeter_data.irmsL3 = module_state.simulation_data.currents.L3;
        module_state.powermeter_data.wattHrL3 = round(module_state.watt_hr.L3);
        module_state.powermeter_data.tempL3 = 25.0 + (wattL1 + wattL2 + wattL3) * 0.003;
        module_state.powermeter_data.freqL3 = module_state.simulation_data.frequencies.L3;

        module_state.powermeter_data.irmsN = module_state.simulation_data.currents.N;
    };

    void publish_ev_board_support(YetiSimulatorT* yeti_simulator) {
        auto& module_state = yeti_simulator->get_module_state();
        auto& ev_board_support = yeti_simulator->get_ev_board_support();

        const auto pp = read_pp_ampacity(module_state);

        ev_board_support.publish_bsp_measurement(
            {.proximity_pilot = pp,
             .cp_pwm_duty_cycle = static_cast<float>(module_state.pwm_duty_cycle * 100),
             .rcd_current_mA = module_state.simulation_data.rcd_current});
    }

    void publish_powermeter(YetiSimulatorT* yeti_simulator) {
        auto& module_state = yeti_simulator->get_module_state();
        auto& powermeter = yeti_simulator->get_powermeter();
        auto& mqtt = yeti_simulator->get_mqtt();
        auto& info = yeti_simulator->get_info();

        powermeter.publish_powermeter(power_meter_external(module_state.powermeter_data));

        // Deprecated external stuff
        mqtt.publish("/external/powermeter/vrmsL1", module_state.powermeter_data.vrmsL1);
        mqtt.publish("/external/powermeter/phaseSeqError", false);
        mqtt.publish("/external/powermeter/time_stamp", std::to_string(module_state.powermeter_data.time_stamp));
        mqtt.publish("/external/powermeter/tempL1", module_state.powermeter_data.tempL1);
        mqtt.publish("/external/powermeter/totalKw",
                     (module_state.powermeter_data.wattL1 + module_state.powermeter_data.wattL2 +
                      module_state.powermeter_data.wattL3) /
                         1000.0);
        mqtt.publish("/external/powermeter/totalKWattHr",
                     (module_state.powermeter_data.wattHrL1 + module_state.powermeter_data.wattHrL2 +
                      module_state.powermeter_data.wattHrL3) /
                         1000.0);
        mqtt.publish("/external/powermeter_json", json{module_state.powermeter_data}.dump());

        mqtt.publish("/external/" + info.id + "/powermeter/tempL1", module_state.powermeter_data.tempL1);
        mqtt.publish("/external/" + info.id + "/powermeter/totalKw",
                     (module_state.powermeter_data.wattL1 + module_state.powermeter_data.wattL2 +
                      module_state.powermeter_data.wattL3) /
                         1000.0);
        mqtt.publish("/external/" + info.id + "/powermeter/totalKWattHr",
                     (module_state.powermeter_data.wattHrL1 + module_state.powermeter_data.wattHrL2 +
                      module_state.powermeter_data.wattHrL3) /
                         1000.0);
    }

    void publish_telemetry(YetiSimulatorT* yeti_simulator) {
        auto& board_support = yeti_simulator->get_board_support();
        const auto& module_state = yeti_simulator->get_module_state();

        board_support.publish_telemetry({.evse_temperature_C = static_cast<float>(module_state.powermeter_data.tempL1),
                                         .fan_rpm = 1500.,
                                         .supply_voltage_12V = 12.01,
                                         .supply_voltage_minus_12V = -11.8,
                                         .relais_on = module_state.relais_on});
    }

    void publish_keepalive(YetiSimulatorT* yeti_simulator) {
        using namespace std::chrono;
        auto& mqtt = yeti_simulator->get_mqtt();

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

    types::powermeter::Powermeter power_meter_external(state::PowermeterData& powermeter_data) {
        const auto current_iso_time_string = util::get_current_iso_time_string();
        const auto& p = powermeter_data;
        return {
            .timestamp = current_iso_time_string,
            .energy_Wh_import =
                {
                    .total = static_cast<float>(p.totalWattHr),
                    .L1 = static_cast<float>(p.wattHrL1),
                    .L2 = static_cast<float>(p.wattHrL2),
                    .L3 = static_cast<float>(p.wattHrL3),
                },
            .meter_id = "YETI_POWERMETER",
            .phase_seq_error = false,
            .power_W =
                types::units::Power{
                    .total = static_cast<float>(p.wattL1 + p.wattL2 + p.wattL3),
                    .L1 = static_cast<float>(p.wattL1),
                    .L2 = static_cast<float>(p.wattL2),
                    .L3 = static_cast<float>(p.wattL3),
                },
            .voltage_V =
                types::units::Voltage{
                    .L1 = p.vrmsL1,
                    .L2 = p.vrmsL2,
                    .L3 = p.vrmsL3,
                },
            .current_A =
                types::units::Current{
                    .L1 = p.irmsL1,
                    .L2 = p.irmsL2,
                    .L3 = p.irmsL3,
                    .N = p.irmsN,
                },
            .frequency_Hz =
                types::units::Frequency{
                    .L1 = static_cast<float>(p.freqL1),
                    .L2 = static_cast<float>(p.freqL2),
                    .L3 = static_cast<float>(p.freqL3),
                },

        };
    }

    double duty_cycle_to_amps(double dc) {
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

    bool is_voltage_in_range(const double voltage, double center) {
        const auto interval = 1.1;
        return ((voltage > center - interval) && (voltage < center + interval));
    }

    void drawPower(state::ModuleState& module_state, int l1, int l2, int l3, int n) {
        module_state.simdata_setting.currents.L1 = l1;
        module_state.simdata_setting.currents.L2 = l2;
        module_state.simdata_setting.currents.L3 = l3;
        module_state.simdata_setting.currents.N = n;
    }

    void clear_disconnect_errors(ErrorHandler& error_handler, const evse_board_supportImplBase& board_support) {
        if (board_support.error_state_monitor->is_error_active("evse_board_support/DiodeFault", "")) {
            error_handler.error_DiodeFault(false);
        }
    }

    void powerOn(state::ModuleState& module_state, evse_board_supportImplBase& board_support) {
        if (!module_state.relais_on) {
            publish_event(board_support, state::State::Event_PowerOn);
            module_state.relais_on = true;
            module_state.telemetry_data.power_switch.switching_count += 1;
        }
    }

    void powerOff(state::ModuleState& module_state, evse_board_supportImplBase& board_support) {
        if (module_state.relais_on) {
            publish_event(board_support, state::State::Event_PowerOff);
            module_state.telemetry_data.power_switch.switching_count++;
            module_state.relais_on = false;
        }
    }

    // NOLINTNEXTLINE
    void publish_event(evse_board_supportImplBase& board_support, state::State event) {
        board_support.publish_event(event_to_enum(event));
    }

    // NOLINTNEXTLINE
    static types::board_support_common::BspEvent event_to_enum(state::State event) {
        using state::State;
        using types::board_support_common::Event;

        switch (event) {
        case State::STATE_A:
            return {.event = Event::A};
        case State::STATE_B:
            return {.event = Event::B};
        case State::STATE_C:
            return {.event = Event::C};
        case State::STATE_D:
            return {.event = Event::D};
        case State::STATE_E:
            return {.event = Event::E};
        case State::STATE_F:
            return {.event = Event::F};
        case State::STATE_DISABLED:
            return {.event = Event::F};
        case State::Event_PowerOn:
            return {.event = Event::PowerOn};
        case State::Event_PowerOff:
            return {.event = Event::PowerOff};
        default:
            EVLOG_error << "Invalid event : " << event_to_string(event);
            return {.event = Event::Disconnected}; // TODO: correct default value
        }
    }

    // NOLINTNEXTLINE
    static std::string event_to_string(state::State state) {
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

    void reset_powermeter(state::ModuleState& module_state) {
        module_state.watt_hr.L1 = 0;
        module_state.watt_hr.L2 = 0;
        module_state.watt_hr.L3 = 0;
        module_state.powermeter_sim_last_time_stamp = 0;
    }

    types::board_support_common::ProximityPilot read_pp_ampacity(state::ModuleState& module_state) {
        const auto pp_resistor = module_state.simdata_setting.pp_resistor;

        if (pp_resistor < 80.0 || pp_resistor > 2460) {
            EVLOG_error << "PP resistor value " << pp_resistor << " Ohm seems to be outside the allowed range.";
            return {.ampacity = types::board_support_common::Ampacity::None};
        }

        // PP resistor value in spec, use a conservative interpretation of the resistance ranges
        if (pp_resistor > 936.0 && pp_resistor <= 2460.0) {
            return {.ampacity = types::board_support_common::Ampacity::A_13};
        }
        if (pp_resistor > 308.0 && pp_resistor <= 936.0) {
            return {.ampacity = types::board_support_common::Ampacity::A_20};
        }
        if (pp_resistor > 140.0 && pp_resistor <= 308.0) {
            return {.ampacity = types::board_support_common::Ampacity::A_32};
        }
        if (pp_resistor > 80.0 && pp_resistor <= 140.0) {
            return {.ampacity = types::board_support_common::Ampacity::A_63_3ph_70_1ph};
        }
        return {.ampacity = types::board_support_common::Ampacity::None};
    };
};
} // namespace module
