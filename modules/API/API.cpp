// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "API.hpp"
#include <utils/date.hpp>

namespace module {

static const auto NOTIFICATION_PERIOD = std::chrono::seconds(1);

SessionInfo::SessionInfo() :
    state("Unknown"),
    start_energy_import_wh(0),
    end_energy_import_wh(0),
    latest_total_w(0),
    start_energy_export_wh(0),
    end_energy_export_wh(0) {
    this->start_time_point = date::utc_clock::now();
    this->end_time_point = this->start_time_point;
}

bool SessionInfo::is_state_charging(const std::string& current_state) {
    if (current_state == "AuthRequired" || current_state == "Charging" || current_state == "ChargingPausedEV" ||
        current_state == "ChargingPausedEVSE") {
        return true;
    }
    return false;
}

void SessionInfo::reset() {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->state = "Unknown";
    this->state_info = "";
    this->start_energy_import_wh = 0;
    this->end_energy_import_wh = 0;
    this->start_energy_export_wh = 0;
    this->end_energy_export_wh = 0;
    this->start_time_point = date::utc_clock::now();
    this->latest_total_w = 0;
}

types::energy::ExternalLimits get_external_limits(const std::string& data, bool is_watts) {
    const auto limit = std::stof(data);
    const auto timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    types::energy::ExternalLimits external_limits;
    types::energy::ScheduleReqEntry target_entry;
    target_entry.timestamp = timestamp;

    types::energy::ScheduleReqEntry zero_entry;
    zero_entry.timestamp = timestamp;
    zero_entry.limits_to_leaves.total_power_W = 0;

    if (is_watts) {
        target_entry.limits_to_leaves.total_power_W = std::fabs(limit);
    } else {
        target_entry.limits_to_leaves.ac_max_current_A = std::fabs(limit);
    }

    if (limit > 0) {
        external_limits.schedule_import.emplace(std::vector<types::energy::ScheduleReqEntry>(1, target_entry));
    } else {
        external_limits.schedule_export.emplace(std::vector<types::energy::ScheduleReqEntry>(1, target_entry));
        external_limits.schedule_import.emplace(std::vector<types::energy::ScheduleReqEntry>(1, zero_entry));
    }
    return external_limits;
}

void SessionInfo::update_state(const std::string& event, const std::string& state_info) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);

    this->state_info = state_info;
    if (event == "Enabled") {
        this->state = "Unplugged";
    } else if (event == "Disabled") {
        this->state = "Disabled";
    } else if (event == "SessionStarted") {
        this->state = "Preparing";
    } else if (event == "ReservationStart") {
        this->state = "Reserved";
    } else if (event == "ReservationEnd") {
        this->state = "Unplugged";
    } else if (event == "AuthRequired") {
        this->state = "AuthRequired";
    } else if (event == "WaitingForEnergy") {
        this->state = "WaitingForEnergy";
    } else if (event == "TransactionStarted") {
        this->state = "Charging";
    } else if (event == "ChargingPausedEV") {
        this->state = "ChargingPausedEV";
    } else if (event == "ChargingPausedEVSE") {
        this->state = "ChargingPausedEVSE";
    } else if (event == "ChargingResumed") {
        this->state = "Charging";
    } else if (event == "TransactionFinished") {
        this->state = "Finished";
    } else if (event == "SessionFinished") {
        this->state = "Unplugged";
    } else if (event == "Error") {
        this->state = "Error";
    } else if (event == "PermanentFault") {
        this->state = "PermanentFault";
    }
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
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    if (this->is_state_charging(this->state)) {
        this->end_time_point = date::utc_clock::now();
        this->end_energy_import_wh = latest_energy_wh;
    }
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
    if (this->is_state_charging(this->state)) {
        this->end_energy_export_wh = latest_export_energy_wh;
        this->end_energy_export_wh_was_set = true;
    }
}

void SessionInfo::set_latest_total_w(double latest_total_w) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->latest_total_w = latest_total_w;
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

    json session_info = json::object({{"state", this->state},
                                      {"state_info", this->state_info},
                                      {"charged_energy_wh", charged_energy_wh},
                                      {"discharged_energy_wh", discharged_energy_wh},
                                      {"latest_total_w", this->latest_total_w},
                                      {"charging_duration_s", charging_duration_s.count()},
                                      {"datetime", Everest::Date::to_rfc3339(now)}});

    return session_info.dump();
}

