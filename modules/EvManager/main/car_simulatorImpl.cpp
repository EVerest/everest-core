// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "car_simulatorImpl.hpp"
#include "constants.hpp"
#include "simulation_command.hpp"
#include <everest/logging.hpp>

namespace module::main {

void car_simulatorImpl::init() {
    loop_interval_ms = constants::DEFAULT_LOOP_INTERVAL_MS;
    register_all_commands();
    subscribe_to_variables_on_init();

    car_simulation = std::make_unique<CarSimulation>(mod->r_ev_board_support, mod->r_ev, mod->r_slac);

    std::thread(&car_simulatorImpl::run, this).detach();
}

void car_simulatorImpl::ready() {
    subscribe_to_external_mqtt();

    setup_ev_parameters();

    if (mod->config.auto_enable) {
        auto enable_copy = mod->config.auto_enable;
        handle_enable(enable_copy);
    }
    if (mod->config.auto_exec) {
        auto value_copy = mod->config.auto_exec_commands;
        handle_execute_charging_session(value_copy);
    }
}

void car_simulatorImpl::handle_enable(bool& value) {
    if (enabled == value) {
        // ignore if value is the same
        EVLOG_warning << "Enabled value didn't change, ignoring enable!";
        return;
    }

    reset_car_simulation_defaults();

    call_ev_board_support_functions();

    enabled = value;

    mod->r_ev_board_support->call_enable(value);
    publish_enabled(value);
}

void car_simulatorImpl::handle_execute_charging_session(std::string& value) {
    if (!check_can_execute()) {
        return;
    }

    set_execution_active(false);
    reset_car_simulation_defaults();

    update_command_queue(value);

    const std::lock_guard<std::mutex> lock{car_simulation_mutex};
    if (!command_queue.empty()) {
        set_execution_active(true);
    }
}

void car_simulatorImpl::handle_modify_charging_session(std::string& value) {
    if (!enabled) {
        EVLOG_warning << "Simulation disabled, cannot execute charging simulation.";
        return;
    }

    set_execution_active(false);

    update_command_queue(value);

    const std::lock_guard<std::mutex> lock{car_simulation_mutex};
    if (!command_queue.empty()) {
        set_execution_active(true);
    }
}

void car_simulatorImpl::run() {
    while (true) {
        if (enabled == true && execution_active == true) {

            const auto finished = run_simulation_loop();

            auto& modify_session_cmds = car_simulation->get_modify_charging_session_cmds();
            if (modify_session_cmds.has_value()) {
                auto cmds = modify_session_cmds.value();
                handle_modify_charging_session(cmds);
                modify_session_cmds.reset();
            }

            if (finished) {
                EVLOG_info << "Finished simulation.";
                set_execution_active(false);

                reset_car_simulation_defaults();

                // If we have auto_exec_infinite configured, restart simulation when it is done
                if (mod->config.auto_exec && mod->config.auto_exec_infinite) {
                    auto value_copy = mod->config.auto_exec_commands;
                    handle_execute_charging_session(value_copy);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(loop_interval_ms));
    }
}

void car_simulatorImpl::register_all_commands() {
    command_registry = std::make_unique<CommandRegistry>();

    command_registry->register_command("sleep", 1, [this](const CmdArguments& arguments) {
        return this->car_simulation->sleep(arguments, loop_interval_ms);
    });
    command_registry->register_command("iec_wait_pwr_ready", 0, [this](const CmdArguments& arguments) {
        return this->car_simulation->iec_wait_pwr_ready(arguments);
    });
    command_registry->register_command("iso_wait_pwm_is_running", 0, [this](const CmdArguments& arguments) {
        return this->car_simulation->iso_wait_pwm_is_running(arguments);
    });
    command_registry->register_command("draw_power_regulated", 2, [this](const CmdArguments& arguments) {
        return this->car_simulation->draw_power_regulated(arguments);
    });
    command_registry->register_command("draw_power_fixed", 2, [this](const CmdArguments& arguments) {
        return this->car_simulation->draw_power_fixed(arguments);
    });
    command_registry->register_command(
        "pause", 0, [this](const CmdArguments& arguments) { return this->car_simulation->pause(arguments); });
    command_registry->register_command(
        "unplug", 0, [this](const CmdArguments& arguments) { return this->car_simulation->unplug(arguments); });
    command_registry->register_command(
        "error_e", 0, [this](const CmdArguments& arguments) { return this->car_simulation->error_e(arguments); });
    command_registry->register_command(
        "diode_fail", 0, [this](const CmdArguments& arguments) { return this->car_simulation->diode_fail(arguments); });
    command_registry->register_command("rcd_current", 1, [this](const CmdArguments& arguments) {
        return this->car_simulation->rcd_current(arguments);
    });
    command_registry->register_command("iso_draw_power_regulated", 2, [this](const CmdArguments& arguments) {
        return this->car_simulation->iso_draw_power_regulated(arguments);
    });
    command_registry->register_command("wait_for_real_plugin", 0, [this](const CmdArguments& arguments) {
        return this->car_simulation->wait_for_real_plugin(arguments);
    });

    if (!mod->r_slac.empty()) {
        command_registry->register_command("iso_wait_slac_matched", 0, [this](const CmdArguments& arguments) {
            return this->car_simulation->iso_wait_slac_matched(arguments);
        });
    }

    if (!mod->r_ev.empty()) {
        command_registry->register_command("iso_wait_pwr_ready", 0, [this](const CmdArguments& arguments) {
            return this->car_simulation->iso_wait_pwr_ready(arguments);
        });
        command_registry->register_command("iso_dc_power_on", 0, [this](const CmdArguments& arguments) {
            return this->car_simulation->iso_dc_power_on(arguments);
        });
        command_registry->register_command("iso_start_v2g_session", 1, [this](const CmdArguments& arguments) {
            return this->car_simulation->iso_start_v2g_session(arguments, mod->config.three_phases);
        });
        command_registry->register_command("iso_stop_charging", 0, [this](const CmdArguments& arguments) {
            return this->car_simulation->iso_stop_charging(arguments);
        });
        command_registry->register_command("iso_wait_for_stop", 1, [this](const CmdArguments& arguments) {
            return this->car_simulation->iso_wait_for_stop(arguments, loop_interval_ms);
        });
        command_registry->register_command("iso_wait_v2g_session_stopped", 0, [this](const CmdArguments& arguments) {
            return this->car_simulation->iso_wait_v2g_session_stopped(arguments);
        });
        command_registry->register_command("iso_pause_charging", 0, [this](const CmdArguments& arguments) {
            return this->car_simulation->iso_pause_charging(arguments);
        });
        command_registry->register_command("iso_wait_for_resume", 0, [this](const CmdArguments& arguments) {
            return this->car_simulation->iso_wait_for_resume(arguments);
        });
        command_registry->register_command("iso_start_bcb_toggle", 1, [this](const CmdArguments& arguments) {
            return this->car_simulation->iso_start_bcb_toggle(arguments);
        });
    }
}

bool car_simulatorImpl::run_simulation_loop() {
    // Execute sim commands until a command blocks, or we are finished
    const std::lock_guard<std::mutex> lock{car_simulation_mutex};
    while (execution_active && !command_queue.empty()) {
        auto& current_command = command_queue.front();

        auto command_blocked = false;

        try {
            command_blocked = !current_command.execute();
        } catch (const std::exception& e) {
            EVLOG_error << e.what();
        }

        if (!command_blocked) {
            command_queue.pop();
        } else {
            break; // command blocked, wait for timer to run this function again
        }
    }

    car_simulation->state_machine();

    if (command_queue.empty()) {
        return true;
    }
    return false;
}

bool car_simulatorImpl::check_can_execute() {
    if (!enabled) {
        EVLOG_warning << "Simulation disabled, cannot execute charging simulation.";
        return false;
    }
    if (execution_active) {
        EVLOG_warning << "Execution of charging session simulation already running, cannot start new one.";
        return false;
    }

    return true;
}

void car_simulatorImpl::subscribe_to_variables_on_init() {
    // subscribe bsp_event
    const std::lock_guard<std::mutex> lock{car_simulation_mutex};
    using types::board_support_common::BspEvent;
    mod->r_ev_board_support->subscribe_bsp_event([this](const auto& bsp_event) {
        car_simulation->set_bsp_event(bsp_event.event);
        if (bsp_event.event == types::board_support_common::Event::Disconnected &&
            car_simulation->get_state() != SimState::UNPLUGGED) {
            set_execution_active(false);
            car_simulation->set_state(SimState::UNPLUGGED);
        }
    });

    // subscribe bsp_measurement
    using types::board_support_common::BspMeasurement;
    mod->r_ev_board_support->subscribe_bsp_measurement([this](const auto& measurement) {
        car_simulation->set_pp(measurement.proximity_pilot.ampacity);
        car_simulation->set_pwm_duty_cycle(measurement.cp_pwm_duty_cycle);
        if (measurement.rcd_current_mA.has_value()) {
            car_simulation->set_rcd_current(measurement.rcd_current_mA.value());
        }
    });

    // subscribe slac_state
    if (!mod->r_slac.empty()) {
        const auto& slac = mod->r_slac.at(0);
        slac->subscribe_state([this](const auto& state) { car_simulation->set_slac_state(state); });
    }

    // subscribe ev events
    if (!mod->r_ev.empty()) {
        const auto& _ev = mod->r_ev.at(0);
        _ev->subscribe_AC_EVPowerReady([this](auto value) { car_simulation->set_iso_pwr_ready(value); });
        _ev->subscribe_AC_EVSEMaxCurrent([this](auto value) { car_simulation->set_evse_max_current(value); });
        _ev->subscribe_AC_StopFromCharger([this]() { car_simulation->set_iso_stopped(true); });
        _ev->subscribe_V2G_Session_Finished([this]() { car_simulation->set_v2g_finished(true); });
        _ev->subscribe_DC_PowerOn([this]() { car_simulation->set_dc_power_on(true); });
        _ev->subscribe_pause_from_charger([this]() { car_simulation->set_iso_d20_paused(true); });
    }
}

void car_simulatorImpl::setup_ev_parameters() {
    if (!mod->r_ev.empty()) {
        mod->r_ev[0]->call_set_dc_params({mod->config.dc_max_current_limit, mod->config.dc_max_power_limit,
                                          mod->config.dc_max_voltage_limit, mod->config.dc_energy_capacity,
                                          mod->config.dc_target_current, mod->config.dc_target_voltage});
        if (mod->config.support_sae_j2847) {
            mod->r_ev[0]->call_enable_sae_j2847_v2g_v2h();
            mod->r_ev[0]->call_set_bpt_dc_params(
                {mod->config.dc_discharge_max_current_limit, mod->config.dc_discharge_max_power_limit,
                 mod->config.dc_discharge_target_current, mod->config.dc_discharge_v2g_minimal_soc});
        }
    }
}

void car_simulatorImpl::call_ev_board_support_functions() {
    mod->r_ev_board_support->call_allow_power_on(false);

    mod->r_ev_board_support->call_set_ac_max_current(mod->config.max_current);
    mod->r_ev_board_support->call_set_three_phases(mod->config.three_phases);
}

void car_simulatorImpl::subscribe_to_external_mqtt() {
    const auto& mqtt = mod->mqtt;
    mqtt.subscribe("everest_external/nodered/" + std::to_string(mod->config.connector_id) + "/carsim/cmd/enable",
                   [this](const std::string& message) {
                       auto enable = (message == "true") ? true : false;
                       handle_enable(enable);
                   });
    mqtt.subscribe("everest_external/nodered/" + std::to_string(mod->config.connector_id) +
                       "/carsim/cmd/execute_charging_session",
                   [this](const auto data) {
                       auto data_copy{data};
                       handle_execute_charging_session(data_copy);
                   });
    mqtt.subscribe("everest_external/nodered/" + std::to_string(mod->config.connector_id) +
                       "/carsim/cmd/modify_charging_session",
                   [this](auto data) {
                       auto data_copy = data;
                       handle_modify_charging_session(data_copy);
                   });
}

void car_simulatorImpl::reset_car_simulation_defaults() {
    const std::lock_guard<std::mutex> lock{car_simulation_mutex};
    car_simulation->reset();
}

void car_simulatorImpl::update_command_queue(std::string& value) {
    const std::lock_guard<std::mutex> lock{car_simulation_mutex};
    command_queue = SimulationCommand::parse_sim_commands(value, *command_registry);
}

void car_simulatorImpl::set_execution_active(bool value) {
    execution_active = value;
}

} // namespace module::main
