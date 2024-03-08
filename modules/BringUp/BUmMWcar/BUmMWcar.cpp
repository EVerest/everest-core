// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "BUmMWcar.hpp"
#include "ftxui/component/component.hpp" // for Checkbox, Renderer, Horizontal, Vertical, Input, Menu, Radiobox, ResizableSplitLeft, Tab
#include "ftxui/component/component_base.hpp"     // for ComponentBase, Component
#include "ftxui/component/component_options.hpp"  // for MenuOption, InputOption
#include "ftxui/component/event.hpp"              // for Event, Event::Custom
#include "ftxui/component/screen_interactive.hpp" // for Component, ScreenInteractive

#include "ftxui/dom/elements.hpp" // for text, color, operator|, bgcolor, filler, Element, vbox, size, hbox, separator, flex, window, graph, EQUAL, paragraph, WIDTH, hcenter, Elements, bold, vscroll_indicator, HEIGHT, flexbox, hflow, border, frame, flex_grow, gauge, paragraphAlignCenter, paragraphAlignJustify, paragraphAlignLeft, paragraphAlignRight, dim, spinner, LESS_THAN, center, yframe, GREATER_THAN
#include "ftxui/dom/flexbox_config.hpp" // for FlexboxConfig
#include "ftxui/dom/table.hpp"          // for Table
#include "ftxui/screen/color.hpp" // for Color, Color::BlueLight, Color::RedLight, Color::Black, Color::Blue, Color::Cyan, Color::CyanLight, Color::GrayDark, Color::GrayLight, Color::Green, Color::GreenLight, Color::Magenta, Color::MagentaLight, Color::Red, Color::White, Color::Yellow, Color::YellowLight, Color::Default, Color::Palette256, ftxui
#include "ftxui/screen/color_info.hpp" // for ColorInfo
#include "ftxui/screen/terminal.hpp"   // for Size, Dimensions

using namespace ftxui;

