// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "BUOverVoltageMonitor.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"
#include <regex>

namespace module {

using namespace ftxui;

void BUOverVoltageMonitor::init() {
    invoke_init(*p_main);
}

void BUOverVoltageMonitor::ready() {
    invoke_ready(*p_main);

    auto screen = ScreenInteractive::Fullscreen();

    auto msg_component = Container::Vertical({Renderer([] { return text(""); })});

    std::map<std::string, std::map<std::string, Component>> messages;

    r_ovm->subscribe_all_errors(
        [&](Everest::error::Error const& error) {
            std::scoped_lock lock(data_mutex);
            std::regex filter("over_voltage_monitor/");
            auto error_type = std::regex_replace(error.type, filter, "");
            auto error_elem = Renderer([=] { return text(" - " + error_type); });
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

    InputOption o;
    o.multiline = false;
    o.cursor_position = 0;

    auto limit_input = Input(&limit, "800.0", o);

    Component ovm_start = Button(
                              "Start",
                              [&] {
                                  auto val = std::stof(limit);
                                  r_ovm->call_start(val);
                              },
                              ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White)) |
                          flex_grow;

    Component ovm_stop = Button(
                             "Stop", [&] { r_ovm->call_stop(); },
                             ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White)) |
                         flex_grow;

    Component ovm_reset = Button(
                              "Reset", [&] { r_ovm->call_reset_over_voltage_error(); },
                              ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White)) |
                          flex_grow;

    auto action_component = Container::Horizontal({
        Container::Vertical({
            limit_input,
            ovm_start,
            ovm_stop,
            ovm_reset,
        }),
    });

    auto action_component_renderer = Renderer(action_component, [&] {
        return vbox({hbox(ovm_start->Render()), hbox(text(" Over voltage limit (V): "), limit_input->Render()),
                     hbox(ovm_stop->Render()), hbox(ovm_reset->Render())});
    });

    auto action_renderer = Renderer(action_component, [&] {
        return vbox({
                   hbox({
                       window(text("Over Voltage Monitor Commands"), action_component_renderer->Render()),
                   }),
               }) |
               flex_grow;
    });

    auto main_container = Container::Horizontal({action_renderer, msg_component_renderer});

    auto main_renderer = Renderer(main_container, [&] {
        std::scoped_lock lock(data_mutex);
        return vbox({
            text("Over Voltage Monitor Bringup") | bold | hcenter,
            hbox({main_container->Render()}),
        });
    });

    screen.Loop(main_renderer);
}

} // namespace module
