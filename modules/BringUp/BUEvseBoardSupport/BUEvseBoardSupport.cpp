// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "BUEvseBoardSupport.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/table.hpp"
#include "generated/types/evse_manager.hpp"

using namespace ftxui;

namespace module {

static std::vector<std::vector<std::string>> to_string(types::evse_board_support::HardwareCapabilities c) {
    std::vector<std::vector<std::string>> hw_caps;
    hw_caps.push_back(
        {"Max/Min Current", fmt::format("Imp: {}A/{}A Exp: {}A/{}A", c.max_current_A_import, c.min_current_A_import,
                                        c.max_current_A_export, c.min_current_A_export)});
    hw_caps.push_back({"Max/Min Phase Count",
                       fmt::format("Imp: {}ph/{}ph Exp: {}ph/{}ph", c.max_phase_count_import, c.min_phase_count_import,
                                   c.max_phase_count_export, c.min_phase_count_export)});
    hw_caps.push_back({"Connector Type", types::evse_board_support::connector_type_to_string(c.connector_type)});
    hw_caps.push_back({"CP State E Support", c.supports_cp_state_E ? "Yes" : "No"});
    if (c.max_plug_temperature_C.has_value()) {
        hw_caps.push_back({"Max plug temperature", fmt::format("{}C", c.max_plug_temperature_C.value())});
    }

    return hw_caps;
}

void BUEvseBoardSupport::init() {
    invoke_init(*p_main);
}

void BUEvseBoardSupport::ready() {
    invoke_ready(*p_main);

    auto screen = ScreenInteractive::Fullscreen();

    r_bsp->subscribe_event([this, &screen](const types::board_support_common::BspEvent e) {
        {
            std::scoped_lock lock(data_mutex);
            switch (e.event) {
            case types::board_support_common::Event::A:
                cp_state = "A";
                break;
            case types::board_support_common::Event::B:
                cp_state = "B";
                break;
            case types::board_support_common::Event::C:
                cp_state = "C";
                break;
            case types::board_support_common::Event::D:
                cp_state = "D";
                break;
            case types::board_support_common::Event::E:
                cp_state = "E";
                break;
            case types::board_support_common::Event::F:
                cp_state = "F";
                break;
            case types::board_support_common::Event::PowerOn:
                relais_feedback = fmt::format("PowerOn (after {} ms)",
                                              std::chrono::duration_cast<std::chrono::milliseconds>(
                                                  std::chrono::steady_clock::now() - last_allow_power_on_time_point)
                                                  .count());
                break;
            case types::board_support_common::Event::PowerOff:
                relais_feedback = fmt::format("PowerOff (after {} ms)",
                                              std::chrono::duration_cast<std::chrono::milliseconds>(
                                                  std::chrono::steady_clock::now() - last_allow_power_on_time_point)
                                                  .count());

                break;
            }
        }
        screen.Post(Event::Custom);
    });

    r_bsp->subscribe_capabilities([this, &screen](const types::evse_board_support::HardwareCapabilities c) {
        {
            std::scoped_lock lock(data_mutex);
            hw_caps = to_string(c);
        }
        screen.Post(Event::Custom);
    });

    r_bsp->subscribe_telemetry([this, &screen](const types::evse_board_support::Telemetry t) {
        {
            std::scoped_lock lock(data_mutex);
            telemetry = fmt::format("EVSE {}C Fan {}rpm Supply {}V/{}V, Relais {}", t.evse_temperature_C, t.fan_rpm,
                                    t.supply_voltage_12V, t.supply_voltage_minus_12V, t.relais_on);

            if (t.plug_temperature_C.has_value()) {
                telemetry = telemetry += fmt::format(" Plug {}C ", t.plug_temperature_C.value());
            }
        }
        screen.Post(Event::Custom);
    });

    r_bsp->subscribe_ac_pp_ampacity([this, &screen](const types::board_support_common::ProximityPilot pp) {
        {
            std::scoped_lock lock(data_mutex);
            proximity_pilot = types::board_support_common::ampacity_to_string(pp.ampacity);
        }
        screen.Post(Event::Custom);
    });

    r_bsp->subscribe_request_stop_transaction([this, &screen](const types::evse_manager::StopTransactionRequest t) {
        {
            std::scoped_lock lock(data_mutex);
            stop_transaction = stop_transaction_reason_to_string(t.reason);
        }
        screen.Post(Event::Custom);
    });
    // telemetry, pp (needs to be requested as well)

    auto error_handler = [this, &screen](const Everest::error::Error& error) {
        {
            std::scoped_lock lock(this->data_mutex);
            this->last_error_raised = fmt::format("Fault raised: {}, {}", error.type, error.sub_type);
        }
        screen.Post(Event::Custom);
    };

    auto error_cleared_handler = [this, &screen](const Everest::error::Error& error) {
        {
            std::scoped_lock lock(this->data_mutex);
            this->last_error_cleared = fmt::format("Fault cleared: {}, {}", error.type, error.sub_type);
        }
        screen.Post(Event::Custom);
    };

    r_bsp->subscribe_all_errors(error_handler, error_cleared_handler);

    if (!r_ac_rcd.empty()) {
        r_ac_rcd.at(0)->subscribe_all_errors(error_handler, error_cleared_handler);
    }

    if (!r_lock_motor.empty()) {
        r_lock_motor.at(0)->subscribe_all_errors(error_handler, error_cleared_handler);
    }

    std::string last_command = "None";

    // ---------------------------------------------------------------------------
    // Relais allow power on
    // ---------------------------------------------------------------------------

    Component relais_on_button = Button(
        "Allow power on",
        [&] {
            last_command = "Allow power on";
            last_allow_power_on_time_point = std::chrono::steady_clock::now();
            r_bsp->call_allow_power_on({true, types::evse_board_support::Reason::FullPowerCharging});
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    Component relais_off_button = Button(
        "Force power off",
        [&] {
            last_command = "Force power off";
            last_allow_power_on_time_point = std::chrono::steady_clock::now();
            r_bsp->call_allow_power_on({false, types::evse_board_support::Reason::PowerOff});
        },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    auto relais_component = Container::Horizontal({
        Container::Vertical({
            relais_on_button,
            relais_off_button,
        }),
    });

    auto relais_renderer = Renderer(relais_component, [&] {
        auto relais_win = window(text("Relais"), relais_component->Render());
        return vbox({
                   hbox({
                       relais_win,
                   }),
               }) |
               flex_grow;
    });

    // ---------------------------------------------------------------------------
    // PWM control
    // ---------------------------------------------------------------------------

    std::string pwm_duty_cycle_str{"5.0"};

    Component cp_state_X1_button = Button(
        "CP State X1 (PWM Off)",
        [&] {
            last_command = "CP State X1 (PWM Off)";
            r_bsp->call_cp_state_X1();
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    Component pwm_on_button = Button(
        "PWM On",
        [&] {
            if (pwm_duty_cycle_str.empty())
                pwm_duty_cycle_str = "5.0";
            last_command = "PWM On (" + pwm_duty_cycle_str + ")";
            r_bsp->call_pwm_on(std::stof(pwm_duty_cycle_str.c_str()));
        },
        ButtonOption::Animated(Color::Green, Color::White, Color::GreenLight, Color::White));

    Component cp_state_E_button = Button(
        "CP State E",
        [&] {
            last_command = "CP State E";
            r_bsp->call_cp_state_E();
        },
        ButtonOption::Animated(Color::Magenta, Color::White, Color::MagentaLight, Color::White));

    Component cp_state_F_button = Button(
        "CP State F",
        [&] {
            last_command = "CP State F";
            r_bsp->call_cp_state_F();
        },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    InputOption o;
    o.multiline = false;
    o.cursor_position = 0;
    o.on_enter = [&]() {
        last_command = "Update PWM DC to " + pwm_duty_cycle_str;
        r_bsp->call_pwm_on(std::stof(pwm_duty_cycle_str.c_str()));
    };
    auto pwm_dc = Input(&pwm_duty_cycle_str, "5.0", o);
    auto pwm_component = Container::Horizontal({
        Container::Vertical({
            cp_state_E_button,
            cp_state_F_button,
            cp_state_X1_button,
            pwm_on_button,
            pwm_dc,
        }),
    });

    auto pwm_renderer = Renderer(pwm_component, [&] {
        auto pwm_win = window(text("PWM"), pwm_component->Render());
        return vbox({
                   hbox({
                       pwm_win,
                   }),
               }) |
               flex_grow;
    });

    // ---------------------------------------------------------------------------
    // Enable/Disable
    // ---------------------------------------------------------------------------

    Component enable_on_button = Button(
        "Enable",
        [&] {
            last_command = "Enable";
            r_bsp->call_enable(true);
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    Component enable_off_button = Button(
        "Disable",
        [&] {
            last_command = "Disable";
            r_bsp->call_enable(false);
        },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    auto enable_component = Container::Vertical({
        enable_on_button,
        enable_off_button,
    });

    auto enable_renderer =
        Renderer(enable_component, [&] { return window(text("Enable"), enable_component->Render()); });

    // ---------------------------------------------------------------------------
    // AC switch phases while charging
    // ---------------------------------------------------------------------------

    Component three_phase_button = Button(
        "ThreePhases",
        [&] {
            last_command = "Switch ThreePhases";
            r_bsp->call_ac_switch_three_phases_while_charging(true);
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    Component single_phase_button = Button(
        "SinglePhase",
        [&] {
            last_command = "Switch SinglePhase";
            r_bsp->call_ac_switch_three_phases_while_charging(false);
        },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    auto phase_switch_component = Container::Vertical({
        three_phase_button,
        single_phase_button,
    });

    auto phase_switch_renderer = Renderer(
        phase_switch_component, [&] { return window(text("AC Phase Switch"), phase_switch_component->Render()); });

    // ---------------------------------------------------------------------------
    // AC Overcurrent Limit command
    // ---------------------------------------------------------------------------

    std::string ac_oc_limit_str = "16.0"; // Default current limit (A)

    Component ac_oc_limit_button = Button(
        "Set Limit",
        [&] {
            if (ac_oc_limit_str.empty())
                ac_oc_limit_str = "16.0";
            last_command = "Set AC Overcurrent Limit (" + ac_oc_limit_str + " A)";
            r_bsp->call_ac_set_overcurrent_limit_A(std::stof(ac_oc_limit_str.c_str()));
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    InputOption ac_oc_input_opt;
    ac_oc_input_opt.multiline = false;
    ac_oc_input_opt.cursor_position = 0;
    ac_oc_input_opt.on_enter = [&]() {
        if (ac_oc_limit_str.empty())
            ac_oc_limit_str = "16.0";
        last_command = "Set AC Overcurrent Limit (" + ac_oc_limit_str + " A)";
        r_bsp->call_ac_set_overcurrent_limit_A(std::stof(ac_oc_limit_str.c_str()));
    };
    auto ac_oc_input = Input(&ac_oc_limit_str, "16.0", ac_oc_input_opt);

    auto ac_oc_component = Container::Vertical({
        ac_oc_limit_button,
        ac_oc_input,
    });

    auto ac_oc_renderer =
        Renderer(ac_oc_component, [&] { return window(text("AC Overcurrent Limit"), ac_oc_component->Render()); });

    // ---------------------------------------------------------------------------
    // Vars
    // ---------------------------------------------------------------------------

    auto vars_renderer = Renderer([&] {
        std::vector<std::vector<std::string>> table_content;
        {
            std::scoped_lock lock(data_mutex);
            table_content = {{"CP State", cp_state},
                             {"Relais", relais_feedback},
                             {"Telemetry", telemetry},
                             {"Proximity Pilot", proximity_pilot}};
        }
        auto vars = Table(table_content);

        vars.SelectColumn(0).Border(LIGHT);
        for (int i = 0; i < table_content.size(); i++) {
            vars.SelectRow(i).Border(LIGHT);
        }
        return vbox({
                   hbox({
                       window(
                           text("Information"),
                           vbox({text("Last command: " + last_command), text("Last error raised: " + last_error_raised),
                                 text("Last error cleared: " + last_error_cleared), vars.Render()})),
                   }),
               }) |
               flex_grow;
    });

    // ---------------------------------------------------------------------------
    // Request stop transaction
    // ---------------------------------------------------------------------------
    auto vars_stop_transaction_renderer = Renderer([&] {
        Element reason = text("Request stop transaction: " + stop_transaction);

        return vbox({
                   hbox({
                       window(text("Buttons"), reason),
                   }),
               }) |
               flex_grow;
    });

    // ---------------------------------------------------------------------------
    // Capabilities
    // ---------------------------------------------------------------------------

    types::evse_board_support::HardwareCapabilities dummy_caps;
    dummy_caps.connector_type = types::evse_board_support::Connector_type::IEC62196Type2Cable;
    hw_caps = to_string(dummy_caps);

    auto caps_renderer = Renderer([&] {
        std::vector<std::vector<std::string>> caps_table_content;
        {
            std::scoped_lock lock(data_mutex);
            caps_table_content = hw_caps;
        }
        auto caps = Table(caps_table_content);

        caps.SelectColumn(0).Border(LIGHT);
        for (int i = 0; i < caps_table_content.size(); i++) {
            caps.SelectRow(i).Border(LIGHT);
        }
        return vbox({
                   hbox({
                       window(text("Capabilities"), caps.Render()),
                   }),
               }) |
               flex_grow;
    });

    // ---------------------------------------------------------------------------
    // AC RCD
    // ---------------------------------------------------------------------------

    auto no_rcd_text = Renderer([] { return text("No AC RCD module connected in config."); });
    auto rcd_component = Container::Vertical({
        no_rcd_text,
    });

    if (!r_ac_rcd.empty()) {

        std::string rcd_current_display = "N/A";
        std::string rcd_reset_result = "";

        Component rcd_self_test_button = Button(
            "Self Test",
            [&] {
                last_command = "AC RCD Self Test";
                r_ac_rcd.at(0)->call_self_test();
            },
            ButtonOption::Animated(Color::Green, Color::White, Color::GreenLight, Color::White));

        Component rcd_reset_button = Button(
            "Reset",
            [&] {
                last_command = "AC RCD Reset";
                bool reset_result = r_ac_rcd.at(0)->call_reset();
                rcd_reset_result = fmt::format("Reset result: {}", reset_result ? "Success" : "Failed");
            },
            ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

        rcd_component = Container::Vertical({
            rcd_self_test_button,
            rcd_reset_button,
        });
    }

    // ---------------------------------------------------------------------------
    // Connector Lock
    // ---------------------------------------------------------------------------

    auto no_lock_text = Renderer([] { return text("No Connector Lock module connected in config."); });
    auto lock_component = Container::Vertical({
        no_lock_text,
    });

    if (!r_lock_motor.empty()) {

        Component lock_button = Button(
            "Connector Lock",
            [&] {
                last_command = "Connector Lock";
                r_lock_motor.at(0)->call_lock();
            },
            ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

        Component unlock_button = Button(
            "Connector Unlock",
            [&] {
                last_command = "Connector Unlock";
                r_lock_motor.at(0)->call_unlock();
            },
            ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

        lock_component = Container::Vertical({
            lock_button,
            unlock_button,
        });
    }

    // ---------------------------------------------------------------------------
    // Main layout
    // ---------------------------------------------------------------------------

    auto command_row =
        Container::Horizontal({pwm_renderer, relais_renderer, enable_renderer, phase_switch_renderer, ac_oc_renderer});

    auto info_row = Container::Horizontal({
        vars_renderer,
        caps_renderer,
        vars_stop_transaction_renderer,
    });

    // Add RCD and Lock components to the interactive container tree
    auto main_container = Container::Vertical({info_row, command_row, rcd_component, lock_component});

    auto main_renderer = Renderer(main_container, [&] {
        return vbox(text("EVSE Board Support") | bold | hcenter, separator(), info_row->Render(), command_row->Render(),
                    separator(),
                    hbox(window(text("AC RCD"), vbox(text("RCD Current: " + rcd_current_display + " mA"), separator(),
                                                     rcd_component->Render(), text(rcd_reset_result))) |
                             flex,
                         separator(), window(text("Connector Lock"), lock_component->Render()) | flex));
    });

    screen.Loop(main_renderer);
}

} // namespace module
