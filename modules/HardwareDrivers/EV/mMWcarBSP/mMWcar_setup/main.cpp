// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <fstream>
#include <map>
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

struct AdcCalibrationPoint {
    uint32_t adc_raw;
    double reference_measurement;
};

struct AdcCalibrationCoeffs {
    double gain;
    double offset;
};

std::mutex data_mutex;

// static component labels
const std::vector<std::string> phase_entries_labels = {"Ua", "Ub", "Uc"};
const std::vector<std::string> ac_cal_types_labels = {"Gain", "Offset"};
const std::vector<std::string> adc_cal_channels_labels = {"CP", "PP", "DC"};
const std::vector<std::string> adc_cal_num_point_labels = {"1", "2"};

// dynamic content values
CalibrationValues cal_data{0};
std::vector<std::vector<std::string>> calibration_table_content = {{"Data", "foo"}};
std::vector<std::vector<std::string>> adc_cal_point_table_content = {{"", "", "", ""}};
std::vector<std::vector<std::string>> adc_cal_coeffs_table_content = {{"Channel", "Gain", "Offset"}};
std::map<std::string, std::vector<AdcCalibrationPoint>> adc_calibration_points{
    {"CP", {{0}, {0}}},
    {"PP", {{0}, {0}}},
    {"DC", {{0}, {0}}},
};
std::map<std::string, AdcCalibrationCoeffs> adc_calibration_coeffs{
    {"CP", {0}},
    {"PP", {0}},
    {"DC", {0}},
};

std::vector<DacSweepResponse> cp_transfer_function{};
uint16_t proposed_cp_load_9V_dac{0}, proposed_cp_load_6V_dac{0}, proposed_cp_load_3V_dac{0};

static constexpr float CP_LUT_VOLTAGE_STEP_SIZE = 0.1;
static constexpr float CP_LUT_MIN_VOLTAGE = 1.0;
static constexpr float CP_LUT_MAX_VOLTAGE = 11.8;
static constexpr uint8_t CP_LUT_SIZE = (CP_LUT_MAX_VOLTAGE - CP_LUT_MIN_VOLTAGE) / CP_LUT_VOLTAGE_STEP_SIZE;
uint16_t cp_lut[CP_LUT_SIZE]{0};

std::string status_info_str{""};

// Input component values struct
struct GlobalInputComponentValues {
    // AC calibration inputs
    std::string Ua_gain;
    std::string Ub_gain;
    std::string Uc_gain;
    std::string Ua_offset;
    std::string Ub_offset;
    std::string Uc_offset;
    std::string ac_reference_voltage{"0.0"};
    int phase_selected = 0;
    int ac_cal_type_selected = 0;

    // CP temporary calibration inputs
    std::string cp_load_9V_dac;
    std::string cp_load_6V_dac;
    std::string cp_load_3V_dac;

    // ADC channel calibration inputs
    std::string adc_reference_voltage{"0.0"};
    int adc_cal_channel_selected{0};
    int adc_cal_num_point_selected{0};

    // ADC channel calibration coeffs to/from MCU
    std::string adc_cp_gain;
    std::string adc_cp_offset;
    bool adc_cp_use_cal;
    std::string adc_pp_gain;
    std::string adc_pp_offset;
    bool adc_pp_use_cal;
    std::string adc_dc_gain;
    std::string adc_dc_offset;
    bool adc_dc_use_cal;
} global_input_component_vals;

const ButtonOption button_color_scheme =
    ButtonOption::Animated(Color::Grey19, Color::White, Color::Grey35, Color::White);

