// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "BUDCExternalDerate.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"
#include <regex>
#include <string>

namespace module {

using namespace ftxui;

void BUDCExternalDerate::init() {
    invoke_init(*p_main);
}

void BUDCExternalDerate::ready() {
    invoke_ready(*p_main);

    auto screen = ScreenInteractive::Fullscreen();

    auto msg_component = Container::Vertical({Renderer([] { return text(""); })});

    std::map<std::string, std::map<std::string, Component>> messages;

    r_derate->subscribe_all_errors(
        [&](Everest::error::Error const& error) {
            std::scoped_lock lock(data_mutex);
            auto error_elem = Renderer([=] { return text(" - " + error.type); });
            auto sub_elem = Renderer([=] { return text("    " + error.sub_type); });
            auto msg_elem = Renderer([=] { return text("    " + error.message); });

            auto component = Container::Vertical({
                error_elem,
                sub_elem,
                msg_elem,
            });

            messages[error.type][error.sub_type] = component;
            msg_component->Add(component);
            screen.Post(Event::Custom);
        },
        [&](Everest::error::Error const& error) {
            std::scoped_lock lock(data_mutex);
            if (messages.count(error.type)) {
                auto& sub = messages.at(error.type);
                if (sub.count(error.sub_type)) {
                    auto& elem = sub.at(error.sub_type);
                    elem->Detach();
                    sub.erase(error.sub_type);
                }
                if (not sub.size()) {
                    messages.erase(error.type);
                }
            }
            screen.Post(Event::Custom);
        });

    auto msg_component_holder = Container::Horizontal({msg_component});

    auto msg_component_renderer = Renderer(msg_component_holder, [&] {
        auto win = window(text("Active Errors"), msg_component_holder->Render());
        return vbox({
                   hbox({
                       win,
                   }),
               }) |
               flex_grow;
    });

    auto var_component =
        Container::Vertical({Renderer([&] { return text("plug_temperature_C: " + std::to_string(plug_temp_C)); })});
    auto var_component_holder = Container::Horizontal({var_component});
    auto var_component_renderer = Renderer(var_component_holder, [&] {
        auto win = window(text("Vars"), var_component_holder->Render());
        return vbox({
                   hbox({
                       win,
                   }),
               }) |
               flex_grow;
    });

    r_derate->subscribe_plug_temperature_C([&](double temp) {
        std::scoped_lock lock(data_mutex);
        plug_temp_C = temp;
        screen.Post(Event::Custom);
    });

    InputOption o;
    o.multiline = false;
    o.cursor_position = 0;

    auto max_export_current_A_input = Input(&max_export_current_A, "100.0", o);
    auto max_import_current_A_input = Input(&max_import_current_A, "200.0", o);
    auto max_export_power_W_input = Input(&max_export_power_W, "300.0", o);
    auto max_import_power_W_input = Input(&max_import_power_W, "400.0", o);

    Component ovm_start = Button(
                              "Set",
                              [&] {
                                  types::dc_external_derate::ExternalDerating val;
                                  val.max_export_current_A = std::stof(max_export_current_A);
                                  val.max_import_current_A = std::stof(max_import_current_A);
                                  val.max_export_power_W = std::stof(max_export_power_W);
                                  val.max_import_power_W = std::stof(max_import_power_W);
                                  r_derate->call_set_external_derating(val);
                              },
                              ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White)) |
                          flex_grow;

    auto action_component = Container::Horizontal({
        Container::Vertical({
            max_export_current_A_input,
            max_import_current_A_input,
            max_export_power_W_input,
            max_import_power_W_input,
            ovm_start,
        }),
    });

    auto action_component_renderer = Renderer(action_component, [&] {
        return vbox({hbox(ovm_start->Render()),

                     vbox(hbox(text(" Max Export Current (A): "), max_export_current_A_input->Render()),
                          hbox(text(" Max Import Current (A): "), max_import_current_A_input->Render()),
                          hbox(text(" Max Export Power (W): "), max_export_power_W_input->Render()),
                          hbox(text(" Max Import Power (W): "), max_import_power_W_input->Render()))});
    });

    auto action_renderer = Renderer(action_component, [&] {
        return vbox({
                   hbox({
                       window(text("DC External Derate Commands"), action_component_renderer->Render()),
                   }),
               }) |
               flex_grow;
    });

    auto main_container = Container::Horizontal({action_renderer, var_component_renderer, msg_component_renderer});

    auto main_renderer = Renderer(main_container, [&] {
        std::scoped_lock lock(data_mutex);
        return vbox({
            text("DC External Derate Bringup") | bold | hcenter,
            hbox({main_container->Render()}),
        });
    });

    screen.Loop(main_renderer);
}

} // namespace module
