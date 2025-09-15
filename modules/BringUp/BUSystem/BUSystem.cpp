// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "BUSystem.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/table.hpp"

using namespace ftxui;

namespace module {

void BUSystem::init() {
    invoke_init(*p_main);
}

void BUSystem::ready() {
    invoke_ready(*p_main);
    auto screen = ScreenInteractive::Fullscreen();

    r_system->subscribe_firmware_update_status([this, &screen](const types::system::FirmwareUpdateStatus fus) {
        firmware_update_status = types::system::firmware_update_status_enum_to_string(fus.firmware_update_status);
        screen.Post(Event::Custom);
    });

    r_system->subscribe_log_status([this, &screen](const types::system::LogStatus ls) {
        log_status = types::system::log_status_enum_to_string(ls.log_status);
        screen.Post(Event::Custom);
    });

    // ---------------------------------------------------------------------------
    // Commands that can be sent over System interface
    // ---------------------------------------------------------------------------
    Component allow_firmware_installation_button = Button(
        "Allow update",
        [&] {
            last_command = "Allow update";
            r_system->call_allow_firmware_installation();
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::RedLight, Color::White));

    Component get_reset_allowed_button = Button(
        "Is reset allowed",
        [&] {
            last_command = "Is reset allowed";
            auto allowed = r_system->call_is_reset_allowed(types::system::ResetType::Soft);
            is_reset_allowed = (allowed) ? "true" : "false";
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::RedLight, Color::White));

    Component reset_button = Button(
        "Reset",
        [&] {
            last_command = "Reset";
            types::system::ResetType type = types::system::ResetType::Soft;
            bool scheduled = false;
            r_system->call_reset(type, scheduled);
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::RedLight, Color::White));

    Component get_boot_reason_button = Button(
        "Get boot reason",
        [&] {
            last_command = "Get boot reason";
            auto reason = r_system->call_get_boot_reason();
            boot_reason = types::system::boot_reason_to_string(reason);
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::RedLight, Color::White));

    auto cmds_component = Container::Horizontal({
        allow_firmware_installation_button,
        get_reset_allowed_button,
        reset_button,
        get_boot_reason_button,
    });

    auto cmds_renderer = Renderer(cmds_component, [&] {
        auto cmds_win = window(text("Commands"), cmds_component->Render());
        return vbox({
                   hbox({cmds_win}) | center,
                   separator(),
               }) |
               flex_grow;
    });

    auto log_location = Input(&log_upload_location, "Upload location");
    Component log_button = Button(
        "Upload logs",
        [&] {
            last_command = "Upload logs";
            types::system::UploadLogsRequest ulr;
            ulr.location = log_upload_location;
            ulr.retries = 1;
            ulr.retry_interval_s = 1;
            auto result = r_system->call_upload_logs(ulr);
            upload_logs_status =
                fmt::format("Upload status:{}", upload_logs_status_to_string(result.upload_logs_status));
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::RedLight, Color::White));

    auto log_container = Container::Horizontal({
        log_location,
        log_button,
    });

    auto input_log_renderer = Renderer(log_container, [&] {
        return vbox({
                   hbox(text(" Upload logs URL :   "), log_location->Render(), log_button->Render()),
               }) |
               flex_grow;
    });

    auto input_time = Input(&new_system_time, "New system time in RFC3339 format");
    Component time_button = Button(
        "Set system time",
        [&] {
            auto result = r_system->call_set_system_time(new_system_time);
            last_command = (result) ? "Set system time - successful" : "Set system time - unsuccessful";
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::RedLight, Color::White));
    auto input_time_container = Container::Horizontal({
        input_time,
        time_button,
    });

    auto input_time_renderer = Renderer(input_time_container, [&] {
        return vbox({
                   hbox(text(" New system time :   "), input_time->Render(), time_button->Render()),
               }) |
               flex_grow;
    });

    auto input_address = Input(&firmware_update_url, "URL where to take the update from");
    Component update_button = Button(
        "Update firmware",
        [&] {
            last_command = "Update";
            types::system::FirmwareUpdateRequest fur;
            fur.request_id = 1;
            fur.location = firmware_update_url;
            auto result = r_system->call_update_firmware(fur);
            update_firmware_response = update_firmware_response_to_string(result);
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::RedLight, Color::White));
    auto input_container = Container::Horizontal({
        input_address,
        update_button,
    });

    auto input_renderer = Renderer(input_container, [&] {
        return vbox({
                   hbox(text(" URL of the update : "), input_address->Render(), update_button->Render()),
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
            // std::scoped_lock lock(data_mutex);
            table_content = {{"Firmware update status", firmware_update_status},
                             {"Firmware update response", update_firmware_response},
                             {"Get boot reason", boot_reason},
                             {"Upload log status", upload_logs_status},
                             {"Log status", log_status},
                             {"Is reset allowed", is_reset_allowed}};
        }
        auto vars = Table(table_content);

        vars.SelectColumn(0).Border(LIGHT);
        for (int i = 0; i < table_content.size(); i++) {
            vars.SelectRow(i).Border(LIGHT);
        }
        return vbox({
                   hbox({
                       window(text("Information"), vbox({text("Last command: " + last_command), vars.Render()})),
                   }) | center,
               }) |
               flex_grow;
    });

    auto main_container = Container::Vertical({
        input_renderer,
        input_time_renderer,
        input_log_renderer,
        cmds_renderer,
        vars_renderer,
    });

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            text("System Component") | bold | hcenter,
            main_container->Render(),
        });
    });

    screen.Loop(main_renderer);
}

} // namespace module
