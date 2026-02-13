// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 - 2026 Pionix GmbH and Contributors to EVerest
#include <ieee2030/charger/session/session.hpp>

#include <ieee2030/charger/v20/state/state_b.hpp>

namespace ieee2030::charger {

Session::Session(std::unique_ptr<io::CanBrokerCharger> can_broker_, const callback::Callbacks& callbacks) :
    can_broker(std::move(can_broker_)),
    session_is_active(true),
    ctx(active_event, callbacks, message_100, message_101, message_102),
    fsm(ctx.create_state<v20::state::StateB>()) {

    can_broker->set_event_callback([this](io::CanEvent event) { this->handle_can_event(event); });

    v20::CanBrokerContext can_broker_callbacks;
    can_broker_callbacks.enable_can = [this] { can_broker->enable_tx_can(); };
    can_broker_callbacks.disable_can = [this] { can_broker->disable_tx_can(); };
    can_broker_callbacks.update_status_error = [this](defs::ChargerStatusError status, bool active) {
        can_broker->update_status_error_flag(status, active);
    };
    can_broker_callbacks.update_reamining_time = [this](uint16_t seconds) {
        const auto time_10s = static_cast<uint16_t>(seconds / 10);
        can_broker->update_reamining_time_10s(time_10s);
    };

    ctx.set_can_broker_callback(can_broker_callbacks);
}

Session::~Session() = default;

void Session::update(const std::vector<events::Event>& current_events) {

    // Handle all events before state machine
    if (!current_events.empty()) {
        for (auto event : current_events) {
            if (std::holds_alternative<events::PresentVoltageCurrent>(event)) {
                const auto& present_values = std::get<events::PresentVoltageCurrent>(event);
                can_broker->update_present_voltage(present_values.voltage);
                can_broker->update_present_current(present_values.current);
            } else if (std::holds_alternative<events::AvailableVoltageCurrent>(event)) {
                const auto& available_values = std::get<events::AvailableVoltageCurrent>(event);
                can_broker->update_available_voltage(available_values.voltage);
                can_broker->update_available_current(available_values.current);
            } else {
                active_event = event;

                if (std::holds_alternative<events::CS1>(event) || std::holds_alternative<events::CS2>(event) ||
                    std::holds_alternative<events::ProximityDetection>(event) ||
                    std::holds_alternative<events::ChargePermission>(event)) {
                    // Todo(sl): check result!
                    [[maybe_unused]] const auto res = fsm.feed(v20::Event::HW_SIGNAL);
                } else {
                    // Todo(sl): check result!
                    [[maybe_unused]] const auto res = fsm.feed(v20::Event::EVENT);
                }
            }
        }
    }

    if (auto timeout_reached = ctx.timeout.timeout_reached()) {
        if (*timeout_reached) {
            // Todo(sl): check result!
            [[maybe_unused]] const auto res = fsm.feed(v20::Event::TIMEOUT);
        }
    }

    if (state.new_data) {
        message_100 = can_broker->get_can_100_message();
        message_101 = can_broker->get_can_101_message();
        message_102 = can_broker->get_can_102_message();
        // Todo(sl): Check return value
        [[maybe_unused]] const auto res = fsm.feed(v20::Event::CAN_MESSAGE);
    }
}

void Session::handle_can_event(io::CanEvent event) {
    using Event = io::CanEvent;

    switch (event) {
    case Event::ACTIVE:
        state.can_active = true;
        break;
    case Event::INACTIVE:
        state.can_active = false;
        break;
    case Event::NEW_DATA:
        state.new_data = true;
        break;
    }
};

} // namespace ieee2030::charger
