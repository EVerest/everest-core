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
    hw_caps.push_back({"Connector type", types::evse_board_support::connector_type_to_string(c.connector_type)});
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

    r_bsp->subscribe_request_stop_transaction([this, &screen](const types::evse_manager::StopTransactionRequest t) {
        {
            std::scoped_lock lock(data_mutex);
            stop_transaction = stop_transaction_reason_to_string(t.reason);
        }
        screen.Post(Event::Custom);
    });
    // telemetry, pp (needs to be requested as well)

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

    Component pwm_off_button = Button(
        "PWM Off",
        [&] {
            last_command = "PWM Off";
            r_bsp->call_pwm_off();
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

    Component pwm_F_button = Button(
        "PWM F",
        [&] {
            last_command = "PWM F";
            r_bsp->call_pwm_F();
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
            pwm_F_button,
            pwm_off_button,
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
    // Vars
    // ---------------------------------------------------------------------------

    auto vars_renderer = Renderer([&] {
        Element last_command_element = text("Last command: " + last_command);
        std::vector<std::vector<std::string>> table_content;
        {
            std::scoped_lock lock(data_mutex);
            table_content = {{"CP State", cp_state}, {"Relais", relais_feedback}, {"Telemetry", telemetry}};
        }
        auto vars = Table(table_content);

        vars.SelectColumn(0).Border(LIGHT);
        for (int i = 0; i < table_content.size(); i++) {
            vars.SelectRow(i).Border(LIGHT);
        }
        return vbox({
                   hbox({
                       window(text("Information"), vbox({text("Last command: " + last_command), vars.Render()})),
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

    auto main_container = Container::Horizontal({
        Container::Vertical({pwm_renderer, relais_renderer}),
        vars_renderer,
        caps_renderer,
        vars_stop_transaction_renderer,
    });

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            text("EVSE Board Support") | bold | hcenter,
            main_container->Render(),
        });
    });

    screen.Loop(main_renderer);
}

} // namespace module
