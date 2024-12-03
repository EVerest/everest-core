// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "EvAPI.hpp"
#include <utils/date.hpp>

namespace module {

static const auto NOTIFICATION_PERIOD = std::chrono::seconds(1);

void EvSessionInfo::reset() {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->event = std::nullopt;
}

void EvSessionInfo::update_event(const types::board_support_common::Event& event) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->event = event;
}

EvSessionInfo::operator std::string() {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);

    const auto now = date::utc_clock::now();

    std::string state = "Unknown";
    if (this->event.has_value()) {
        state = types::board_support_common::event_to_string(this->event.value());
    }

    json session_info = json::object({
        {"state", state},
        {"datetime", Everest::Date::to_rfc3339(now)},
    });

    return session_info.dump();
}

void EvAPI::init() {
    invoke_init(*p_main);

    std::vector<std::string> ev_connectors;
    std::string var_ev_connectors = this->api_base + "ev_connectors";

    for (auto& ev : this->r_ev_manager) {
        auto& session_info = this->info.emplace_back(std::make_unique<EvSessionInfo>());
        const std::string ev_base = this->api_base + ev->module_id;
        ev_connectors.push_back(ev->module_id);

        // API variables
        const std::string var_base = ev_base + "/var/";

        const std::string var_ev_info = var_base + "ev_info";
        ev->subscribe_ev_info([this, &ev, var_ev_info](types::evse_manager::EVInfo ev_info) {
            json ev_info_json = ev_info;
            this->mqtt.publish(var_ev_info, ev_info_json.dump());
        });

        const std::string var_session_info = var_base + "session_info";
        ev->subscribe_bsp_event([this, var_session_info, &session_info](const auto& bsp_event) {
            session_info->update_event(bsp_event.event);
            this->mqtt.publish(var_session_info, *session_info);
        });

        const std::string var_datetime = var_base + "datetime";
        this->api_threads.push_back(std::thread([this, var_datetime, var_session_info, &session_info]() {
            auto next_tick = std::chrono::steady_clock::now();
            while (this->running) {
                std::string datetime_str = Everest::Date::to_rfc3339(date::utc_clock::now());
                this->mqtt.publish(var_datetime, datetime_str);
                this->mqtt.publish(var_session_info, *session_info);

                next_tick += NOTIFICATION_PERIOD;
                std::this_thread::sleep_until(next_tick);
            }
        }));

        // API commands
        const std::string cmd_base = ev_base + "/cmd/";
    }

    this->api_threads.push_back(std::thread([this, var_ev_connectors, ev_connectors]() {
        auto next_tick = std::chrono::steady_clock::now();
        while (this->running) {
            const json ev_connectors_array = ev_connectors;
            this->mqtt.publish(var_ev_connectors, ev_connectors_array.dump());

            next_tick += NOTIFICATION_PERIOD;
            std::this_thread::sleep_until(next_tick);
        }
    }));
}

void EvAPI::ready() {
    invoke_ready(*p_main);

    const std::string var_active_errors = this->api_base + "errors/var/active_errors";
    this->api_threads.push_back(std::thread([this, var_active_errors]() {
        auto next_tick = std::chrono::steady_clock::now();
        while (this->running) {
            if (not r_error_history.empty()) {
                // request active errors
                types::error_history::FilterArguments filter;
                filter.state_filter = types::error_history::State::Active;
                const auto active_errors = r_error_history.at(0)->call_get_errors(filter);
                json errors_json = json(active_errors);

                // publish
                this->mqtt.publish(var_active_errors, errors_json.dump());
            }
            next_tick += NOTIFICATION_PERIOD;
            std::this_thread::sleep_until(next_tick);
        }
    }));
}

} // namespace module