namespace module {

static std::vector<std::vector<std::string>> keep_alive_to_table(types::ev_board_support_extended::KeepAliveData d);
static std::vector<std::vector<std::string>> ac_instant_to_table(types::ev_board_support_extended::ACVoltageData d);
static std::vector<std::vector<std::string>> ac_stats_to_table(types::ev_board_support_extended::ACPhaseStatistics d);
static std::vector<std::vector<std::string>> dc_instant_to_table(double d);
static std::vector<std::vector<std::string>> edge_stats_to_table(types::ev_board_support_extended::EdgeTimingData d);
static std::vector<std::vector<std::string>> bsp_event_meas_to_table(types::board_support_common::BspEvent e,
                                                                     types::board_support_common::BspMeasurement m);

void BUmMWcar::init() {
    invoke_init(*p_main);
}

void BUmMWcar::ready() {
    invoke_ready(*p_main);

    last_ac_instant_poll = std::chrono::high_resolution_clock::now();

    auto screen = ScreenInteractive::Fullscreen();

    /* Variable subscriptions */
    r_board_support_extended->subscribe_keep_alive_event(
        [this, &screen](const types::ev_board_support_extended::KeepAliveData data) {
            std::scoped_lock lock(data_mutex);
            last_keep_alive_data = data;
            screen.Post(Event::Custom);
        });

    r_board_support_extended->subscribe_ac_voltage_event(
        [this, &screen](const types::ev_board_support_extended::ACVoltageData data) {
            std::scoped_lock lock(data_mutex);
            last_ac_instant_data = data;
            screen.Post(Event::Custom);
        });

    r_board_support_extended->subscribe_ac_statistics_event(
        [this, &screen](const types::ev_board_support_extended::ACPhaseStatistics data) {
            std::scoped_lock lock(data_mutex);
            last_ac_stats_data = data;
            screen.Post(Event::Custom);
        });

    r_board_support_extended->subscribe_dc_voltage_event([this, &screen](const double data) {
        std::scoped_lock lock(data_mutex);
        last_dc_instant_data = data;
        screen.Post(Event::Custom);
    });

    r_board_support_extended->subscribe_edge_timing_event(
        [this, &screen](const types::ev_board_support_extended::EdgeTimingData data) {
            std::scoped_lock lock(data_mutex);
            last_edge_data = data;
            screen.Post(Event::Custom);
        });

    r_board_support->subscribe_bsp_event([this, &screen](const types::board_support_common::BspEvent data) {
        std::scoped_lock lock(data_mutex);
        last_bsp_event = data;
        screen.Post(Event::Custom);
    });

    r_board_support->subscribe_bsp_measurement([this, &screen](const types::board_support_common::BspMeasurement data) {
        std::scoped_lock lock(data_mutex);
        last_bsp_meas = data;
        screen.Post(Event::Custom);
    });

    /* Reset Controls renderer */
    Component hard_reset_button = Button(
        "Hard reset", [&] { r_board_support_extended->call_reset(false); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component soft_reset_button = Button(
        "Soft reset", [&] { r_board_support_extended->call_reset(true); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    auto reset_buttons_component = Container::Horizontal({
                                       Container::Vertical({
                                           soft_reset_button,
                                           hard_reset_button,
                                       }) | hcenter |
                                           flex_grow,
                                   }) |
                                   hcenter | flex_grow;

    auto reset_buttons_renderer = Renderer(reset_buttons_component, [&] {
        auto ctrl_win = window(text("Reset"), reset_buttons_component->Render());
        return hbox({ctrl_win});
    });

    /* CP state controls renderer*/
    Component set_state_A_button = Button(
        "Set State A", [&] { r_board_support->call_set_cp_state(types::ev_board_support::EvCpState::A); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component set_state_B_button = Button(
        "Set State B", [&] { r_board_support->call_set_cp_state(types::ev_board_support::EvCpState::B); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component set_state_C_button = Button(
        "Set State C", [&] { r_board_support->call_set_cp_state(types::ev_board_support::EvCpState::C); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component set_state_D_button = Button(
        "Set State D", [&] { r_board_support->call_set_cp_state(types::ev_board_support::EvCpState::D); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component set_state_E_button = Button(
        "Set State E", [&] { r_board_support->call_set_cp_state(types::ev_board_support::EvCpState::E); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    auto cp_state_buttons_component = Container::Horizontal({Container::Vertical({
                                                                 set_state_A_button,
                                                                 set_state_B_button,
                                                                 set_state_C_button,
                                                                 set_state_D_button,
                                                                 set_state_E_button,
                                                             }) |
                                                             hcenter | flex_grow}) |
                                      flex_grow;

    auto cp_state_buttons_renderer = Renderer(cp_state_buttons_component, [&] {
        auto win = window(text("CP State"), cp_state_buttons_component->Render());
        return hbox({
            win,
        });
    });

    /* CP voltage renderer */
    Component set_cp_voltage_button = Button(
        "Set CP voltage",
        [&] {
            try {
                float voltage = std::stof(set_cp_voltage_input_str);
                r_board_support_extended->call_set_cp_voltage(voltage);
            } catch (...) {
            }
        },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));
    // Enable/disable CP load
    Component set_cp_load_enabled_button = Button(
        "Enable CP load", [&] { r_board_support_extended->call_set_cp_load_en(true); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));
    Component set_cp_load_disabled_button = Button(
        "Disable CP load", [&] { r_board_support_extended->call_set_cp_load_en(false); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));
    // Enable/disable CP short to GND
    Component set_cp_short_to_gnd_enabled_button = Button(
        "Enable CP short to GND", [&] { r_board_support_extended->call_set_cp_short_to_gnd_en(true); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));
    Component set_cp_short_to_gnd_disabled_button = Button(
        "Disable CP short to GND", [&] { r_board_support_extended->call_set_cp_short_to_gnd_en(false); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));
    // Enable/disable CP diode fault
    Component set_cp_diode_fault_enabled_button = Button(
        "Enable CP diode fault", [&] { r_board_support_extended->call_set_cp_diode_fault_en(true); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));
    Component set_cp_diode_fault_disabled_button = Button(
        "Disable CP diode fault", [&] { r_board_support_extended->call_set_cp_diode_fault_en(false); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component set_cp_voltage_input = Input(&set_cp_voltage_input_str, "0.0");

    auto cp_control_components = Container::Horizontal({
        set_cp_load_enabled_button,
        set_cp_load_disabled_button,
        set_cp_short_to_gnd_enabled_button,
        set_cp_short_to_gnd_disabled_button,
        set_cp_diode_fault_enabled_button,
        set_cp_diode_fault_disabled_button,
        set_cp_voltage_input,
        set_cp_voltage_button,
    });

    auto cp_control_renderer = Renderer(cp_control_components, [&] {
        auto content = vbox({
                           set_cp_load_enabled_button->Render(),
                           set_cp_load_disabled_button->Render(),
                           separator(),
                           set_cp_short_to_gnd_enabled_button->Render(),
                           set_cp_short_to_gnd_disabled_button->Render(),
                           separator(),
                           set_cp_diode_fault_enabled_button->Render(),
                           set_cp_diode_fault_disabled_button->Render(),
                           separator(),
                           hbox({text("CP Voltage: "), set_cp_voltage_input->Render()}),
                           set_cp_voltage_button->Render(),
                       }) |
                       border;
        return window(text("CP extended control"), content);
    });

    /* Keep alive renderer */
    auto keep_alive_renderer = Renderer([&] {
        std::vector<std::vector<std::string>> keep_alive_table_content;
        {
            std::scoped_lock lock(data_mutex);
            keep_alive_table_content = keep_alive_to_table(last_keep_alive_data);
        }
        auto keep_alive_table = Table(keep_alive_table_content);

        // add borders
        keep_alive_table.SelectColumn(0).Border(LIGHT);
        for (int i = 0; i < keep_alive_table_content.size(); i++) {
            keep_alive_table.SelectRow(i).Border(LIGHT);
        }

        return window(text("Keep alive data"), hbox({keep_alive_table.Render()}) | flex_grow);
    });

    /* BSP Events and BSP Measurements */
    auto bsp_event_measurement_renderer = Renderer([&] {
        std::vector<std::vector<std::string>> bsp_event_meas_table_content;
        {
            std::scoped_lock lock(data_mutex);
            bsp_event_meas_table_content = bsp_event_meas_to_table(last_bsp_event, last_bsp_meas);
        }
        auto bsp_event_meas_table = Table(bsp_event_meas_table_content);

        // add borders
        bsp_event_meas_table.SelectColumn(0).Border(LIGHT);
        for (int i = 0; i < bsp_event_meas_table_content.size(); i++) {
            bsp_event_meas_table.SelectRow(i).Border(LIGHT);
        }

        return window(text("BSP data"), hbox({bsp_event_meas_table.Render()}) | flex_grow);
    });

    /* AC instantaneous renderer */
    /* Controls*/
    Component get_ac_instant_button = Button(
        "Get AC instant data", [&] { r_board_support_extended->call_get_ac_data_instant(); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component get_ac_stats_button = Button(
        "Get AC statistics", [&] { r_board_support_extended->call_get_ac_statistics(); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component cb_ac_instant_poll = Checkbox("Poll AC instant data", &ac_instant_poll);

    auto ac_instant_ctrl_component = Container::Horizontal({
        Container::Vertical({get_ac_instant_button, get_ac_stats_button, cb_ac_instant_poll}),
    });

    auto ac_ctrl_renderer = Renderer(ac_instant_ctrl_component, [&] {
        auto ctrl_win = ac_instant_ctrl_component->Render();
        return vbox({
            hbox({
                ctrl_win,
            }),
        });
    });

    /* AC renderer */
    auto ac_renderer = Renderer(ac_ctrl_renderer, [&] {
        std::vector<std::vector<std::string>> ac_instant_table_content;
        std::vector<std::vector<std::string>> ac_stats_table_content;

        {
            std::scoped_lock lock(data_mutex);
            if (ac_instant_poll) {
                auto now = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> elapsed = now - last_ac_instant_poll;
                if (elapsed.count() > 200) {
                    r_board_support_extended->call_get_ac_data_instant();
                    last_ac_instant_poll = std::chrono::high_resolution_clock::now();
                }
            }
            ac_instant_table_content = ac_instant_to_table(last_ac_instant_data);
            ac_stats_table_content = ac_stats_to_table(last_ac_stats_data);
        }
        auto ac_instant_table = Table(ac_instant_table_content);
        auto ac_stats_table = Table(ac_stats_table_content);

        // add borders
        ac_instant_table.SelectColumn(0).Border(HEAVY);
        for (int i = 0; i < ac_instant_table_content.size(); i++) {
            ac_instant_table.SelectRow(i).Border(LIGHT);
        }

        for (int col = 0; col < 3; col++) {
            ac_stats_table.SelectColumn(col).Border(LIGHT);
            for (int i = 0; i < ac_stats_table_content.size(); i++) {
                ac_stats_table.SelectRow(i).Border(LIGHT);
            }
        }

        return window(text("AC measurement"),
                      hbox({
                          window(text("Ctrl"), ac_ctrl_renderer->Render()),
                          window(text("Instantaneous"), ac_instant_table.Render() | hcenter),
                          window(text("Statistics"),
                                 vbox({/*
                                        * parameterization not implemented or decided if needed
                                        *
                                       vbox({
                                          hbox({text("Sample rate:") | flex_grow, text(fmt::format("{} ms", 0))}),
                                          hbox({text("Window length:") | flex_grow, text(fmt::format("{}", 0))}),
                                          hbox({text("Window duration:") | flex_grow, text(fmt::format("{} ms", 0))}),
                                       }) | border | flex_grow,
                                       */
                                       ac_stats_table.Render() | center}) |
                                     flex_grow),
                      }) | flex_grow);
    });

    /* Edge timing renderer */
    /* Controls*/
    Component set_edge_timing_num_periods_input = Input(&set_edge_timing_num_periods_str, "100");
    Component cb_edge_meas_force_restart = Checkbox("", &edge_meas_force_restart);

    Component start_edge_timing_meas_button = Button(
        "Start edge timing meas",
        [&] {
            try {
                uint16_t num_periods = std::stoi(set_edge_timing_num_periods_str);
                r_board_support_extended->call_trigger_edge_timing_measurement(num_periods, edge_meas_force_restart);
            } catch (...) {
            }
        },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    auto edge_ctrl_component = Container::Horizontal({
        Container::Vertical({
            set_edge_timing_num_periods_input,
            cb_edge_meas_force_restart,
            start_edge_timing_meas_button,
        }),
    });

    auto edge_ctrl_renderer = Renderer(edge_ctrl_component, [&] {
        auto content = vbox({
            hbox({text("#Periods:      "), set_edge_timing_num_periods_input->Render()}),
            hbox({text("Force restart: "), cb_edge_meas_force_restart->Render()}),
            separator(),
            start_edge_timing_meas_button->Render(),
        });
        return content;
    });

    auto edge_result_info_renderer = Renderer([&] {
        auto content =
            vbox({
                hbox({text("Number of cycles:") | flex_grow, text(fmt::format("{}", last_edge_data.num_periods))}),
                hbox({text("CP state:") | flex_grow,
                      text(types::ev_board_support::ev_cp_state_to_string(last_edge_data.cp_state))}),
            }) |
            border | flex_grow;
        return content;
    });

    /* Edge renderer */
    auto edge_renderer = Renderer(edge_ctrl_renderer, [&] {
        std::vector<std::vector<std::string>> edge_stats_table_content;

        {
            std::scoped_lock lock(data_mutex);
            edge_stats_table_content = edge_stats_to_table(last_edge_data);
        }
        auto edge_stats_table = Table(edge_stats_table_content);

        for (int col = 0; col < 3; col++) {
            edge_stats_table.SelectColumn(col).Border(LIGHT);
            for (int i = 0; i < edge_stats_table_content.size(); i++) {
                edge_stats_table.SelectRow(i).Border(LIGHT);
            }
        }

        return window(
            text("Edge timing measurement"),
            hbox({
                window(text("Ctrl"), edge_ctrl_renderer->Render()),
                window(text("Statistics"),
                       vbox({edge_result_info_renderer->Render(), edge_stats_table.Render() | center}) | flex_grow),
            }) | flex_grow);
    });

    /* DC measurement */
    /* Controls*/
    Component get_dc_instant_button = Button(
        "Get DC instant data", [&] { r_board_support_extended->call_get_dc_data_instant(); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    // The component tree:
    auto dc_input_component = Container::Vertical({
        get_dc_instant_button,
    });

    auto dc_meas_instant_display_renderer = Renderer([&] {
        std::vector<std::vector<std::string>> dc_instant_table_content;
        {
            std::scoped_lock lock(data_mutex);
            dc_instant_table_content = dc_instant_to_table(last_dc_instant_data);
        }

        auto table = Table(dc_instant_table_content);
        // add borders
        table.SelectColumn(1).Border(LIGHT);
        for (int i = 0; i < dc_instant_table_content.size(); i++) {
            table.SelectRow(i).Border(LIGHT);
        }

        return hbox({table.Render()}) | flex_shrink;
    });

    // Tweak how the component tree is rendered:
    auto dc_measurement_renderer = Renderer(dc_input_component, [&] {
        auto content = vbox({
                           get_dc_instant_button->Render(),
                           dc_meas_instant_display_renderer->Render(),
                       }) |
                       border;
        return window(text("DC Measurement"), content);
    });

    /* Main container*/
    auto main_container = Container::Horizontal({
        Container::Vertical({
            reset_buttons_renderer,
            cp_state_buttons_renderer,
        }),
        Container::Vertical({Container::Horizontal({keep_alive_renderer, bsp_event_measurement_renderer}), ac_renderer,
                             edge_renderer}) |
            flex_grow,
        Container::Vertical({
            dc_measurement_renderer,
            cp_control_renderer,
        }) | flex_grow,
    });

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            text("mMW car simulator") | bold | hcenter,
            main_container->Render(),
        });
    });

    screen.Loop(main_renderer);
}

static std::vector<std::vector<std::string>> keep_alive_to_table(types::ev_board_support_extended::KeepAliveData d) {
    std::vector<std::vector<std::string>> table;
    table.push_back({"Timestamp", fmt::format("{:010} ticks", d.timestamp)});
    table.push_back({"HW Major Rev.", hw_revision_major_to_string(d.hw_revision_major)});
    table.push_back({"HW Minor Rev.", fmt::format("{}", d.hw_revision_minor)});
    table.push_back({"SW version", d.sw_version});
    return table;
}

static std::vector<std::vector<std::string>> ac_instant_to_table(types::ev_board_support_extended::ACVoltageData d) {
    std::vector<std::vector<std::string>> table;
    table.push_back({"L1", fmt::format("{:.3f} V", d.Ua)});
    table.push_back({"L2", fmt::format("{:.3f} V", d.Ub)});
    table.push_back({"L3", fmt::format("{:.3f} V", d.Uc)});

    return table;
}

static std::vector<std::vector<std::string>> ac_stats_to_table(types::ev_board_support_extended::ACPhaseStatistics d) {
    std::vector<std::vector<std::string>> table;
    table.push_back({"Phase", "Min.", "Max.", "Avg."});
    table.push_back({"L1", fmt::format("{:.4f} V", d.Ua.min), fmt::format("{:.4f} V", d.Ua.max),
                     fmt::format("{:.4f} V", d.Ua.average)});
    table.push_back({"L2", fmt::format("{:.4f} V", d.Ub.min), fmt::format("{:.4f} V", d.Ub.max),
                     fmt::format("{:.4f} V", d.Ub.average)});
    table.push_back({"L3", fmt::format("{:.4f} V", d.Uc.min), fmt::format("{:.4f} V", d.Uc.max),
                     fmt::format("{:.4f} V", d.Uc.average)});

    return table;
}

static std::vector<std::vector<std::string>> dc_instant_to_table(double d) {
    std::vector<std::vector<std::string>> table = {{"DC bus voltage", fmt::format("{:.4f} V", d)}};
    return table;
}

static std::vector<std::vector<std::string>> edge_stats_to_table(types::ev_board_support_extended::EdgeTimingData d) {
    std::vector<std::vector<std::string>> table;
    table.push_back({"Edge", "Min.", "Avg.", "Max."});
    table.push_back({"Rising", fmt::format("{} ns", d.rising.min), fmt::format("{} ns", d.rising.average),
                     fmt::format("{} ns", d.rising.max)});
    table.push_back({"Falling", fmt::format("{} ns", d.falling.min), fmt::format("{} ns", d.falling.average),
                     fmt::format("{} ns", d.falling.max)});

    return table;
}

static std::vector<std::vector<std::string>> bsp_event_meas_to_table(types::board_support_common::BspEvent e,
                                                                     types::board_support_common::BspMeasurement m) {
    std::vector<std::vector<std::string>> table;

    table.push_back({"CP state", types::board_support_common::event_to_string(e.event)});
    table.push_back({"PP", types::board_support_common::ampacity_to_string(m.proximity_pilot.ampacity)});
    table.push_back({"PWM duty", fmt::format("{:.2f}%", m.cp_pwm_duty_cycle)});
    return table;
}

} // namespace module
