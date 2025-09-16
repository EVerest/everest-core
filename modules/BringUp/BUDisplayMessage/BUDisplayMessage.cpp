// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "BUDisplayMessage.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/table.hpp"
#include <generated/types/text_message.hpp>

using namespace ftxui;
namespace {

constexpr std::array<types::display_message::MessagePriorityEnum, 3> MessagePriorityEnumValues = {
    types::display_message::MessagePriorityEnum::AlwaysFront, types::display_message::MessagePriorityEnum::InFront,
    types::display_message::MessagePriorityEnum::NormalCycle};

constexpr std::array<types::display_message::MessageStateEnum, 4> MessageStateEnumValues = {
    types::display_message::MessageStateEnum::Charging, types::display_message::MessageStateEnum::Faulted,
    types::display_message::MessageStateEnum::Idle, types::display_message::MessageStateEnum::Unavailable};

constexpr std::array<types::text_message::MessageFormat, 4> MessageFormatValues = {
    types::text_message::MessageFormat::ASCII, types::text_message::MessageFormat::HTML,
    types::text_message::MessageFormat::URI, types::text_message::MessageFormat::UTF8};

constexpr std::array<types::display_message::IdentifierType, 3> Identifier_typeValues = {
    types::display_message::IdentifierType::IdToken, types::display_message::IdentifierType::SessionId,
    types::display_message::IdentifierType::TransactionId};

// Enum-String to be used for Dropdown GUI elements
std::vector<std::string> MessagePriorityEnumStrings;
std::vector<std::string> MessageStateEnumStrings;
std::vector<std::string> MessageFormatStrings;
std::vector<std::string> IdentifierTypeStrings;

struct MessageContentParameters {
    std::string content;
    std::string language;

    std::string msg_id;
    std::string timestamp_from;
    std::string timestamp_to;
    std::string identifier_id;
    std::string qr_code;

