/*
 * Standalone setup tool for initial HW calibration
 * -> moving everything to BU module would result in cluttered/unnecessary types+interfaces
 * - used for low level setup and debugging
 */
#include <stdio.h>
#include <string.h>

#include "evSerial.h"
#include <unistd.h>

#include "mMWcar.pb.h"
#include <sigslot/signal.hpp>

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

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <fmt/core.h>

using namespace ftxui;

std::mutex data_mutex;
std::vector<std::vector<std::string>> calibration_table_content = {{"Data", "foo"}};
std::string Ua_gain{"0"}, Ub_gain{"0"}, Uc_gain{"0"}, Ua_offset{"0"}, Ub_offset{"0"}, Uc_offset{"0"},
    reference_voltage{"0.0"};
std::vector<std::string> phase_entries = {"Ua", "Ub", "Uc"};
std::vector<std::string> ac_cal_types = {"Gain", "Offset"};
int phase_selected = 0;
int ac_cal_type_selected = 0;

struct CalibrationData {
    // HW-specific calibration values
    uint16_t atm90_ua_gain{0};
    uint16_t atm90_ub_gain{0};
    uint16_t atm90_uc_gain{0};
    uint16_t atm90_ua_offset{0};
    uint16_t atm90_ub_offset{0};
    uint16_t atm90_uc_offset{0};
} cal_data;

void calibration_table_data(struct CalibrationData& data) {
    calibration_table_content = {{"Ua", "gain", fmt::format("{}", data.atm90_ua_gain)}};
    calibration_table_content.push_back({"Ua", "offset", fmt::format("{}", data.atm90_ua_offset)});
    calibration_table_content.push_back({"Ub", "gain", fmt::format("{}", data.atm90_ub_gain)});
    calibration_table_content.push_back({"Ub", "offset", fmt::format("{}", data.atm90_ub_offset)});
    calibration_table_content.push_back({"Uc", "gain", fmt::format("{}", data.atm90_uc_gain)});
    calibration_table_content.push_back({"Uc", "offset", fmt::format("{}", data.atm90_uc_offset)});
}

void send_new_calibration_values(evSerial& p) {
    CalibrationValues data;
    data.atm90_ua_gain = std::stoi(Ua_gain);
    data.atm90_ub_gain = std::stoi(Ub_gain);
    data.atm90_uc_gain = std::stoi(Uc_gain);
    data.atm90_ua_offset = std::stoi(Ua_offset);
    data.atm90_ub_offset = std::stoi(Ub_offset);
    data.atm90_uc_offset = std::stoi(Uc_offset);
    p.set_calibration_values(data);
}

void start_calibration(evSerial& p) {
    CalibrationRequest req;
    req.which_calibration_type = CalibrationRequest_ac_tag;
    req.calibration_type.ac.phase = static_cast<ACPhase>(phase_selected);
    if (ac_cal_type_selected == 0) {
        req.calibration_type.ac.which_type = AC_Calibration_gain_tag;
        // TODO: read reference voltage from input
        double _reference_voltage = 0;
        try {
            _reference_voltage = std::stof(reference_voltage);
        } catch (...) {
            reference_voltage = "0.0"; // RESET input
            return;
        }
        req.calibration_type.ac.type.gain = _reference_voltage;
    } else {
        req.calibration_type.ac.which_type = AC_Calibration_offset_tag;
    }

    p.request_ac_calibration(req);
}

void help() {
    printf("Usage: ./mMWcar_setup /dev/ttyXXX\n\n");
}

