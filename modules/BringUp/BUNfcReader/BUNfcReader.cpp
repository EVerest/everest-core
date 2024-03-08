// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <chrono>

#include "BUNfcReader.hpp"
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

static std::vector<std::vector<std::string>> to_table(types::authorization::ProvidedIdToken t) {
    std::vector<std::vector<std::string>> token;
    token.push_back({"Protocol", fmt::format("{}:", id_token_type_to_string(t.id_token.type))});
    token.push_back({"UID", fmt::format("{}", t.id_token.value)});

    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm* p_tm = std::localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(p_tm, "%Y-%m-%d %H:%M:%S");

    token.push_back({"Timestamp", oss.str()});
    return token;
}

void BUNfcReader::init() {
    invoke_init(*p_main);

    types::authorization::ProvidedIdToken t;
    t.id_token.type = types::authorization::IdTokenType::NoAuthorization;
    t.id_token.value = std::string("0");
    last_token = to_table(t);
}

void BUNfcReader::ready() {
    invoke_ready(*p_main);

    auto screen = ScreenInteractive::Fullscreen();

    r_token_provider->subscribe_provided_token([this, &screen](const types::authorization::ProvidedIdToken t) {
        std::scoped_lock lock(data_mutex);
        last_token = to_table(t);
        screen.Post(Event::Custom);
    });

    // ---------------------------------------------------------------------------
    // Vars
    // ---------------------------------------------------------------------------

    auto vars_renderer = Renderer([&] {
        std::vector<std::vector<std::string>> table_content;
        {
            std::scoped_lock lock(data_mutex);
            table_content = last_token;
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
        vars_renderer,
    });

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            text("NFC Token Provider") | bold | hcenter,
            main_container->Render(),
        });
    });

    screen.Loop(main_renderer);
}

} // namespace module