    int priority;
    int state;
    int format;
    int id_type;
};

types::display_message::DisplayMessage inputs2dspmsg_types(const MessageContentParameters& params) {
    int set_msg_id = 0;
    if (not params.msg_id.empty()) {
        set_msg_id = std::stoi(params.msg_id);
    }

    types::text_message::MessageContent msg_content{
        params.content,
        params.format == MessageFormatValues.size()
            ? std::nullopt
            : std::optional<types::text_message::MessageFormat>(MessageFormatValues[params.format]),
        params.language.empty() ? std::nullopt : std::optional<std::string>{params.language}};
    types::display_message::DisplayMessage msg{
        msg_content,
        params.msg_id.empty() ? std::nullopt : std::optional<int>(set_msg_id),
        params.priority == MessagePriorityEnumValues.size()
            ? std::nullopt
            : std::optional<types::display_message::MessagePriorityEnum>(MessagePriorityEnumValues[params.priority]),
        params.state == MessageStateEnumValues.size()
            ? std::nullopt
            : std::optional<types::display_message::MessageStateEnum>(MessageStateEnumValues[params.state]),
        params.timestamp_from.empty() ? std::nullopt : std::optional<std::string>(params.timestamp_from),
        params.timestamp_to.empty() ? std::nullopt : std::optional<std::string>(params.timestamp_to),
        params.identifier_id.empty() ? std::nullopt : std::optional<std::string>(params.identifier_id),
        params.id_type == Identifier_typeValues.size()
            ? std::nullopt
            : std::optional<types::display_message::IdentifierType>(Identifier_typeValues[params.id_type]),
        params.qr_code.empty() ? std::nullopt : std::optional<std::string>(params.qr_code)};

    return msg;
}

auto input_parameters_to_display_messages(
    const std::vector<std::unique_ptr<MessageContentParameters>>& set_msg_parameter_vector) {
    std::vector<types::display_message::DisplayMessage> display_messages;
    for (auto& msg_params : set_msg_parameter_vector) {
        if (msg_params->content.empty()) {
            continue;
        }
        display_messages.push_back(inputs2dspmsg_types(*msg_params));
    }
    return display_messages;
}

auto comma_separated_ids_to_vector(const std::string& get_msg_id_str) {
    std::optional<std::vector<int32_t>> get_msg_ids = std::vector<int32_t>{};

    int get_msg_id;
    std::stringstream msg_ids_ss(get_msg_id_str);

    while (msg_ids_ss.good()) {
        std::string singleNumber_str;
        getline(msg_ids_ss, singleNumber_str, ',');
        try {
            int singleNumber = std::stoi(singleNumber_str);
            get_msg_ids->push_back(singleNumber);
        } catch (...) {
            // just ignore it
        }
    }
    if (get_msg_ids->size() == 0) {
        get_msg_ids = std::nullopt;
    }
    return get_msg_ids;
}

auto msg_priority_int_to_optional_enum(int get_msg_priority_selected) {
    return (get_msg_priority_selected < MessagePriorityEnumValues.size())
               ? std::optional<
                     types::display_message::MessagePriorityEnum>{MessagePriorityEnumValues[get_msg_priority_selected]}
               : std::nullopt;
}

auto msg_state_int_to_optional_enum(int get_msg_state_selected) {
    return (get_msg_state_selected < MessageStateEnumValues.size())
               ? std::optional<types::display_message::MessageStateEnum>{MessageStateEnumValues[get_msg_state_selected]}
               : std::nullopt;
}

void log_cmd_response(std::vector<std::string>* log, const std::string& log_prefix, const std::string& status,
                      const std::optional<std::string>& status_info) {
    log->insert(log->begin(), log_prefix + ": " + status + ": '" + status_info.value_or("") + "'");
}

void log_message(std::vector<std::string>* log, types::display_message::DisplayMessage msg) {
    std::string message_str = msg.message.content;
    std::string format_str = (msg.message.format ? message_format_to_string(msg.message.format.value()) : "<no fmt>");
    std::string lang_str = msg.message.language.value_or("<no lang>");
    std::string id_str = (msg.id ? std::to_string(msg.id.value()) : "<no id>");
    std::string prio_str = (msg.priority ? message_priority_enum_to_string(msg.priority.value()) : "<no prio>");
    std::string state_str = (msg.state ? message_state_enum_to_string(msg.state.value()) : "<no stat>");
    std::string tstmp_fr_str = msg.timestamp_from.value_or("<no t_from>");
    std::string tstmp_to_str = msg.timestamp_to.value_or("<no t_to>");
    std::string id_id_str = msg.identifier_id.value_or("<no id_id>");
    std::string id_type_str =
        (msg.identifier_type ? identifier_type_to_string(msg.identifier_type.value()) : "<no id_typ>");
    std::string qr_str = msg.qr_code.value_or("<no qr>");

    std::vector<std::string> strings_to_join = {message_str,  format_str,   lang_str,  id_str,      prio_str, state_str,
                                                tstmp_fr_str, tstmp_to_str, id_id_str, id_type_str, qr_str};
    std::string joined_string = std::accumulate(
        std::begin(strings_to_join), std::end(strings_to_join), std::string{},
        [](std::string& first, std::string& second) { return first.empty() ? second : first + ", " + second; });

    log->insert(log->begin(), joined_string);
}

class MessageInputMask : public ComponentBase {
public:
    MessageInputMask(const std::string& heading, MessageContentParameters* parameters) :
        m_heading(heading), m_parameters(parameters) {

        m_msg_prio_dd = Dropdown(&MessagePriorityEnumStrings, &(m_parameters->priority));
        m_msg_stat_dd = Dropdown(&MessageStateEnumStrings, &(m_parameters->state));
        m_msg_form_dd = Dropdown(&MessageFormatStrings, &(m_parameters->format));
        m_id_type_dd = Dropdown(&IdentifierTypeStrings, &(m_parameters->id_type));

        InputOption id_input_o;
        id_input_o.multiline = false;
        id_input_o.cursor_position = 0;
        id_input_o.on_change = [&]() {
            if (m_parameters->identifier_id.size() > 36) {
                m_parameters->identifier_id.resize(36);
            }
        };

        InputOption msg_id_input_o;
        msg_id_input_o.multiline = false;
        msg_id_input_o.cursor_position = 0;
        msg_id_input_o.on_change = [&]() {
            m_parameters->msg_id.erase(std::remove_if(m_parameters->msg_id.begin(), m_parameters->msg_id.end(),
                                                      [](char c) { return !std::isdigit(c); }),
                                       m_parameters->msg_id.end());
        };

        InputOption input_o;
        input_o.multiline = false;

        m_msg_input = Input(&(m_parameters->content), "<required>", input_o);
        m_msg_id_input = Input(&(m_parameters->msg_id), "<none>", msg_id_input_o);
        m_qr_input = Input(&(m_parameters->qr_code), "<none>", input_o);
        m_lang_input = Input(&(m_parameters->language), "<none>", input_o);
        m_t_from_input = Input(&(m_parameters->timestamp_from), "<none>", input_o);
        m_t_to_input = Input(&(m_parameters->timestamp_to), "<none>", input_o);
        m_id_input = Input(&(m_parameters->identifier_id), "<none>", id_input_o);

        m_msg_details_component = Container::Vertical({
            m_msg_prio_dd,
            m_msg_stat_dd,
            m_id_type_dd,
            m_msg_form_dd,
            m_msg_id_input,
            m_lang_input,
            m_id_input,
            m_t_from_input,
            m_t_to_input,
            m_qr_input,
        });

        auto set_parameters_renderer = Renderer(m_msg_details_component, [&] {
            return vbox({
                hbox(hbox(text(" [Priority] : ") | vcenter, m_msg_prio_dd->Render()), filler(),
                     hbox(text(" [State] : ") | vcenter, m_msg_stat_dd->Render()), filler()),
                hbox(hbox(text(" [Identifier Type] : ") | vcenter, m_id_type_dd->Render()), filler(),
                     hbox(text(" [Format] : ") | vcenter, m_msg_form_dd->Render()), filler()),
                hbox(text(" [Message Id]    : "), m_msg_id_input->Render()),
                hbox(text(" [Language]      : "), m_lang_input->Render()),
                hbox(text(" [Identifier]    : "), m_id_input->Render()),
                hbox(text(" [Date-Time From]: "), m_t_from_input->Render()),
                hbox(text(" [Date-Time To]  : "), m_t_to_input->Render()),
                hbox(text(" [QR-Code]       : "), m_qr_input->Render()),
            });
        });

        m_details_collapsible = Collapsible("Message Details", set_parameters_renderer);

        auto set_msg_component = Container::Vertical({
            m_msg_input,
            m_details_collapsible,
        });

        // Add it to the container so it can receive focus.
        Add(set_msg_component);
    }

