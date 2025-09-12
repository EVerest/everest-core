// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "BUPowerSupplyDC.hpp"
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
#include "generated/types/power_supply_DC.hpp"

using namespace ftxui;

namespace module {

static std::vector<std::vector<std::string>> to_string(types::power_supply_DC::Capabilities c) {
    std::vector<std::vector<std::string>> hw_caps;
    hw_caps.push_back({"Bi-Directional", (c.bidirectional ? "yes" : "no")});
    hw_caps.push_back({"Export", fmt::format("{}V/{}V {}A/{}A {}W Efficiency {}", c.max_export_voltage_V,
                                             c.min_export_voltage_V, c.max_export_current_A, c.min_export_current_A,
                                             c.max_export_power_W, c.conversion_efficiency_export.value_or(1.0))});
    if (c.bidirectional) {
        hw_caps.push_back(
            {"Import", fmt::format("{}V/{}V {}A/{}A {}W Efficiency {}", c.max_import_voltage_V.value_or(0),
                                   c.min_import_voltage_V.value_or(0), c.max_import_current_A.value_or(0),
                                   c.min_import_current_A.value_or(0), c.max_import_power_W.value_or(0),
                                   c.conversion_efficiency_import.value_or(1.0))});
    }
    hw_caps.push_back({"Current", fmt::format("Ripple {}A Regulation {}A", c.peak_current_ripple_A,
                                              c.current_regulation_tolerance_A)});

    return hw_caps;
}

void BUPowerSupplyDC::init() {
    invoke_init(*p_main);
}

void BUPowerSupplyDC::ready() {
    invoke_ready(*p_main);

    auto screen = ScreenInteractive::Fullscreen();

    r_psu->subscribe_voltage_current([this, &screen](const types::power_supply_DC::VoltageCurrent v) {
        std::scoped_lock lock(data_mutex);
        actual_voltage = fmt::format("{}V", v.voltage_V);
        actual_current = fmt::format("{}A", v.current_A);
        screen.Post(Event::Custom);
    });

    r_psu->subscribe_mode([this, &screen](const types::power_supply_DC::Mode mode) {
        std::scoped_lock lock(data_mutex);
        actual_mode = mode_to_string(mode);
        screen.Post(Event::Custom);
    });

    r_psu->subscribe_capabilities([this, &screen](const types::power_supply_DC::Capabilities caps) {
        std::scoped_lock lock(data_mutex);
        raw_caps = caps;
        screen.Post(Event::Custom);
    });

    std::string last_command = "None";

    // ---------------------------------------------------------------------------
    // Select mode
    // ---------------------------------------------------------------------------

    Component psu_off = Button(
        "Power Off",
        [&] {
            last_command = "Switch Off";
            r_psu->call_setMode(types::power_supply_DC::Mode::Off, types::power_supply_DC::ChargingPhase::Other);
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    Component psu_export_cablecheck = Button(
        "Export (cablecheck EV)",
        [&] {
            last_command = "Export (precharge EV)";
            r_psu->call_setMode(types::power_supply_DC::Mode::Export,
                                types::power_supply_DC::ChargingPhase::CableCheck);
        },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component psu_export_precharge = Button(
        "Export (precharge EV)",
        [&] {
            last_command = "Export (precharge EV)";
            r_psu->call_setMode(types::power_supply_DC::Mode::Export, types::power_supply_DC::ChargingPhase::PreCharge);
        },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component psu_export = Button(
        "Export (charge EV)",
        [&] {
            last_command = "Export (charge EV)";
            r_psu->call_setMode(types::power_supply_DC::Mode::Export, types::power_supply_DC::ChargingPhase::Charging);
        },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component psu_import = Button(
        "Import (discharge EV)",
        [&] {
            last_command = "Import (discharge EV)";
            r_psu->call_setMode(types::power_supply_DC::Mode::Import, types::power_supply_DC::ChargingPhase::Other);
        },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    auto mode_component = Container::Horizontal({
        Container::Vertical({
            psu_off,
            psu_export_cablecheck,
            psu_export_precharge,
            psu_export,
            psu_import,
        }),
    });

    auto mode_renderer = Renderer(mode_component, [&] {
        auto mode_win = window(text("Relais"), mode_component->Render());
        return vbox({
                   hbox({
                       mode_win,
                   }),
               }) |
               flex_grow;
    });

    // ---------------------------------------------------------------------------
    // PWM control
    // ---------------------------------------------------------------------------

    std::string setpoint_export_voltage{"10.0"};
    std::string setpoint_export_current{"1.0"};
    std::string setpoint_import_voltage{"10.0"};
    std::string setpoint_import_current{"1.0"};

    Component set_export_voltage_current = Button(
        "Set Export voltage/current limit",
        [&] {
            float v = std::stof(setpoint_export_voltage);
            float c = std::stof(setpoint_export_current);
            last_command = fmt::format("Set Export voltage/current limit ({}V / {}A)", v, c);
            r_psu->call_setExportVoltageCurrent(v, c);
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    Component set_import_voltage_current = Button(
        "Set Import voltage/current limit",
        [&] {
            float v = std::stof(setpoint_import_voltage);
            float c = std::stof(setpoint_import_current);
            last_command = fmt::format("Set Import voltage/current limit ({}V / {}A)", v, c);
            r_psu->call_setImportVoltageCurrent(v, c);
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    InputOption o;
    o.multiline = false;
    o.cursor_position = 0;

    auto export_voltage_input = Input(&setpoint_export_voltage, "10.0", o);
    auto export_current_input = Input(&setpoint_export_current, "1.0", o);
    auto import_voltage_input = Input(&setpoint_import_voltage, "10.0", o);
    auto import_current_input = Input(&setpoint_import_current, "1.0", o);

    auto voltage_current_component = Container::Horizontal({
        Container::Vertical({
            set_export_voltage_current,
            set_import_voltage_current,
            export_voltage_input,
            export_current_input,
            import_voltage_input,
            import_current_input,
        }),
    });

    auto voltage_current_component_renderer = Renderer(voltage_current_component, [&] {
        return vbox({hbox(set_export_voltage_current->Render()),
                     hbox(text(" Export voltage (V): "), export_voltage_input->Render()),
                     hbox(text(" Export current (A): "), export_current_input->Render()),
                     hbox(set_import_voltage_current->Render()),
                     hbox(text(" Import voltage (V): "), import_voltage_input->Render()),
                     hbox(text(" Import current (A): "), import_current_input->Render())});
    });

    auto voltage_current_renderer = Renderer(voltage_current_component, [&] {
        return vbox({
                   hbox({
                       window(text("Voltage/Current Setting"), voltage_current_component_renderer->Render()),
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
            table_content = {
                {"Actual voltage", actual_voltage}, {"Actual current", actual_current}, {"Actual mode", actual_mode}};
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
    // Capabilities
    // ---------------------------------------------------------------------------

    auto caps_renderer = Renderer([&] {
        std::vector<std::vector<std::string>> caps_table_content;
        {
            std::scoped_lock lock(data_mutex);
            caps_table_content = to_string(raw_caps);
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

    auto main_container = Container::Horizontal(
        {voltage_current_renderer, mode_renderer, Container::Vertical({caps_renderer, vars_renderer})});

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            text("Power Supply DC Bringup") | bold | hcenter,
            main_container->Render(),
        });
    });

    screen.Loop(main_renderer);
}

} // namespace module
