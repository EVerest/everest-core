// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "BUSlac.hpp"

#include "ftxui/dom/table.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace module {

void BUSlac::init() {
    invoke_init(*p_main);
}

void BUSlac::ready() {
    invoke_ready(*p_main);

    auto screen = ScreenInteractive::Fullscreen();

    r_slac->subscribe_state([this, &screen](const std::string& new_state) {
        {
            std::scoped_lock lock(data_mutex);
            state = new_state;
        }
        screen.PostEvent(Event::Custom);
    });

    r_slac->subscribe_dlink_ready([this, &screen](const bool& is_ready) {
        {
            std::scoped_lock lock(data_mutex);
            dlink = is_ready ? "true" : "false";
        }
        screen.PostEvent(Event::Custom);
    });

    r_slac->subscribe_request_error_routine([this, &screen]() {
        {
            std::scoped_lock lock(data_mutex);
            last_request_error_routine_timestamp = std::to_string(
                std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
                    .count());
        }
        screen.PostEvent(Event::Custom);
    });

    r_slac->subscribe_ev_mac_address([this, &screen](const std::string& mac) {
        {
            std::scoped_lock lock(data_mutex);
            ev_mac_address = mac;
        }
        screen.PostEvent(Event::Custom);
    });

    // -------------------------------------------------------------------
    // Left column (Var Display)
    // -------------------------------------------------------------------
    auto data_renderer = Renderer([&] {
        std::vector<std::vector<std::string>> table_content;

        {
            std::scoped_lock lock(data_mutex);
            table_content = {
                {"State", state},
                {"DLink Ready", dlink},
                {"Last Request Error Routine", last_request_error_routine_timestamp},
                {"EV MAC Address", ev_mac_address},
            };
        }

        auto table = Table(table_content);

        table.SelectAll().Border(LIGHT);
        table.SelectColumn(0).Border(LIGHT);
        for (int i = 0; i < (int)table_content.size(); ++i)
            table.SelectRow(i).Border(LIGHT);

        return vbox({
                   window(text("Module Data"), vbox({
                                                   table.Render(),
                                               })) |
                       size(WIDTH, EQUAL, 40),
               }) |
               flex_grow;
    });

    // -------------------------------------------------------------------
    // Right column (Command Buttons)
    // -------------------------------------------------------------------

    auto button_start_slac = Button("Start SLAC", [&] { r_slac->call_reset(true); });
    auto button_stop_slac = Button("Stop SLAC", [&] { r_slac->call_reset(false); });
    auto button_enter_bcd = Button("Enter BCD", [&] { r_slac->call_enter_bcd(); });
    auto button_leave_bcd = Button("Leave BCD", [&] { r_slac->call_leave_bcd(); });
    auto button_dlink_terminate = Button("DLink Terminate", [&] { r_slac->call_dlink_terminate(); });
    auto button_dlink_error = Button("DLink Error", [&] { r_slac->call_dlink_error(); });
    auto button_dlink_pause = Button("DLink Pause", [&] { r_slac->call_dlink_pause(); });

    // Compose the command panel layout
    auto command_container = Container::Vertical({
        Container::Horizontal({button_start_slac, button_stop_slac}),
        button_enter_bcd,
        button_leave_bcd,
        button_dlink_terminate,
        button_dlink_error,
        button_dlink_pause,
    });

    auto command_renderer = Renderer(command_container, [&] {
        return vbox({
                   text("Commands") | bold | center,
                   separator(),
                   button_start_slac->Render(),
                   button_stop_slac->Render(),
                   button_enter_bcd->Render(),
                   button_leave_bcd->Render(),
                   button_dlink_terminate->Render(),
                   button_dlink_error->Render(),
                   button_dlink_pause->Render(),
               }) |
               border | size(WIDTH, EQUAL, 40);
    });

    // -------------------------------------------------------------------
    // Combine columns
    // -------------------------------------------------------------------
    auto layout = Container::Horizontal({
        data_renderer,
        command_renderer,
    });

    auto main_renderer = Renderer(layout, [&] {
        return vbox({
            text("SLAC BringUp") | bold | center,
            separator(),
            hbox({
                data_renderer->Render(),
                command_renderer->Render(),
            }),
        });
    });

    screen.Loop(main_renderer);
}

} // namespace module
