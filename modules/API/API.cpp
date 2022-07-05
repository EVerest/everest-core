// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "API.hpp"
#include <utils/date.hpp>

namespace module {

SessionInfo::SessionInfo() : state("Unknown"), start_energy_wh(0), end_energy_wh(0), latest_total_w(0) {
    this->start_time_point = date::utc_clock::now();
    this->end_time_point = this->start_time_point;
}

bool SessionInfo::is_state_charging(const std::string& current_state) {
    if (current_state == "PluggedIn" || current_state == "AuthRequired" || current_state == "Charging" ||
        current_state == "ChargingPausedEV" || current_state == "ChargingPausedEVSE") {
        return true;
    }
    return false;
}

void SessionInfo::reset() {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->state = "Unknown";
    this->start_energy_wh = 0;
    this->end_energy_wh = 0;
    this->start_time_point = date::utc_clock::now();
    this->latest_total_w = 0;
}

void SessionInfo::update_state(const std::string& event) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);

    if (event == "Enabled") {
        this->state = "Unplugged";
    } else if (event == "Disabled") {
        this->state = "Disabled";
    } else if (event == "SessionStarted") {
        this->state = "PluggedIn";
    } else if (event == "AuthRequired") {
        this->state = "AuthRequired";
    } else if (event == "ChargingStarted") {
        this->state = "Charging";
    } else if (event == "ChargingPausedEV") {
        this->state = "ChargingPausedEV";
    } else if (event == "ChargingPausedEVSE") {
        this->state = "ChargingPausedEVSE";
    } else if (event == "ChargingResumed") {
        this->state = "Charging";
    } else if (event == "SessionFinished") {
        this->state = "Unplugged";
    } else if (event == "Error") {
        this->state = "Error";
    } else if (event == "PermanentFault") {
        this->state = "PermanentFault";
    }
}

void SessionInfo::set_start_energy_wh(int32_t start_energy_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->start_energy_wh = start_energy_wh;
    this->end_energy_wh = start_energy_wh;
    this->start_time_point = date::utc_clock::now();
    this->end_time_point = this->start_time_point;
}

void SessionInfo::set_end_energy_wh(int32_t end_energy_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->end_energy_wh = end_energy_wh;
    this->end_time_point = date::utc_clock::now();
}

void SessionInfo::set_latest_energy_wh(int32_t latest_energy_wh) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    if (this->is_state_charging(this->state)) {
        this->end_time_point = date::utc_clock::now();
        this->end_energy_wh = latest_energy_wh;
    }
}

void SessionInfo::set_latest_total_w(double latest_total_w) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->latest_total_w = latest_total_w;
}

SessionInfo::operator std::string() {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);

    auto charged_energy_wh = this->end_energy_wh - this->start_energy_wh;
    auto now = date::utc_clock::now();

    auto charging_duration_s =
        std::chrono::duration_cast<std::chrono::seconds>(this->end_time_point - this->start_time_point);

    json session_info = json::object({{"state", this->state},
                                      {"charged_energy_wh", charged_energy_wh},
                                      {"latest_total_w", this->latest_total_w},
                                      {"charging_duration_s", charging_duration_s.count()},
                                      {"datetime", Everest::Date::to_rfc3339(now)}});

    return session_info.dump();
}

void API::init() {
    invoke_init(*p_main);

    std::string api_base = "everest_api/";

    int32_t count = 0;
    for (auto& evse : this->r_evse_manager) {
        this->info.push_back(std::make_unique<SessionInfo>());
        auto& session_info = this->info.back();
        std::string evse_base = api_base + evse->module_id;

        // API variables
        std::string var_base = evse_base + "/var/";

        std::string var_powermeter = var_base + "powermeter";
        evse->subscribe_powermeter([this, var_powermeter, &session_info](types::powermeter::Powermeter powermeter) {
            json powermeter_json = powermeter;
            this->mqtt.publish(var_powermeter, powermeter_json.dump());
            session_info->set_latest_energy_wh(powermeter_json.at("energy_Wh_import").at("total"));
            session_info->set_latest_total_w(powermeter_json.at("power_W").at("total"));
        });

        std::string var_limits = var_base + "limits";
        evse->subscribe_limits([this, var_limits](types::evse_manager::Limits limits) {
            json limits_json = limits;
            this->mqtt.publish(var_limits, limits_json.dump());
        });

        std::string var_telemetry = var_base + "telemetry";
        evse->subscribe_telemetry([this, var_telemetry](types::board_support::Telemetry telemetry) {
            json telemetry_json = telemetry;
            this->mqtt.publish(var_telemetry, telemetry_json.dump());
        });

        std::string var_datetime = var_base + "datetime";
        std::string var_session_info = var_base + "session_info";
        this->datetime_thread = std::thread([this, var_datetime, var_session_info, &session_info]() {
            auto next_tick = std::chrono::steady_clock::now();
            while (this->running) {
                std::string datetime_str = Everest::Date::to_rfc3339(date::utc_clock::now());
                this->mqtt.publish(var_datetime, datetime_str);
                this->mqtt.publish(var_session_info, *session_info);

                next_tick += std::chrono::seconds(1);
                std::this_thread::sleep_until(next_tick);
            }
        });

        evse->subscribe_session_events([this, var_session_info, &session_info](types::evse_manager::SessionEvents session_events) {
            auto event = types::evse_manager::session_event_to_string(session_events.event);
            session_info->update_state(event);
            if (event == "SessionStarted") {
                auto session_started = session_events.session_started.value();
                auto energy_Wh_import = session_started.energy_Wh_import;
                session_info->set_start_energy_wh(energy_Wh_import);
            } else if (event == "SessionFinished") {
                auto session_finished = session_events.session_finished.value();
                auto energy_Wh_import = session_finished.energy_Wh_import;
                session_info->set_end_energy_wh(energy_Wh_import);
            }

            this->mqtt.publish(var_session_info, *session_info);
        });

        // API commands
        std::string cmd_base = evse_base + "/cmd/";

        std::string cmd_pause_charging = cmd_base + "pause_charging";
        this->mqtt.subscribe(cmd_pause_charging, [&evse](const std::string& data) {
            evse->call_pause_charging(); //
        });

        std::string cmd_resume_charging = cmd_base + "resume_charging";
        this->mqtt.subscribe(cmd_resume_charging, [&evse](const std::string& data) {
            evse->call_resume_charging(); //
        });

        count += 1;
    }
}

void API::ready() {
    invoke_ready(*p_main);
}

} // namespace module
