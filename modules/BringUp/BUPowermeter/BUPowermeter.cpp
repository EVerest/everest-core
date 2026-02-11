// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "BUPowermeter.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/table.hpp"
#include <ranges>

using namespace ftxui;

namespace module {

void optional_add(std::vector<std::vector<std::string>>& table, std::string text, std::string s_value) {
    table.push_back({text, s_value});
}

void optional_add(std::vector<std::vector<std::string>>& table, std::string text, std::optional<float> o_value) {
    if (o_value.has_value()) {
        table.push_back({text, std::to_string(o_value.value())});
    }
}

void optional_add(std::vector<std::vector<std::string>>& table, std::string text, std::optional<std::string> o_value) {
    if (o_value.has_value()) {
        table.push_back({text, o_value.value()});
    }
}

void BUPowermeter::init() {
    invoke_init(*p_main);
}

// Helper function to wrap long text into multiple lines
std::vector<std::string> WrapText(const std::string& text, size_t max_width) {
    std::vector<std::string> lines;
    size_t start = 0;

    if (max_width < text.size()) {
        while (start < text.size()) {
            size_t end = std::min(start + max_width, text.size());
            size_t last_space = text.rfind(' ', end);

            // If there's no space, split at max_width
            if (last_space == std::string::npos || last_space < start) {
                lines.push_back(text.substr(start, max_width));
                start += max_width;
            } else {
                lines.push_back(text.substr(start, last_space - start));
                start = last_space + 1; // Skip the space
            }
        }
    } else {
        lines.push_back(text);
    }

    return lines;
}

// Helper function to transform text strings into FTXUI Elements
std::vector<Element> WrapTextToElements(const std::vector<std::string>& strings) {
    std::vector<Element> elements;
    elements.reserve(strings.size()); // Preallocate memory

    // Use std::transform to convert strings to FTXUI elements
    std::transform(strings.begin(), strings.end(), std::back_inserter(elements),
                   [](const std::string& line) { return text(line); } // Convert each string to FTXUI text
    );

    return elements;
}

// Helper function to format and render the table content
std::vector<std::vector<Element>> FormatTableContent(const std::vector<std::vector<std::string>>& raw_table_content,
                                                     size_t max_width) {
    std::vector<std::vector<Element>> table_elements;

    for (const auto& row : raw_table_content) {
        std::vector<Element> formatted_row;

        for (const auto& cell : row) {
            // Wrap long strings into multiple lines
            std::vector<std::string> wrapped_lines = WrapText(cell, max_width);

            // Convert wrapped lines into FTXUI text elements
            auto cell_element = vbox(WrapTextToElements(wrapped_lines));

            formatted_row.push_back(cell_element);
        }

        table_elements.push_back(std::move(formatted_row));
    }

    return table_elements;
}

void BUPowermeter::ready() {
    invoke_ready(*p_main);
    auto screen = ScreenInteractive::Fullscreen();

    r_powermeter->subscribe_public_key_ocmf([this, &screen](std::string pk) {
        public_key = pk;
        last_event_received = event_received;
        event_received = std::chrono::steady_clock::now();
        screen.Post(Event::Custom);
    });

    r_powermeter->subscribe_powermeter([this, &screen](types::powermeter::Powermeter pm) {
        powermeter = pm;
        last_event_received = event_received;
        event_received = std::chrono::steady_clock::now();
        screen.Post(Event::Custom);
    });

    // ---------------------------------------------------------------------------
    // Commands that can be sent over Powermeter interface
    // ---------------------------------------------------------------------------
    Component start_transaction_button = Button(
        "Start transaction",
        [&] {
            types::powermeter::OCMFUserIdentificationStatus
                identification_status; ///< OCMF Identification Status (IS): General status for user assignment
            std::vector<types::powermeter::OCMFIdentificationFlags>
                identification_flags; ///< OCMF Identification Flags (IF): Detailed statements about the user
                                      ///< assignment, represented by one or more identifiers
            types::powermeter::OCMFIdentificationType
                identification_type; ///< OCMF Identification Type (IT): Type of identification data
            std::optional<types::powermeter::OCMFIdentificationLevel>
                identification_level; ///< OCMF Identification Level (IL): Encoded overall status of the user assignment
            std::optional<std::string>
                identification_data; ///< OCMF Identification Data (ID): The actual identification data e.g. a hex-coded
                                     ///< UID according to ISO 14443.
            std::optional<std::string> tariff_text; ///< Tariff text

            last_command = "Start transaction";
            last_command_duration = "";
            screen.Post(Event::Custom);
            types::powermeter::TransactionReq tr;
            tr.evse_id = config.evse_id;
            tr.transaction_id = config.transaction_id;
            tr.identification_status = types::powermeter::OCMFUserIdentificationStatus::ASSIGNED;
            tr.identification_type = types::powermeter::OCMFIdentificationType::ISO14443;
            tr.identification_data = config.identification_data;
            tr.tariff_text = config.tariff_text;

            auto now_b = std::chrono::steady_clock::now();
            tr_start = r_powermeter->call_start_transaction(tr);
            auto now_e = std::chrono::steady_clock::now();
            last_command_duration =
                fmt::format("{} ms", std::chrono::duration_cast<std::chrono::milliseconds>(now_e - now_b).count());
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::RedLight, Color::White));

    Component stop_transaction_button = Button(
        "Stop transaction",
        [&] {
            last_command = "Stop transaction";
            std::string transaction_id = config.transaction_id;
            auto now_b = std::chrono::steady_clock::now();
            tr_stop = r_powermeter->call_stop_transaction(transaction_id);
            auto now_e = std::chrono::steady_clock::now();
            last_command_duration =
                fmt::format("{} ms", std::chrono::duration_cast<std::chrono::milliseconds>(now_e - now_b).count());
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::RedLight, Color::White));

    Component stop_transaction0_button = Button(
        "Stop transaction0",
        [&] {
            last_command = "Stop transaction";
            std::string transaction_id = "";
            auto now_b = std::chrono::steady_clock::now();
            tr_stop = r_powermeter->call_stop_transaction(transaction_id);
            auto now_e = std::chrono::steady_clock::now();
            last_command_duration =
                fmt::format("{} ms", std::chrono::duration_cast<std::chrono::milliseconds>(now_e - now_b).count());
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::RedLight, Color::White));

    auto cmds_component = Container::Horizontal({
        start_transaction_button,
        stop_transaction_button,
        stop_transaction0_button,
    });

    auto cmds_renderer = Renderer(cmds_component, [&] {
        auto cmds_win = window(text("Commands"), cmds_component->Render());
        return vbox({
                   hbox({cmds_win}) | center,
                   separator(),
               }) |
               flex_grow;
    });

    // ---------------------------------------------------------------------------
    // Vars
    // ---------------------------------------------------------------------------
    auto vars_renderer = Renderer([&] {
        std::vector<std::vector<std::string>> table_content;
        auto last_event_duration = fmt::format(
            "{} ms",
            std::chrono::duration_cast<std::chrono::milliseconds>(event_received - last_event_received).count());
        types::units::Energy ed = {};
        types::units::Power pd = {};
        types::units::Voltage vd = {};
        types::units_signed::SignedMeterValue svd = {};
        types::units::Current cd = {};
        types::units::Frequency fd = {};

        optional_add(table_content, "Transaction start response: status",
                     types::powermeter::transaction_request_status_to_string(tr_start.status));
        optional_add(table_content, "Transaction start response: error", tr_start.error);
        optional_add(table_content, "Transaction start response: transaction_min_stop_time",
                     tr_start.transaction_min_stop_time);
        optional_add(table_content, "Transaction start response: transaction_max_stop_time",
                     tr_start.transaction_max_stop_time);
        optional_add(table_content, "Transaction stop response: status",
                     types::powermeter::transaction_request_status_to_string(tr_stop.status));
        optional_add(table_content, "Transaction stop response: error", tr_stop.error);
        optional_add(table_content, "Transaction stop response: signed_meter_value",
                     tr_stop.signed_meter_value.value_or(svd).signed_meter_data);
        optional_add(table_content, "Powermeter: time stamp", powermeter.timestamp);
        if (powermeter.temperatures.has_value()) {
            int i = 1;
            for (const auto& sensor : *powermeter.temperatures) {
                std::string label = "Temperature sensor" + std::to_string(i++);
                optional_add(table_content, label, sensor.temperature);
            }
        }

        optional_add(table_content, "Powermeter: imported energy in Wh (from grid), total",
                     std::to_string(powermeter.energy_Wh_import.total));
        optional_add(table_content, "Powermeter: imported energy in Wh (from grid): L1",
                     powermeter.energy_Wh_import.L1);
        optional_add(table_content, "Powermeter: imported energy in Wh (from grid): L2",
                     powermeter.energy_Wh_import.L2);
        optional_add(table_content, "Powermeter: imported energy in Wh (from grid): L3",
                     powermeter.energy_Wh_import.L3);

        optional_add(table_content, "Powermeter: exported energy in Wh (to grid), total",
                     std::to_string(powermeter.energy_Wh_export.value_or(ed).total));
        optional_add(table_content, "Powermeter: exported energy in Wh (to grid): L1",
                     powermeter.energy_Wh_export.value_or(ed).L1);
        optional_add(table_content, "Powermeter: exported energy in Wh (to grid): L2",
                     powermeter.energy_Wh_export.value_or(ed).L2);
        optional_add(table_content, "Powermeter: exported energy in Wh (to grid): L3",
                     powermeter.energy_Wh_export.value_or(ed).L3);

        optional_add(table_content, "Powermeter: user defined meter ID", powermeter.meter_id);
        optional_add(table_content, "Powermeter: 3 phase rotation error (ccw)", powermeter.phase_seq_error);

        optional_add(table_content, "Powermeter: voltage in V, DC", powermeter.voltage_V.value_or(vd).DC);
        optional_add(table_content, "Powermeter: voltage in V: L1", powermeter.voltage_V.value_or(vd).L1);
        optional_add(table_content, "Powermeter: voltage in V: L2", powermeter.voltage_V.value_or(vd).L2);
        optional_add(table_content, "Powermeter: voltage in V: L3", powermeter.voltage_V.value_or(vd).L3);
        optional_add(table_content, "Powermeter: current in A, DC", powermeter.current_A.value_or(cd).DC);
        optional_add(table_content, "Powermeter: current in A: L1", powermeter.current_A.value_or(cd).L1);
        optional_add(table_content, "Powermeter: current in A: L2", powermeter.current_A.value_or(cd).L2);
        optional_add(table_content, "Powermeter: current in A: L3", powermeter.current_A.value_or(cd).L3);
        optional_add(table_content, "Powermeter: frequency in Hz: L1", powermeter.frequency_Hz.value_or(fd).L1);
        optional_add(table_content, "Powermeter: frequency in Hz: L2", powermeter.frequency_Hz.value_or(fd).L2);
        optional_add(table_content, "Powermeter: frequency in Hz: L3", powermeter.frequency_Hz.value_or(fd).L3);
        optional_add(table_content, "Public key", public_key);

        size_t max_width = 120;
        auto formatted_content = FormatTableContent(table_content, max_width);
        auto vars = Table(std::move(formatted_content));

        vars.SelectColumn(0).Border(LIGHT);
        for (int i = 0; i < table_content.size(); i++) {
            vars.SelectRow(i).Border(LIGHT);
        }

        return vbox({
                   hbox({
                       window(text("Information"),
                              vbox({text("Last command: " + last_command),
                                    text("Last command duration: " + last_command_duration),
                                    text("Time between events: " + last_event_duration), vars.Render()})),
                   }) | center,
               }) |
               flex_grow;
    });

    auto main_container = Container::Vertical({
        cmds_renderer,
        vars_renderer,
    });

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            text("Powermeter Component") | bold | hcenter,
            main_container->Render(),
        });
    });

    screen.Loop(main_renderer);
}

} // namespace module