void start_ui(evSerial& p) {
    // build UI
    auto screen = ScreenInteractive::Fullscreen();
    calibration_table_data(cal_data);

    // register/subscribe signals here
    p.signal_calibration_values.connect([&](CalibrationValues v) {
        std::scoped_lock lock(data_mutex);
        cal_data.atm90_ua_gain = v.atm90_ua_gain;
        cal_data.atm90_ua_offset = v.atm90_ua_offset;
        cal_data.atm90_ub_gain = v.atm90_ub_gain;
        cal_data.atm90_ub_offset = v.atm90_ub_offset;
        cal_data.atm90_uc_gain = v.atm90_uc_gain;
        cal_data.atm90_uc_offset = v.atm90_uc_offset;

        Ua_gain = fmt::format("{}", v.atm90_ua_gain);
        Ub_gain = fmt::format("{}", v.atm90_ub_gain);
        Uc_gain = fmt::format("{}", v.atm90_uc_gain);
        Ua_offset = fmt::format("{}", v.atm90_ua_offset);
        Ub_offset = fmt::format("{}", v.atm90_ub_offset);
        Uc_offset = fmt::format("{}", v.atm90_uc_offset);

        screen.Post(Event::Custom);
    });

    p.signal_calibration_response.connect([&](CalibrationResponse v) {
        std::scoped_lock lock(data_mutex);

        ACPhase phase = v.calibration_type.ac.phase;
        uint32_t value = v.value;
        if (v.calibration_type.ac.which_type == AC_Calibration_gain_tag) {
            switch (phase) {
            case ACPhase::ACPhase_Ua:
                Ua_gain = fmt::format("{}", value);
                break;
            case ACPhase::ACPhase_Ub:
                Ub_gain = fmt::format("{}", value);
                break;
            case ACPhase::ACPhase_Uc:
                Uc_gain = fmt::format("{}", value);
                break;
            }
        } else {
            switch (phase) {
            case ACPhase::ACPhase_Ua:
                Ua_offset = fmt::format("{}", value);
                break;
            case ACPhase::ACPhase_Ub:
                Ub_offset = fmt::format("{}", value);
                break;
            case ACPhase::ACPhase_Uc:
                Uc_offset = fmt::format("{}", value);
                break;
            }
        }

        screen.Post(Event::Custom);
    });

    // patch together and run

    /* Calibration values */

    /* Buttons */
    Component get_calibration_button = Button(
        "Get calibration values in EEPROM", [&] { p.get_calibration_values(); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    auto cal_ctrl_component = Container::Horizontal({
        Container::Vertical({
            get_calibration_button,
        }),
    });

    auto calibration_controls_renderer = Renderer(cal_ctrl_component, [&] {
        auto ctrl_win = window(text("Control"), cal_ctrl_component->Render());
        return vbox({
                   hbox({
                       ctrl_win,
                   }) | flex_grow,
               }) |
               flex_grow;
    });

    /* Inputs for manual setting of gain/offset or calibration */
    Component input_Ua_gain = Input(&Ua_gain, "0");
    input_Ua_gain |=
        CatchEvent([&](Event event) { return event.is_character() && !std::isdigit(event.character()[0]); });

    Component input_Ub_gain = Input(&Ub_gain, "0");
    input_Ub_gain |=
        CatchEvent([&](Event event) { return event.is_character() && !std::isdigit(event.character()[0]); });

    Component input_Uc_gain = Input(&Uc_gain, "0");
    input_Uc_gain |=
        CatchEvent([&](Event event) { return event.is_character() && !std::isdigit(event.character()[0]); });

    Component input_Ua_offset = Input(&Ua_offset, "0");
    input_Ua_offset |=
        CatchEvent([&](Event event) { return event.is_character() && !std::isdigit(event.character()[0]); });

    Component input_Ub_offset = Input(&Ub_offset, "0");
    input_Ub_offset |=
        CatchEvent([&](Event event) { return event.is_character() && !std::isdigit(event.character()[0]); });

    Component input_Uc_offset = Input(&Uc_offset, "0");
    input_Uc_offset |=
        CatchEvent([&](Event event) { return event.is_character() && !std::isdigit(event.character()[0]); });

    Component input_reference_voltage_offset = Input(&reference_voltage, "0.0");

    Component set_calibration_button = Button(
        "Set calibration values in EEPROM", [&] { send_new_calibration_values(p); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    Component do_calibration_button = Button(
        "Calibrate phase/type", [&] { start_calibration(p); },
        ButtonOption::Animated(Color::Red, Color::White, Color::RedLight, Color::White));

    auto set_cal_component = Container::Horizontal({set_calibration_button, do_calibration_button});

    auto dropdown_phase = Dropdown(&phase_entries, &phase_selected);
    auto dropdown_ac_type = Dropdown(&ac_cal_types, &ac_cal_type_selected);

    // The component tree:
    auto calibration_input_component = Container::Vertical({
        input_Ua_gain,
        input_Ua_offset,
        input_Ub_gain,
        input_Ub_offset,
        input_Uc_gain,
        input_Uc_offset,
        dropdown_phase,
        dropdown_ac_type,
        input_reference_voltage_offset,
        set_cal_component,
    });

    // Tweak how the component tree is rendered:
    auto calibration_input_renderer = Renderer(calibration_input_component, [&] {
        return vbox({
                   text("Values to commit to MCU"),
                   separator(),
                   hbox(text(" Ua gain   : "), input_Ua_gain->Render()),
                   hbox(text(" Ua offset : "), input_Ua_offset->Render()),
                   hbox(text(" Ub gain   : "), input_Ub_gain->Render()),
                   hbox(text(" Ub offset : "), input_Ub_offset->Render()),
                   hbox(text(" Uc gain   : "), input_Uc_gain->Render()),
                   hbox(text(" Uc offset : "), input_Uc_offset->Render()),
                   separator(),
                   hbox({
                       vbox(text("Phase"), dropdown_phase->Render()) | flex_grow,
                       vbox(text("AC cal. type"), dropdown_ac_type->Render()) | flex_grow,
                   }),
                   hbox(text(" Reference voltage : "), input_reference_voltage_offset->Render()),
                   do_calibration_button->Render(),
                   separator(),
                   set_calibration_button->Render(),
               }) |
               border;
    });

    /* Display of MCU values*/
    auto calibration_display_renderer = Renderer([&] {
        {
            std::scoped_lock lock(data_mutex);
            calibration_table_data(cal_data);
        }

        auto cal_table = Table(calibration_table_content);
        // add borders
        cal_table.SelectColumn(1).Border(LIGHT);
        for (int i = 0; i < calibration_table_content.size(); i++) {
            cal_table.SelectRow(i).Border(LIGHT);
        }

        return window(text("Calibration data MCU"), hbox({cal_table.Render()}) | flex_grow);
    });

    /* Main container*/
    auto main_container = Container::Horizontal(
        {calibration_controls_renderer, calibration_input_renderer, calibration_display_renderer});

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            text("mMW car setup tool") | bold | hcenter,
            main_container->Render(),
        });
    });

    screen.Loop(main_renderer);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        help();
        exit(0);
    }
    const char* device = argv[1];

    evSerial p;

    if (!p.open_device(device, 115200)) {
        printf("Cannot open device \"%s\"\n", device);
    } else {
        p.run();

        start_ui(p);
    }
}