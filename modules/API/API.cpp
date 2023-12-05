// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "API.hpp"
#include <utils/date.hpp>
#include <utils/yaml_loader.hpp>

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
        this->state = "Preparing";
    } else if (event == "ChargingPausedEV") {
        this->state = "ChargingPausedEV";
    } else if (event == "ChargingPausedEVSE") {
        this->state = "ChargingPausedEVSE";
    } else if (event == "ChargingStarted") {
        this->state = "Charging";
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
    this->limit_decimal_places = std::make_unique<LimitDecimalPlaces>(this->config);
    std::string api_base = "everest_api/";
    std::vector<std::string> connectors;
    std::string var_connectors = api_base + "connectors";

    for (auto& evse : this->r_evse_manager) {
        auto& session_info = this->info.emplace_back(std::make_unique<SessionInfo>());
        auto& hw_caps = this->hw_capabilities_str.emplace_back("");
        std::string evse_base = api_base + evse->module_id;
        connectors.push_back(evse->module_id);

        // API variables
        std::string var_base = evse_base + "/var/";

        std::string var_hw_caps = var_base + "hardware_capabilities";
        evse->subscribe_hw_capabilities(
            [this, var_hw_caps, &hw_caps](types::evse_board_support::HardwareCapabilities hw_capabilities) {
                hw_caps = this->limit_decimal_places->limit(hw_capabilities);
                this->mqtt.publish(var_hw_caps, hw_caps);
            });

        std::string var_powermeter = var_base + "powermeter";
        evse->subscribe_powermeter([this, var_powermeter, &session_info](types::powermeter::Powermeter powermeter) {
            this->mqtt.publish(var_powermeter, this->limit_decimal_places->limit(powermeter));
            session_info->set_latest_energy_import_wh(powermeter.energy_Wh_import.total);
            if (powermeter.energy_Wh_export.has_value()) {
                session_info->set_latest_energy_export_wh(powermeter.energy_Wh_export.value().total);
            }
            if (powermeter.power_W.has_value()) {
                session_info->set_latest_total_w(powermeter.power_W.value().total);
            }
        });

        std::string var_limits = var_base + "limits";
        evse->subscribe_limits([this, var_limits](types::evse_manager::Limits limits) {
            this->mqtt.publish(var_limits, this->limit_decimal_places->limit(limits));
        });

        std::string var_telemetry = var_base + "telemetry";
        evse->subscribe_telemetry([this, var_telemetry](types::evse_board_support::Telemetry telemetry) {
            this->mqtt.publish(var_telemetry, this->limit_decimal_places->limit(telemetry));
        });

        std::string var_ev_info = var_base + "ev_info";
        evse->subscribe_ev_info([this, var_ev_info](types::evse_manager::EVInfo ev_info) {
            json ev_info_json = ev_info;
            this->mqtt.publish(var_ev_info, ev_info_json.dump());
        });

        std::string var_selected_protocol = var_base + "selected_protocol";
        evse->subscribe_selected_protocol([this, var_selected_protocol](const std::string& selected_protocol) {
            this->selected_protocol = selected_protocol;
        });

        std::string var_datetime = var_base + "datetime";
        std::string var_session_info = var_base + "session_info";
        std::string var_logging_path = var_base + "logging_path";
        this->api_threads.push_back(std::thread(
            [this, var_datetime, var_session_info, var_hw_caps, var_selected_protocol, &session_info, &hw_caps]() {
                auto next_tick = std::chrono::steady_clock::now();
                while (this->running) {
                    std::string datetime_str = Everest::Date::to_rfc3339(date::utc_clock::now());
                    this->mqtt.publish(var_datetime, datetime_str);
                    this->mqtt.publish(var_session_info, *session_info);
                    this->mqtt.publish(var_hw_caps, hw_caps);
                    this->mqtt.publish(var_selected_protocol, this->selected_protocol);

                    next_tick += NOTIFICATION_PERIOD;
                    std::this_thread::sleep_until(next_tick);
                }
            }));

        evse->subscribe_session_event(
            [this, var_session_info, var_logging_path, &session_info](types::evse_manager::SessionEvent session_event) {
                auto event = types::evse_manager::session_event_enum_to_string(session_event.event);
                std::string state_info = "";
                if (session_event.error.has_value()) {
                    state_info = types::evse_manager::error_enum_to_string(session_event.error.value().error_code);
                }
                session_info->update_state(event, state_info);

                if (session_event.event == types::evse_manager::SessionEventEnum::SessionStarted) {
                    if (session_event.session_started.has_value()) {
                        auto session_started = session_event.session_started.value();
                        if (session_started.logging_path.has_value()) {
                            this->mqtt.publish(var_logging_path, session_started.logging_path.value());
                        }
                    }
                }
                if (session_event.event == types::evse_manager::SessionEventEnum::TransactionStarted) {
                    if (session_event.transaction_started.has_value()) {
                        auto transaction_started = session_event.transaction_started.value();
                        auto energy_Wh_import = transaction_started.meter_value.energy_Wh_import.total;
                        session_info->set_start_energy_import_wh(energy_Wh_import);

                        if (transaction_started.meter_value.energy_Wh_export.has_value()) {
                            auto energy_Wh_export = transaction_started.meter_value.energy_Wh_export.value().total;
                            session_info->set_start_energy_export_wh(energy_Wh_export);
                        } else {
                            session_info->start_energy_export_wh_was_set = false;
                        }
                    }
                } else if (session_event.event == types::evse_manager::SessionEventEnum::TransactionFinished) {
                    if (session_event.transaction_finished.has_value()) {
                        auto transaction_finished = session_event.transaction_finished.value();
                        auto energy_Wh_import = transaction_finished.meter_value.energy_Wh_import.total;
                        session_info->set_end_energy_import_wh(energy_Wh_import);

                        if (transaction_finished.meter_value.energy_Wh_export.has_value()) {
                            auto energy_Wh_export = transaction_finished.meter_value.energy_Wh_export.value().total;
                            session_info->set_end_energy_export_wh(energy_Wh_export);
                        } else {
                            session_info->end_energy_export_wh_was_set = false;
                        }
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
            try {
                const auto external_limits = get_external_limits(data, true);
                evse->call_set_external_limits(external_limits);
            } catch (const std::invalid_argument& e) {
                EVLOG_warning << "Invalid limit: No conversion of given input could be performed.";
            } catch (const std::out_of_range& e) {
                EVLOG_warning << "Invalid limit: Out of range.";
            }
        });
        std::string cmd_force_unlock = cmd_base + "force_unlock";
        this->mqtt.subscribe(cmd_force_unlock, [&evse](const std::string& data) {
            int connector_id = 1;
            if (!data.empty()) {
                try {
                    connector_id = std::stoi(data);
                } catch (const std::exception& e) {
                    EVLOG_error << "Could not parse connector id for force unlock, using " << connector_id
                                << ", error: " << e.what();
                }
            }
            evse->call_force_unlock(connector_id); //
        });
    }

    std::string var_ocpp_connection_status = api_base + "ocpp/var/connection_status";

    if (this->r_ocpp.size() == 1) {
        this->r_ocpp.at(0)->subscribe_is_connected([this](bool is_connected) {
            if (is_connected) {
                this->ocpp_connection_status = "connected";
            } else {
                this->ocpp_connection_status = "disconnected";
            }
        });
    }

    std::string var_info = api_base + "info/var/info";

    if (this->config.charger_information_file != "") {
        auto charger_information_path = std::filesystem::path(this->config.charger_information_file);
        try {
            this->charger_information = Everest::load_yaml(charger_information_path);

        } catch (const std::exception& err) {
            EVLOG_error << "Error parsing charger information file at " << this->config.charger_information_file << ": "
                        << err.what();
        }
    }

    this->api_threads.push_back(std::thread([this, var_connectors, connectors, var_info, var_ocpp_connection_status]() {
        auto next_tick = std::chrono::steady_clock::now();
        while (this->running) {
            json connectors_array = connectors;
            this->mqtt.publish(var_connectors, connectors_array.dump());
            this->mqtt.publish(var_info, this->charger_information.dump());
            this->mqtt.publish(var_ocpp_connection_status, this->ocpp_connection_status);

            next_tick += NOTIFICATION_PERIOD;
            std::this_thread::sleep_until(next_tick);
        }
    }));
}

void API::ready() {
    invoke_ready(*p_main);
}

} // namespace module
