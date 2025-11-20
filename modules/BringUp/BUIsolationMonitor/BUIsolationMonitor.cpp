// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "BUIsolationMonitor.hpp"
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

static std::vector<std::vector<std::string>> to_table(types::isolation_monitor::IsolationMeasurement m) {
    std::vector<std::vector<std::string>> measurement;
    measurement.push_back({"Resistance F", fmt::format("{} Ohm", m.resistance_F_Ohm)});

    if (m.voltage_V.has_value()) {
        measurement.push_back({"DC Voltage", fmt::format("{} V", m.voltage_V.value())});
    }

    if (m.voltage_to_earth_l1e_V.has_value()) {
        measurement.push_back({"DC Voltage to Earth L1E", fmt::format("{} V", m.voltage_to_earth_l1e_V.value())});
    }

    if (m.voltage_to_earth_l2e_V.has_value()) {
        measurement.push_back({"DC Voltage to Earth L2E", fmt::format("{} V", m.voltage_to_earth_l2e_V.value())});
    }

    measurement.push_back({"Timestamp: ", Everest::Date::to_rfc3339(date::utc_clock::now())});

    return measurement;
}

void BUIsolationMonitor::init() {
    invoke_init(*p_main);

    types::isolation_monitor::IsolationMeasurement m;
    m.resistance_F_Ohm = 0;
    last_measurement = to_table(m);
}

void BUIsolationMonitor::ready() {
    invoke_ready(*p_main);

    auto screen = ScreenInteractive::Fullscreen();

    r_imd->subscribe_isolation_measurement([this, &screen](const types::isolation_monitor::IsolationMeasurement m) {
        std::scoped_lock lock(data_mutex);
        last_measurement = to_table(m);
        screen.Post(Event::Custom);
    });

    // ---------------------------------------------------------------------------
    // Start IMD Measurements
    // ---------------------------------------------------------------------------

    Component start_button = Button(
        "Start Measurements", [&] { r_imd->call_start(); },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    Component stop_button = Button(
        "Stop Measurements", [&] { r_imd->call_stop(); },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    Component selftest_button = Button(
        "Run Self-Test", [&] { r_imd->call_start_self_test(500); },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    auto ctrl_component = Container::Horizontal({
        Container::Vertical({
            start_button,
            stop_button,
            selftest_button,
        }),
    });

    auto ctrl_renderer = Renderer(ctrl_component, [&] {
        auto ctrl_win = window(text("Control"), ctrl_component->Render());
        return vbox({
                   hbox({
                       ctrl_win,
                   }),
               }) |
               flex_grow;
    });

    // ---------------------------------------------------------------------------
    // Vars
    // ---------------------------------------------------------------------------

    auto vars_renderer = Renderer([&] {
        std::vector<std::vector<std::string>> table_content;
        {
            std::scoped_lock lock(data_mutex);
            table_content = last_measurement;
            // table_content = {{"CP State", "cp_state"}, {"Relais", "relais_feedback"}, {"Telemetry", "telemetry"}};
        }
        auto vars = Table(table_content);

        vars.SelectColumn(0).Border(LIGHT);
        for (int i = 0; i < table_content.size(); i++) {
            vars.SelectRow(i).Border(LIGHT);
        }
        return vbox({
                   hbox({
                       window(text("Information"), vars.Render()),
                   }),
               }) |
               flex_grow;
    });

    auto main_container = Container::Horizontal({
        ctrl_renderer,
        vars_renderer,
    });

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            text("Isolation Monitor") | bold | hcenter,
            main_container->Render(),
        });
    });

    screen.Loop(main_renderer);
}

} // namespace module