void API::init() {
    invoke_init(*p_main);
    std::string api_base = "everest_api/";
    std::vector<std::string> connectors;
    std::string var_connectors = api_base + "connectors";

    for (auto& evse : this->r_evse_manager) {
        this->info.push_back(std::make_unique<SessionInfo>());
        auto& session_info = this->info.back();
        this->hw_capabilities_json.push_back(json{});
        auto& hw_caps = this->hw_capabilities_json.back();
        std::string evse_base = api_base + evse->module_id;
        connectors.push_back(evse->module_id);

        // API variables
        std::string var_base = evse_base + "/var/";

        std::string var_hw_caps = var_base + "hardware_capabilities";
        evse->subscribe_hw_capabilities(
            [this, var_hw_caps, &hw_caps](types::board_support::HardwareCapabilities hw_capabilities) {
                hw_caps = hw_capabilities;
                this->mqtt.publish(var_hw_caps, hw_caps.dump());
            });

        std::string var_powermeter = var_base + "powermeter";
        evse->subscribe_powermeter([this, var_powermeter, &session_info](types::powermeter::Powermeter powermeter) {
            json powermeter_json = powermeter;
            this->mqtt.publish(var_powermeter, powermeter_json.dump());
            session_info->set_latest_energy_import_wh(powermeter.energy_Wh_import.total);
            if (powermeter.energy_Wh_export.has_value()) {
                session_info->set_latest_energy_export_wh(powermeter.energy_Wh_export.value().total);
            }
            if (powermeter.power_W.has_value())
                session_info->set_latest_total_w(powermeter.power_W.value().total);
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

        std::string var_ev_info = var_base + "ev_info";
        evse->subscribe_ev_info([this, var_ev_info](types::evse_manager::EVInfo ev_info) {
            json ev_info_json = ev_info;
            this->mqtt.publish(var_ev_info, ev_info_json.dump());
        });

        std::string var_datetime = var_base + "datetime";
        std::string var_session_info = var_base + "session_info";
        this->api_threads.push_back(
            std::thread([this, var_datetime, var_session_info, var_hw_caps, &session_info, &hw_caps]() {
                auto next_tick = std::chrono::steady_clock::now();
                while (this->running) {
                    std::string datetime_str = Everest::Date::to_rfc3339(date::utc_clock::now());
                    this->mqtt.publish(var_datetime, datetime_str);
                    this->mqtt.publish(var_session_info, *session_info);
                    this->mqtt.publish(var_hw_caps, hw_caps.dump());

                    next_tick += NOTIFICATION_PERIOD;
                    std::this_thread::sleep_until(next_tick);
                }
            }));

        evse->subscribe_session_event([this, var_session_info,
                                       &session_info](types::evse_manager::SessionEvent session_event) {
            auto event = types::evse_manager::session_event_enum_to_string(session_event.event);
            if (session_event.error) {
                session_info->update_state(event, types::evse_manager::error_enum_to_string(session_event.error.value().error_code));
            } else {
                session_info->update_state(event, "");
            }
            if (event == "TransactionStarted") {
                auto transaction_started = session_event.transaction_started.value();
                auto energy_Wh_import = transaction_started.meter_value.energy_Wh_import.total;
                session_info->set_start_energy_import_wh(energy_Wh_import);

                if (transaction_started.meter_value.energy_Wh_export.has_value()) {
                    auto energy_Wh_export = transaction_started.meter_value.energy_Wh_export.value().total;
                    session_info->set_start_energy_export_wh(energy_Wh_export);
                } else {
                    session_info->start_energy_export_wh_was_set = false;
                }
            } else if (event == "TransactionFinished") {
                auto transaction_finished = session_event.transaction_finished.value();
                auto energy_Wh_import = transaction_finished.meter_value.energy_Wh_import.total;
                session_info->set_end_energy_import_wh(energy_Wh_import);

                if (transaction_finished.meter_value.energy_Wh_export.has_value()) {
                    auto energy_Wh_export = transaction_finished.meter_value.energy_Wh_export.value().total;
                    session_info->set_end_energy_export_wh(energy_Wh_export);
                } else {
                    session_info->end_energy_export_wh_was_set = false;
                }

                this->mqtt.publish(var_session_info, *session_info);
            }
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

        std::string cmd_set_limit = cmd_base + "set_limit_amps";
        this->mqtt.subscribe(cmd_set_limit, [&evse](const std::string& data) {
            // reset previous limits set by this module
            types::energy::ExternalLimits empty_limits;
            evse->call_set_external_limits(empty_limits);
            try {
                const auto external_limits = get_external_limits(data, false);
                evse->call_set_external_limits(external_limits);
            } catch (const std::invalid_argument& e) {
                EVLOG_warning << "Invalid limit: No conversion of given input could be performed.";
            } catch (const std::out_of_range& e) {
                EVLOG_warning << "Invalid limit: Out of range.";
            }
        });

        std::string cmd_set_limit_watts = cmd_base + "set_limit_watts";
        this->mqtt.subscribe(cmd_set_limit_watts, [&evse](const std::string& data) {
            // reset previous limits set by this module
            types::energy::ExternalLimits empty_limits;
            evse->call_set_external_limits(empty_limits);
            try {
                const auto external_limits = get_external_limits(data, true);
                evse->call_set_external_limits(external_limits);
            } catch (const std::invalid_argument& e) {
                EVLOG_warning << "Invalid limit: No conversion of given input could be performed.";
            } catch (const std::out_of_range& e) {
                EVLOG_warning << "Invalid limit: Out of range.";
            }
        });
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

void API::ready() {
    invoke_ready(*p_main);
}

} // namespace module