void calibration_table_data(CalibrationValues& data) {
    // AC measurement calibration
    calibration_table_content = {{"Ua", "gain", fmt::format("{}", data.atm90_ua_gain)}};
    calibration_table_content.push_back({"Ua", "offset", fmt::format("{}", data.atm90_ua_offset)});
    calibration_table_content.push_back({"Ub", "gain", fmt::format("{}", data.atm90_ub_gain)});
    calibration_table_content.push_back({"Ub", "offset", fmt::format("{}", data.atm90_ub_offset)});
    calibration_table_content.push_back({"Uc", "gain", fmt::format("{}", data.atm90_uc_gain)});
    calibration_table_content.push_back({"Uc", "offset", fmt::format("{}", data.atm90_uc_offset)});
    // JFET calibration values only used for non-linearized HW Rev.
    calibration_table_content.push_back({"CP", "State B/9V", fmt::format("{}", data.cp_load_9V_dac_val)});
    calibration_table_content.push_back({"CP", "State C/6V", fmt::format("{}", data.cp_load_6V_dac_val)});
    calibration_table_content.push_back({"CP", "State D/3V", fmt::format("{}", data.cp_load_3V_dac_val)});
    // ADC channel calibration values
    calibration_table_content.push_back({"ADC", "CP Gain", fmt::format("{}", data.adc_cp_gain)});
    calibration_table_content.push_back({"ADC", "CP Offset", fmt::format("{}", data.adc_cp_offset)});
}

void adc_cal_point_table_data() {
    adc_cal_point_table_content = {{"Channel", "Point#", "ADC raw", "Reference voltage"}};
    for (auto const& [key, val] : adc_calibration_points) {
        int point_num = 1;
        for (auto const& point : val) {
            adc_cal_point_table_content.push_back({key, fmt::format("{}", point_num), fmt::format("{}", point.adc_raw),
                                                   fmt::format("{} V", point.reference_measurement)});
            point_num++;
        }
    }
}

void adc_cal_coeffs_table_data() {
    adc_cal_coeffs_table_content = {{"Channel", "Gain", "Offset"}};
    for (auto const& [key, val] : adc_calibration_coeffs) {
        adc_cal_coeffs_table_content.push_back({key, fmt::format("{}", val.gain), fmt::format("{}", val.offset)});
    }
}

void send_new_calibration_values(evSerial& p) {
    CalibrationValues data;
    data.atm90_ua_gain = std::stoi(global_input_component_vals.Ua_gain);
    data.atm90_ub_gain = std::stoi(global_input_component_vals.Ub_gain);
    data.atm90_uc_gain = std::stoi(global_input_component_vals.Uc_gain);
    data.atm90_ua_offset = std::stoi(global_input_component_vals.Ua_offset);
    data.atm90_ub_offset = std::stoi(global_input_component_vals.Ub_offset);
    data.atm90_uc_offset = std::stoi(global_input_component_vals.Uc_offset);
    data.cp_load_9V_dac_val = std::stoi(global_input_component_vals.cp_load_9V_dac);
    data.cp_load_6V_dac_val = std::stoi(global_input_component_vals.cp_load_6V_dac);
    data.cp_load_3V_dac_val = std::stoi(global_input_component_vals.cp_load_3V_dac);
    data.adc_cp_gain = std::stod(global_input_component_vals.adc_cp_gain);
    data.adc_cp_offset = std::stod(global_input_component_vals.adc_cp_offset);
    data.adc_cp_use_cal = global_input_component_vals.adc_cp_use_cal;
    data.adc_pp_gain = std::stod(global_input_component_vals.adc_pp_gain);
    data.adc_pp_offset = std::stod(global_input_component_vals.adc_pp_offset);
    data.adc_pp_use_cal = global_input_component_vals.adc_pp_use_cal;
    data.adc_dc_gain = std::stod(global_input_component_vals.adc_dc_gain);
    data.adc_dc_offset = std::stod(global_input_component_vals.adc_dc_offset);
    data.adc_dc_use_cal = global_input_component_vals.adc_dc_use_cal;
    p.set_calibration_values(data);
}