    // Override the Render method to display label and input together.
    Element Render() override {
        return vbox({
            text(m_heading) | inverted,
            hbox(text(" Message         : ") | bold, m_msg_input->Render()),
            m_details_collapsible->Render(),
            text(""),
        });
    }

private:
    std::string m_heading;
    MessageContentParameters* m_parameters;

    Component m_msg_prio_dd;
    Component m_msg_form_dd;
    Component m_id_type_dd;
    Component m_msg_stat_dd;

    Component m_msg_input;
    Component m_msg_id_input;
    Component m_qr_input;
    Component m_lang_input;
    Component m_t_from_input;
    Component m_t_to_input;
    Component m_id_input;

    Component m_msg_details_component;

    Component m_details_collapsible;
};

Component MessageInputMaskComponent(const std::string& heading, MessageContentParameters* parameters) {
    return Make<MessageInputMask>(heading, parameters);
}

} // namespace

namespace module {

void BUDisplayMessage::init() {
    invoke_init(*p_main);
}

void BUDisplayMessage::ready() {
    invoke_ready(*p_main);

    // Prepare Enum-String to be used for Dropdown GUI elements
    for (auto msgPriority : MessagePriorityEnumValues) {
        MessagePriorityEnumStrings.push_back(message_priority_enum_to_string(msgPriority));
    }
    MessagePriorityEnumStrings.push_back("<none>");

    for (auto msgState : MessageStateEnumValues) {
        MessageStateEnumStrings.push_back(message_state_enum_to_string(msgState));
    }
    MessageStateEnumStrings.push_back("<none>");

    for (auto msgFormat : MessageFormatValues) {
        MessageFormatStrings.push_back(message_format_to_string(msgFormat));
    }
    MessageFormatStrings.push_back("<none>");

    for (auto idType : Identifier_typeValues) {
        IdentifierTypeStrings.push_back(identifier_type_to_string(idType));
    }
    IdentifierTypeStrings.push_back("<none>");

    /***************************************************************************************+
     * Start of FTXUI-GUI
     ****************************************************************************************/

    auto screen = ScreenInteractive::Fullscreen();

    int log_pane_index = 0;
    std::vector<std::string> log;
    auto log_pane_raw_component = Menu(&log, &log_pane_index);
    auto log_pane_component = log_pane_raw_component | vscroll_indicator;

    auto log_pane_renderer =
        Renderer(log_pane_component, [&] { return window(text(" Command Log "), log_pane_component->Render()); });

    /***************************************************************************************+
     * Set Display Messages
     ****************************************************************************************/

    /* ********************** INPUTS ******************************************** */
    std::vector<std::unique_ptr<MessageContentParameters>> set_msg_parameter_vector{};

    int message_input_mask_count;

    std::vector<Component> set_msg_input_masks{};

    auto msg_inputs_component = Container::Vertical({});

    auto add_msg_input_widget = [&] {
        set_msg_parameter_vector.push_back(std::make_unique<MessageContentParameters>());

        // Initialize dropdown choices to be <none> for all dropdowns
        set_msg_parameter_vector.back()->priority = MessagePriorityEnumStrings.size() - 1;
        set_msg_parameter_vector.back()->state = MessageStateEnumStrings.size() - 1;
        set_msg_parameter_vector.back()->format = MessageFormatStrings.size() - 1;
        set_msg_parameter_vector.back()->id_type = IdentifierTypeStrings.size() - 1;

        message_input_mask_count += 1;
        set_msg_input_masks.push_back(MessageInputMaskComponent("Message " + std::to_string(message_input_mask_count),
                                                                set_msg_parameter_vector.back().get()));
        msg_inputs_component->Add(set_msg_input_masks.back());

        // Tell FTXUI to redraw
        screen.PostEvent(Event::Custom);
    };

    auto reset_msg_input_widgets = [&] {
        msg_inputs_component->DetachAllChildren();
        set_msg_input_masks.clear();
        set_msg_parameter_vector.clear();
        message_input_mask_count = 0;
        add_msg_input_widget();
    };

    add_msg_input_widget();

    /* ********************** BUTTONS ******************************************* */
    auto add_msg_button = Button(
        "Add Message!", [&] { add_msg_input_widget(); }, ButtonOption::Border());

    auto set_button = Button(
        "Set!",
        [&] {
            auto display_messages = input_parameters_to_display_messages(set_msg_parameter_vector);

            if (display_messages.empty()) {
                return;
            }

            auto [status, status_info] = r_dm->call_set_display_message(display_messages);

            std::string log_prefix{"Set " + std::to_string(display_messages.size()) + " message(s)"};
            log_cmd_response(&log, log_prefix, display_message_status_enum_to_string(status), status_info);

            reset_msg_input_widgets();
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    /* ********************** COMPOSITION ******************************************** */

    auto set_parameters_renderer = Renderer(msg_inputs_component, [&] {
        Elements elements;

        for (auto& input_mask : set_msg_input_masks) {
            elements.push_back(input_mask->Render());
        }

        return vbox(std::move(elements));
    });
    auto set_component = Container::Vertical({set_parameters_renderer, add_msg_button, set_button});

    /***************************************************************************************+
     * Get Display Messages
     ****************************************************************************************/

    /* ********************** INPUTS ******************************************** */
    std::string get_msg_id_str;

    InputOption get_msg_id_input_o;
    get_msg_id_input_o.multiline = false;
    get_msg_id_input_o.cursor_position = 0;
    get_msg_id_input_o.on_change = [&]() {
        get_msg_id_str.erase(std::remove_if(get_msg_id_str.begin(), get_msg_id_str.end(),
                                            [](char c) { return !std::isdigit(c) and c != ','; }),
                             get_msg_id_str.end());
    };
    auto get_msg_id_input = Input(&get_msg_id_str, "<comma-separated list of IDs or none>", get_msg_id_input_o);

    /* ********************** DROPDOWNS ******************************************** */
    int get_msg_priority_selected = MessagePriorityEnumStrings.size() - 1;
    int get_msg_state_selected = MessageStateEnumStrings.size() - 1;

    auto get_msg_priority_dropdown = Dropdown(&MessagePriorityEnumStrings, &get_msg_priority_selected);
    auto get_msg_state_dropdown = Dropdown(&MessageStateEnumStrings, &get_msg_state_selected);

    /* ********************** BUTTON ******************************************** */
    auto get_button = Button(
        "Get!",
        [&] {
            auto get_msg_ids = comma_separated_ids_to_vector(get_msg_id_str);
            auto msgPriorityChoice = msg_priority_int_to_optional_enum(get_msg_priority_selected);
            auto msgStateChoice = msg_state_int_to_optional_enum(get_msg_state_selected);

            auto [status_info, messages] =
                r_dm->call_get_display_messages({get_msg_ids, msgPriorityChoice, msgStateChoice});

            std::string log_prefix{"Got " + (messages ? std::to_string(messages->size()) : "no") + " message(s)"};
            log_cmd_response(&log, log_prefix, "", status_info);
            for (auto msg : messages.value_or(std::vector<types::display_message::DisplayMessage>())) {
                log_message(&log, msg);
            }
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    /* ********************** COMPOSITION ******************************************** */

    auto get_dropdowns_component = Container::Horizontal({get_msg_priority_dropdown, get_msg_state_dropdown});

    auto get_parameters_component = Container::Vertical({
        get_dropdowns_component,
        get_msg_id_input,
    });

    auto get_parameters_renderer = Renderer(get_parameters_component, [&] {
        return vbox({
            text(" Command Parameters ") | inverted,
            hbox(hbox(text(" [Priority] : ") | vcenter, get_msg_priority_dropdown->Render()), filler(),
                 hbox(text(" [State] : ") | vcenter, get_msg_state_dropdown->Render()), filler()),
            hbox(text(" [Message Id] : "), get_msg_id_input->Render()),
            text("  "),
        });
    });
    auto get_component = Container::Vertical({get_parameters_renderer, get_button});

    /***************************************************************************************+
     * Clear Display Messages
     ****************************************************************************************/

    /* ********************** INPUTS ******************************************** */
    std::string clear_msg_id_str;
    InputOption o;
    o.multiline = false;
    o.cursor_position = 0;
    o.on_change = [&]() {
        clear_msg_id_str.erase(
            std::remove_if(clear_msg_id_str.begin(), clear_msg_id_str.end(), [](char c) { return !std::isdigit(c); }),
            clear_msg_id_str.end());
    };
    auto clear_id_input = Input(&clear_msg_id_str, "<single id (int) required>", o);

    /* ********************** BUTTON ******************************************** */
    auto clear_button = Button(
        "Clear!",
        [&] {
            int clear_msg_id;
            try {
                clear_msg_id = std::stoi(clear_msg_id_str);
                clear_msg_id_str = std::to_string(clear_msg_id);
            } catch (...) {
                return;
            }

            auto [status, status_info] = r_dm->call_clear_display_message({clear_msg_id});

            std::string log_prefix{"Cleared Messages with ID " + std::to_string(clear_msg_id)};
            log_cmd_response(&log, log_prefix, clear_message_response_enum_to_string(status), status_info);
        },
        ButtonOption::Animated(Color::Blue, Color::White, Color::BlueLight, Color::White));

    /* ********************** COMPOSITION ******************************************** */
    auto clear_parameters_renderer = Renderer(clear_id_input, [&] {
        return vbox({
            text(" Command Parameters ") | inverted,
            hbox(text(" Message Id : ") | bold, clear_id_input->Render()),
            text("  "),
        });
    });

    auto clear_component = Container::Vertical({clear_parameters_renderer, clear_button});

    /***************************************************************************************+
     * Composition of full display
     ****************************************************************************************/

    std::vector<std::string> tab_values{
        "Set Message",
        "Get Message",
        "Clear Message",
    };

    int tab_selected = 0;
    auto tab_toggle_component = Toggle(&tab_values, &tab_selected);

    auto tab_toggle_renderer =
        Renderer(tab_toggle_component, [&] { return window(text(" Commands "), tab_toggle_component->Render()); });

    auto tab_container_component = Container::Tab(
        {
            set_component,
            get_component,
            clear_component,
        },
        &tab_selected);

    auto main_container = Container::Vertical({
        tab_toggle_renderer,
        tab_container_component,
        log_pane_renderer,
    });

    auto main_renderer = Renderer(main_container, [&] {
        return vbox({
            text("Display Message") | bold | hcenter,
            main_container->Render(),
        });
    });

    screen.Loop(main_renderer);
}

} // namespace module
