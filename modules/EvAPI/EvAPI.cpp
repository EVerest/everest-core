// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "EvAPI.hpp"
#include <utils/date.hpp>
#include <utils/yaml_loader.hpp>

namespace module {

static const auto NOTIFICATION_PERIOD = std::chrono::seconds(1);

SessionInfo::SessionInfo() :
    start_energy_import_wh(0),
    end_energy_import_wh(0),
    start_energy_export_wh(0),
    end_energy_export_wh(0),
    latest_total_w(0),
    state("Unknown") {
    this->start_time_point = date::utc_clock::now();
    this->end_time_point = this->start_time_point;
}

void SessionInfo::reset() {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->state = "Unknown";
    this->start_energy_import_wh = 0;
    this->end_energy_import_wh = 0;
    this->start_energy_export_wh = 0;
    this->end_energy_export_wh = 0;
    this->start_time_point = date::utc_clock::now();
    this->latest_total_w = 0;
    this->permanent_fault = false;
}

static void remove_error_from_list(std::vector<module::SessionInfo::Error>& list, const std::string& error_type) {
    list.erase(std::remove_if(list.begin(), list.end(),
                              [error_type](const module::SessionInfo::Error& err) { return err.type == error_type; }),
               list.end());
}

void SessionInfo::update_state(const std::string& event) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->state = event;
}

void SessionInfo::set_start_energy_import_wh(int32_t start_energy_import_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->start_energy_import_wh = start_energy_import_wh;
    this->end_energy_import_wh = start_energy_import_wh;
    this->start_time_point = date::utc_clock::now();
    this->end_time_point = this->start_time_point;
}

void SessionInfo::set_end_energy_import_wh(int32_t end_energy_import_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->end_energy_import_wh = end_energy_import_wh;
    this->end_time_point = date::utc_clock::now();
}

void SessionInfo::set_latest_energy_import_wh(int32_t latest_energy_wh) {
}

void SessionInfo::set_start_energy_export_wh(int32_t start_energy_export_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->start_energy_export_wh = start_energy_export_wh;
    this->end_energy_export_wh = start_energy_export_wh;
    this->start_energy_export_wh_was_set = true;
}

void SessionInfo::set_end_energy_export_wh(int32_t end_energy_export_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->end_energy_export_wh = end_energy_export_wh;
    this->end_energy_export_wh_was_set = true;
}

void SessionInfo::set_latest_energy_export_wh(int32_t latest_export_energy_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
}

void SessionInfo::set_latest_total_w(double latest_total_w) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->latest_total_w = latest_total_w;
}

void SessionInfo::set_enable_disable_source(const std::string& active_source, const std::string& active_state,
                                            const int active_priority) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->active_enable_disable_source = active_source;
    this->active_enable_disable_state = active_state;
    this->active_enable_disable_priority = active_priority;
}

static void to_json(json& j, const SessionInfo::Error& e) {
    j = json{{"type", e.type}, {"description", e.description}, {"severity", e.severity}};
}

SessionInfo::operator std::string() {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);

    auto charged_energy_wh = this->end_energy_import_wh - this->start_energy_import_wh;
    int32_t discharged_energy_wh{0};
    if ((this->start_energy_export_wh_was_set == true) && (this->end_energy_export_wh_was_set == true)) {
        discharged_energy_wh = this->end_energy_export_wh - this->start_energy_export_wh;
    }
    auto now = date::utc_clock::now();

    auto charging_duration_s =
        std::chrono::duration_cast<std::chrono::seconds>(this->end_time_point - this->start_time_point);

    json session_info = json::object({
        {"state", this->state},
        {"permanent_fault", this->permanent_fault},
        {"charged_energy_wh", charged_energy_wh},
        {"discharged_energy_wh", discharged_energy_wh},
        {"latest_total_w", this->latest_total_w},
        {"charging_duration_s", charging_duration_s.count()},
        {"datetime", Everest::Date::to_rfc3339(now)},

    });

    json active_disable_enable = json::object({{"source", this->active_enable_disable_source},
                                               {"state", this->active_enable_disable_state},
                                               {"priority", this->active_enable_disable_priority}});
    session_info["active_enable_disable_source"] = active_disable_enable;

    return session_info.dump();
}

void EvAPI::init() {
    invoke_init(*p_main);

    std::vector<std::string> connectors;
    std::string var_connectors = this->api_base + "connectors";

    for (auto& ev : this->r_ev_manager) {
        auto& session_info = this->info.emplace_back(std::make_unique<SessionInfo>());
        auto& hw_caps = this->hw_capabilities_str.emplace_back("");
        std::string ev_base = this->api_base + ev->module_id;
        connectors.push_back(ev->module_id);

        // API variables
        std::string var_base = ev_base + "/var/";

        std::string var_ev_info = var_base + "ev_info";
        ev->subscribe_ev_info([this, &ev, var_ev_info](types::evse_manager::EVInfo ev_info) {
            json ev_info_json = ev_info;
            this->mqtt.publish(var_ev_info, ev_info_json.dump());
        });

        std::string var_session_info = var_base + "session_info";
        ev->subscribe_bsp_event([this, var_session_info, &session_info](const auto& bsp_event) {
            session_info->update_state(types::board_support_common::event_to_string(bsp_event.event));
            this->mqtt.publish(var_session_info, *session_info);
        });

        std::string var_datetime = var_base + "datetime";
        std::string var_logging_path = var_base + "logging_path";
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
        std::string cmd_base = ev_base + "/cmd/";
    }

    this->api_threads.push_back(std::thread([this, var_connectors, connectors]() {
        auto next_tick = std::chrono::steady_clock::now();
        while (this->running) {
            json connectors_array = connectors;
            this->mqtt.publish(var_connectors, connectors_array.dump());

            next_tick += NOTIFICATION_PERIOD;
            std::this_thread::sleep_until(next_tick);
        }
    }));
}

void EvAPI::ready() {
    invoke_ready(*p_main);

    std::string var_active_errors = this->api_base + "errors/var/active_errors";
    this->api_threads.push_back(std::thread([this, var_active_errors]() {
        auto next_tick = std::chrono::steady_clock::now();
        while (this->running) {
            std::string datetime_str = Everest::Date::to_rfc3339(date::utc_clock::now());

            if (not r_error_history.empty()) {
                // request active errors
                types::error_history::FilterArguments filter;
                filter.state_filter = types::error_history::State::Active;
                auto active_errors = r_error_history.at(0)->call_get_errors(filter);
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