void start_ac_calibration(evSerial& p) {
    CalibrationRequest req;
    req.which_calibration_type = CalibrationRequest_ac_tag;
    req.calibration_type.ac.phase = static_cast<ACPhase>(global_input_component_vals.phase_selected);
    if (global_input_component_vals.ac_cal_type_selected == 0) {
        req.calibration_type.ac.which_type = AC_Calibration_gain_tag;
        // TODO: read reference voltage from input
        double _reference_voltage = 0;
        try {
            _reference_voltage = std::stof(global_input_component_vals.ac_reference_voltage);
        } catch (...) {
            global_input_component_vals.ac_reference_voltage = "0.0"; // RESET input
            return;
        }
        req.calibration_type.ac.type.gain = _reference_voltage;
    } else {
        req.calibration_type.ac.which_type = AC_Calibration_offset_tag;
    }

    p.request_ac_calibration(req);
}

void get_adc_calibration_point(evSerial& p) {
    Adc_Raw req;
    req.channel = static_cast<Adc_Channel>(global_input_component_vals.adc_cal_channel_selected);
    p.get_adc_raw(req);
}

void calculate_adc_calibration_coeffs() {
    auto& channel = adc_cal_channels_labels[global_input_component_vals.adc_cal_channel_selected];
    std::vector<AdcCalibrationPoint>& points = adc_calibration_points[channel];

    double gain = (points[1].reference_measurement - points[0].reference_measurement) /
                  ((double)points[1].adc_raw - (double)points[0].adc_raw);
    double offset = points[1].reference_measurement - gain * points[1].adc_raw;

    adc_calibration_coeffs[channel].gain = gain;
    adc_calibration_coeffs[channel].offset = offset;
}

void transfer_adc_cal_to_new_calibration_values(const int channel_idx) {
    auto& channel = adc_cal_channels_labels[channel_idx];
    double& gain = adc_calibration_coeffs[channel].gain;
    double& offset = adc_calibration_coeffs[channel].offset;

    switch (channel_idx) {
    case 0:
        global_input_component_vals.adc_cp_gain = fmt::format("{}", gain);
        global_input_component_vals.adc_cp_offset = fmt::format("{}", offset);
        break;
    case 1:
        global_input_component_vals.adc_pp_gain = fmt::format("{}", gain);
        global_input_component_vals.adc_pp_offset = fmt::format("{}", offset);
        break;
    case 2:
        global_input_component_vals.adc_dc_gain = fmt::format("{}", gain);
        global_input_component_vals.adc_dc_offset = fmt::format("{}", offset);
        break;
    default:
        break;
    }
}

uint16_t find_nearest_cp_index(const std::vector<DacSweepResponse>& tf_table, double target_voltage) {
    uint16_t min_idx = 0;
    double min_error = 10000;

    for (uint16_t i = 0; i < tf_table.size(); i++) {
        double error = std::fabs(target_voltage - tf_table[i].cp_voltage);
        if (error < min_error) {
            min_idx = i;
            min_error = error;
        }
    }

    return tf_table[min_idx].dac_input;
}

void calculate_cp_cal_nominal() {
    if (cp_transfer_function.size() != 4096)
        return;

    global_input_component_vals.cp_load_3V_dac = fmt::format("{}", find_nearest_cp_index(cp_transfer_function, 3.0));
    global_input_component_vals.cp_load_6V_dac = fmt::format("{}", find_nearest_cp_index(cp_transfer_function, 6.0));
    global_input_component_vals.cp_load_9V_dac = fmt::format("{}", find_nearest_cp_index(cp_transfer_function, 9.0));
}

void calculate_cp_LUT() {
    if (cp_transfer_function.size() != 4096)
        return;

    // DEBUG
    std::ofstream lut_file;
    lut_file.open("lut.csv", std::ios::out | std::ios::trunc);
    for (uint8_t i = 0; i < CP_LUT_SIZE; i++) {
        float voltage = CP_LUT_MIN_VOLTAGE + i * CP_LUT_VOLTAGE_STEP_SIZE;
        cp_lut[i] = find_nearest_cp_index(cp_transfer_function, voltage);
        lut_file << fmt::format("{},{},{}\n", i, voltage, cp_lut[i]);
    }
    lut_file.close();
}

void read_tf_csv() {
    std::ifstream tf_file;
    try {
        tf_file.open("tf.csv");
        cp_transfer_function = {};
        std::string row;
        uint8_t idx = 0;
        while (not tf_file.eof()) {
            std::getline(tf_file, row);
            std::stringstream ss(row);
            std::vector<std::string> items = {};
            while (ss.good()) {
                std::string item;
                std::getline(ss, item, ',');
                items.push_back(item);
            }

            DacSweepResponse v;
            v.dac_input = std::stod(items[0]);
            v.cp_voltage = std::stod(items[1]);
            cp_transfer_function.push_back(v);
            idx++;
        }
    } catch (...) {
    };
}

void send_cp_lut(evSerial& p) {
    CpLUT data;
    for (uint8_t i = 0; i < CP_LUT_SIZE; i++) {
        data.dac_values[i] = cp_lut[i];
    }
    data.dac_values_count = CP_LUT_SIZE;
    p.set_cp_lut(data);
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
        cal_data = v;

        global_input_component_vals.Ua_gain = fmt::format("{}", cal_data.atm90_ua_gain);
        global_input_component_vals.Ub_gain = fmt::format("{}", cal_data.atm90_ub_gain);
        global_input_component_vals.Uc_gain = fmt::format("{}", cal_data.atm90_uc_gain);
        global_input_component_vals.Ua_offset = fmt::format("{}", cal_data.atm90_ua_offset);
        global_input_component_vals.Ub_offset = fmt::format("{}", cal_data.atm90_ub_offset);
        global_input_component_vals.Uc_offset = fmt::format("{}", cal_data.atm90_uc_offset);

        global_input_component_vals.cp_load_9V_dac = fmt::format("{}", cal_data.cp_load_9V_dac_val);
        global_input_component_vals.cp_load_6V_dac = fmt::format("{}", cal_data.cp_load_6V_dac_val);
        global_input_component_vals.cp_load_3V_dac = fmt::format("{}", cal_data.cp_load_3V_dac_val);

        global_input_component_vals.adc_cp_gain = fmt::format("{}", cal_data.adc_cp_gain);
        global_input_component_vals.adc_cp_offset = fmt::format("{}", cal_data.adc_cp_offset);
        global_input_component_vals.adc_cp_use_cal = cal_data.adc_cp_use_cal;
        global_input_component_vals.adc_pp_gain = fmt::format("{}", cal_data.adc_pp_gain);
        global_input_component_vals.adc_pp_offset = fmt::format("{}", cal_data.adc_pp_offset);
        global_input_component_vals.adc_pp_use_cal = cal_data.adc_pp_use_cal;
        global_input_component_vals.adc_dc_gain = fmt::format("{}", cal_data.adc_dc_gain);
        global_input_component_vals.adc_dc_offset = fmt::format("{}", cal_data.adc_dc_offset);
        global_input_component_vals.adc_dc_use_cal = cal_data.adc_dc_use_cal;

        screen.Post(Event::Custom);
    });

    p.signal_calibration_response.connect([&](CalibrationResponse v) {
        std::scoped_lock lock(data_mutex);

        ACPhase phase = v.calibration_type.ac.phase;
        uint32_t value = v.value;
        if (v.calibration_type.ac.which_type == AC_Calibration_gain_tag) {
            switch (phase) {
            case ACPhase::ACPhase_Ua:
                global_input_component_vals.Ua_gain = fmt::format("{}", value);
                break;
            case ACPhase::ACPhase_Ub:
                global_input_component_vals.Ub_gain = fmt::format("{}", value);
                break;
            case ACPhase::ACPhase_Uc:
                global_input_component_vals.Uc_gain = fmt::format("{}", value);
                break;
            }
        } else {
            switch (phase) {
            case ACPhase::ACPhase_Ua:
                global_input_component_vals.Ua_offset = fmt::format("{}", value);
                break;
            case ACPhase::ACPhase_Ub:
                global_input_component_vals.Ub_offset = fmt::format("{}", value);
                break;
            case ACPhase::ACPhase_Uc:
                global_input_component_vals.Uc_offset = fmt::format("{}", value);
                break;
            }
        }

        screen.Post(Event::Custom);
    });

    p.signal_adc_raw_response.connect([&](Adc_Raw v) {
        auto channel = adc_cal_channels_labels[global_input_component_vals.adc_cal_channel_selected];
        adc_calibration_points[channel][global_input_component_vals.adc_cal_num_point_selected].adc_raw = v.value;
        adc_calibration_points[channel][global_input_component_vals.adc_cal_num_point_selected].reference_measurement =
            std::stof(global_input_component_vals.adc_reference_voltage);
    });

    p.signal_sweep_cp_response.connect([&](DacSweepResponse v) {
        cp_transfer_function.push_back(v);
        // log to CSV for now
        if (cp_transfer_function.size() == 4096) {
            std::ofstream csv_file;
            csv_file.open("tf.csv", std::ios::out | std::ios::trunc);
            for (const auto& entry : cp_transfer_function) {
                csv_file << fmt::format("{},{}\n", entry.dac_input, entry.cp_voltage);
            }
            csv_file.close();
        }
        screen.Post(Event::Custom);
    });

    p.signal_keep_alive.connect([&](KeepAlive v) {
        std::scoped_lock lock(data_mutex);
        status_info_str = fmt::format("Timestamp: {}", v.time_stamp);

        screen.Post(Event::Custom);
    });

    // patch together and run

    /* Controls for getting calibration values and setting CP states */
    Component get_calibration_button = Button(
        "Get calibration values in EEPROM", [&] { p.get_calibration_values(); }, button_color_scheme);

    Component set_calibration_button = Button(
        "Set calibration values in EEPROM", [&] { send_new_calibration_values(p); }, button_color_scheme);

    Component set_state_A_button = Button(
        "Set State A", [&] { p.set_cp_state(CpState_STATE_A); }, button_color_scheme);

    Component set_state_B_button = Button(
        "Set State B", [&] { p.set_cp_state(CpState_STATE_B); }, button_color_scheme);

    Component set_state_C_button = Button(
        "Set State C", [&] { p.set_cp_state(CpState_STATE_C); }, button_color_scheme);

    Component set_state_D_button = Button(
        "Set State D", [&] { p.set_cp_state(CpState_STATE_D); }, button_color_scheme);

    // The component tree:
    auto main_control_component = Container::Vertical({
        get_calibration_button,
        set_calibration_button,
        set_state_A_button,
        set_state_B_button,
        set_state_C_button,
        set_state_D_button,
    });

    // Tweak how the component tree is rendered:
    auto main_control_renderer = Renderer(main_control_component, [&] {
        return window(text("Main controls") | bold, vbox({
                                                        get_calibration_button->Render(),
                                                        set_calibration_button->Render(),
                                                        separator(),
                                                        set_state_A_button->Render(),
                                                        set_state_B_button->Render(),
                                                        set_state_C_button->Render(),
                                                        set_state_D_button->Render(),
                                                    }));
    });

    /* Inputs for manual setting of gain/offset or calibration */
    Component input_Ua_gain = Input(&global_input_component_vals.Ua_gain, "N/A");
    Component input_Ub_gain = Input(&global_input_component_vals.Ub_gain, "N/A");
    Component input_Uc_gain = Input(&global_input_component_vals.Uc_gain, "N/A");
    Component input_Ua_offset = Input(&global_input_component_vals.Ua_offset, "N/A");
    Component input_Ub_offset = Input(&global_input_component_vals.Ub_offset, "N/A");
    Component input_Uc_offset = Input(&global_input_component_vals.Uc_offset, "N/A");

    Component input_cp_load_9V_dac_value = Input(&global_input_component_vals.cp_load_9V_dac, "N/A");
    Component input_cp_load_6V_dac_value = Input(&global_input_component_vals.cp_load_6V_dac, "N/A");
    Component input_cp_load_3V_dac_value = Input(&global_input_component_vals.cp_load_3V_dac, "N/A");

    Component input_adc_cp_gain = Input(&global_input_component_vals.adc_cp_gain, "N/A");
    Component input_adc_cp_offset = Input(&global_input_component_vals.adc_cp_offset, "N/A");
    Component input_adc_cp_use_cal = Checkbox("", &global_input_component_vals.adc_cp_use_cal);
    Component input_adc_pp_gain = Input(&global_input_component_vals.adc_pp_gain, "N/A");
    Component input_adc_pp_offset = Input(&global_input_component_vals.adc_pp_offset, "N/A");
    Component input_adc_pp_use_cal = Checkbox("", &global_input_component_vals.adc_pp_use_cal);
    Component input_adc_dc_gain = Input(&global_input_component_vals.adc_dc_gain, "N/A");
    Component input_adc_dc_offset = Input(&global_input_component_vals.adc_dc_offset, "N/A");
    Component input_adc_dc_use_cal = Checkbox("", &global_input_component_vals.adc_dc_use_cal);

    // apply input filter to integer only inputs
    {
        std::vector<Component*> integer_inputs = {
            &input_Ua_gain,
            &input_Ub_gain,
            &input_Uc_gain,
            &input_Ua_offset,
            &input_Ub_offset,
            &input_Uc_offset,
            &input_cp_load_9V_dac_value,
            &input_cp_load_6V_dac_value,
            &input_cp_load_3V_dac_value,
        };

        for (const auto& i : integer_inputs) {
            *i |= CatchEvent([&](Event event) { return event.is_character() && !std::isdigit(event.character()[0]); });
        }
    }

    // float inputs
    Component input_ac_reference_voltage_offset = Input(&global_input_component_vals.ac_reference_voltage, "0.0");
    Component input_adc_reference_voltage = Input(&global_input_component_vals.adc_reference_voltage, "0.0");

    // calibration buttons
    Component do_ac_calibration_button = Button(
        "Calibrate phase/type", [&] { start_ac_calibration(p); }, button_color_scheme);

    Component get_adc_calibration_point_button = Button(
        "Get ADC calibration point", [&] { get_adc_calibration_point(p); }, button_color_scheme);

    Component calculate_adc_calibration_coeffs_button = Button(
        "Calculate ADC calibration coeffs", [&] { calculate_adc_calibration_coeffs(); }, button_color_scheme);

    Component copy_cp_coeffs_button = Button(
        "Copy CP coeffs to commit values", [&] { transfer_adc_cal_to_new_calibration_values(0); }, button_color_scheme);

    Component copy_pp_coeffs_button = Button(
        "Copy PP coeffs to commit values", [&] { transfer_adc_cal_to_new_calibration_values(1); }, button_color_scheme);

    Component copy_dc_coeffs_button = Button(
        "Copy DC coeffs to commit values", [&] { transfer_adc_cal_to_new_calibration_values(2); }, button_color_scheme);

    Component sweep_cp_button = Button(
        "Sweep CP DAC output",
        [&] {
            cp_transfer_function = {};
            p.sweep_cp();
        },
        button_color_scheme);

    Component calibrate_cp_nominal_button = Button(
        "Calculate nominal state calibration values",
        [&] {
            calculate_cp_cal_nominal();
            screen.Post(Event::Custom);
        },
        button_color_scheme);

    Component calculate_cp_lut_button = Button(
        "Calculate CP LUT",
        [&] {
            calculate_cp_LUT();
            screen.Post(Event::Custom);
        },
        button_color_scheme);

    Component read_cp_tf_csv_button = Button(
        "Read CP from tf.csv file",
        [&] {
            read_tf_csv();
            screen.Post(Event::Custom);
        },
        button_color_scheme);

    Component send_cp_lut_button = Button(
        "Send CP LUT to MCU", [&] { send_cp_lut(p); }, button_color_scheme);

    // calibration drop-downs
    auto dropdown_phase = Dropdown(&phase_entries_labels, &global_input_component_vals.phase_selected);
    auto dropdown_ac_type = Dropdown(&ac_cal_types_labels, &global_input_component_vals.ac_cal_type_selected);

    auto dropdown_adc_channel =
        Dropdown(&adc_cal_channels_labels, &global_input_component_vals.adc_cal_channel_selected);
    auto dropdown_adc_num_point =
        Dropdown(&adc_cal_num_point_labels, &global_input_component_vals.adc_cal_num_point_selected);

    // The component tree:
    auto calibration_input_component = Container::Vertical({
        // AC raw inputs
        input_Ua_gain,
        input_Ua_offset,
        input_Ub_gain,
        input_Ub_offset,
        input_Uc_gain,
        input_Uc_offset,
        // CP load
        input_cp_load_9V_dac_value,
        input_cp_load_6V_dac_value,
        input_cp_load_3V_dac_value,
        // ADC calibration values
        input_adc_cp_gain,
        input_adc_cp_offset,
        input_adc_pp_gain,
        input_adc_pp_offset,
        input_adc_dc_gain,
        input_adc_dc_offset,
        input_adc_cp_use_cal,
        input_adc_pp_use_cal,
        input_adc_dc_use_cal,
        // AC calibration methods
        dropdown_phase,
        dropdown_ac_type,
        input_ac_reference_voltage_offset,
        do_ac_calibration_button,
        // ADC calibration methods
        dropdown_adc_channel,
        dropdown_adc_num_point,
        get_adc_calibration_point_button,
        calculate_adc_calibration_coeffs_button,
        input_adc_reference_voltage,
        // Copy cal values to commit inputs buttons
        copy_cp_coeffs_button,
        copy_pp_coeffs_button,
        copy_dc_coeffs_button,
        // CP transfer function
        read_cp_tf_csv_button,
        sweep_cp_button,
        calibrate_cp_nominal_button,
        calculate_cp_lut_button,
        send_cp_lut_button,
    });

    // Tweak how the component tree is rendered:
    auto calibration_input_renderer = Renderer(calibration_input_component, [&] {
        return vbox({
                   text("Values to commit to MCU"),
                   separator(),
                   hbox(text("AC measurements") | bold),
                   hbox(text(" Ua gain      : "), input_Ua_gain->Render()),
                   hbox(text(" Ua offset    : "), input_Ua_offset->Render()),
                   hbox(text(" Ub gain      : "), input_Ub_gain->Render()),
                   hbox(text(" Ub offset    : "), input_Ub_offset->Render()),
                   hbox(text(" Uc gain      : "), input_Uc_gain->Render()),
                   hbox(text(" Uc offset    : "), input_Uc_offset->Render()),
                   hbox(text("JFET load") | bold),
                   hbox(text(" CP 9V DACout : "), input_cp_load_9V_dac_value->Render()),
                   hbox(text(" CP 6V DACout : "), input_cp_load_6V_dac_value->Render()),
                   hbox(text(" CP 3V DACout : "), input_cp_load_3V_dac_value->Render()),
                   hbox(text("ADC channels") | bold),
                   hbox(text(" CP gain      : "), input_adc_cp_gain->Render()),
                   hbox(text(" CP offset    : "), input_adc_cp_offset->Render()),
                   hbox(text(" PP gain      : "), input_adc_pp_gain->Render()),
                   hbox(text(" PP offset    : "), input_adc_pp_offset->Render()),
                   hbox(text(" DC gain      : "), input_adc_dc_gain->Render()),
                   hbox(text(" DC offset    : "), input_adc_dc_offset->Render()),
                   hbox(text(" Use CP cal.  : "), input_adc_cp_use_cal->Render()),
                   hbox(text(" Use PP cal.  : "), input_adc_pp_use_cal->Render()),
                   hbox(text(" Use DC cal.  : "), input_adc_dc_use_cal->Render()),
                   separator(),
                   hbox(text("AC calibration") | bold),
                   hbox({
                       vbox(text("Phase"), dropdown_phase->Render()) | flex_grow,
                       vbox(text("AC cal. type"), dropdown_ac_type->Render()) | flex_grow,
                   }),
                   hbox(text(" Reference voltage : "), input_ac_reference_voltage_offset->Render()),
                   do_ac_calibration_button->Render(),
                   separator(),
                   hbox(text("ADC calibration") | bold),
                   hbox({
                       vbox(text("Channel"), dropdown_adc_channel->Render()) | flex_grow,
                       vbox(text("Point#"), dropdown_adc_num_point->Render()) | flex_grow,
                   }),
                   hbox(text(" Reference voltage : "), input_adc_reference_voltage->Render()),
                   get_adc_calibration_point_button->Render(),
                   calculate_adc_calibration_coeffs_button->Render(),
                   // separator(),
                   // set_calibration_button->Render(),
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

        return window(text("Calibration data MCU") | bold, hbox({cal_table.Render()}) | flex_shrink);
    });

    /* Display of ADC calibration points */
    auto adc_cal_point_display_renderer = Renderer([&] {
        {
            std::scoped_lock lock(data_mutex);
            adc_cal_point_table_data();
        }

        auto table = Table(adc_cal_point_table_content);
        // add borders
        for (int cols = 1; cols < 3; cols++) {
            table.SelectColumn(cols).Border(LIGHT);
            for (int i = 0; i < calibration_table_content.size(); i++) {
                table.SelectRow(i).Border(LIGHT);
            }
        }

        return window(text("Calibration points") | bold, hbox({table.Render()}) | flex_grow);
    });

    /* Display of ADC calibration points */
    auto adc_cal_coeffs_display_renderer = Renderer([&] {
        {
            std::scoped_lock lock(data_mutex);
            adc_cal_coeffs_table_data();
        }

        auto table = Table(adc_cal_coeffs_table_content);
        // add borders
        for (int cols = 1; cols < 3; cols++) {
            table.SelectColumn(cols).Border(LIGHT);
            for (int i = 0; i < calibration_table_content.size(); i++) {
                table.SelectRow(i).Border(LIGHT);
            }
        }

        return window(text("Calibration coeffs") | bold, hbox({
                                                             table.Render(),
                                                         }) | flex_grow);
    });

    auto adc_cal_display_renderer = Renderer(calibration_input_component, [&] {
        return window(text("ADC calibration"), hbox({
                                                   vbox({
                                                       adc_cal_point_display_renderer->Render() | flex,
                                                       adc_cal_coeffs_display_renderer->Render() | flex,
                                                   }),
                                                   vbox({
                                                       copy_cp_coeffs_button->Render(),
                                                       separator(),
                                                       copy_pp_coeffs_button->Render(),
                                                       separator(),
                                                       copy_dc_coeffs_button->Render(),
                                                   }) | border,
                                               }) | flex_shrink);
    });

    /* CP transfer function */
    auto cp_transfer_display_renderer = Renderer(calibration_input_component, [&] {
        return window(text("JFET transfer function"),
                      hbox({
                          vbox({
                              text(fmt::format("Transfer function size: {}", cp_transfer_function.size())),
                              read_cp_tf_csv_button->Render(),
                              sweep_cp_button->Render(),
                              separator(),
                              calibrate_cp_nominal_button->Render(),
                              calculate_cp_lut_button->Render(),
                              send_cp_lut_button->Render(),
                          }),
                          vbox({

                          }),
                      }) | flex_shrink);
    });

    /* Status bar */
    auto status_bar_renderer = Renderer([&] {
        return hbox({
                   text(status_info_str),
               }) |
               border;
    });

    /* Main container*/
    auto main_container = Container::Horizontal({
        main_control_renderer,
        calibration_input_renderer,
        //            calibration_display_renderer,
        adc_cal_display_renderer,
        cp_transfer_display_renderer,
    });

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
                   text("mMW car setup tool") | bold | hcenter,
                   main_container->Render(),
                   status_bar_renderer->Render(),
               }) |
               flex_grow;
    });

    p.get_calibration_values();

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