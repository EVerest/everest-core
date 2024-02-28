// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "API.hpp"
#include <utils/date.hpp>
#include <utils/yaml_loader.hpp>

namespace module {

static const auto NOTIFICATION_PERIOD = std::chrono::seconds(1);

SessionInfo::SessionInfo() :
    state(State::Unknown),
    start_energy_import_wh(0),
    end_energy_import_wh(0),
    latest_total_w(0),
    start_energy_export_wh(0),
    end_energy_export_wh(0) {
    this->start_time_point = date::utc_clock::now();
    this->end_time_point = this->start_time_point;

    uk_random_delay_remaining.countdown_s = 0;
    uk_random_delay_remaining.current_limit_after_delay_A = 0.;
    uk_random_delay_remaining.current_limit_during_delay_A = 0;
}

bool SessionInfo::is_state_charging(const SessionInfo::State current_state) {
    if (current_state == State::AuthRequired || current_state == State::Charging ||
        current_state == State::ChargingPausedEV || current_state == State::ChargingPausedEVSE) {
        return true;
    }
    return false;
}

void SessionInfo::reset() {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->state = State::Unknown;
    this->active_permanent_faults.clear();
    this->active_errors.clear();
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

static void remove_error_from_list(std::vector<module::SessionInfo::Error>& list, const std::string& error_type) {
    list.erase(std::remove_if(list.begin(), list.end(),
                              [error_type](const module::SessionInfo::Error& err) { return err.type == error_type; }),
               list.end());
}

void SessionInfo::update_state(const types::evse_manager::SessionEventEnum event, const SessionInfo::Error& error) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    using Event = types::evse_manager::SessionEventEnum;

    if (event == Event::Enabled) {
        this->state = State::Unplugged;
    } else if (event == Event::Disabled) {
        this->state = State::Disabled;
    } else if (event == Event::SessionStarted) {
        this->state = State::Preparing;
    } else if (event == Event::ReservationStart) {
        this->state = State::Reserved;
    } else if (event == Event::ReservationEnd) {
        this->state = State::Unplugged;
    } else if (event == Event::AuthRequired) {
        this->state = State::AuthRequired;
    } else if (event == Event::WaitingForEnergy) {
        this->state = State::WaitingForEnergy;
    } else if (event == Event::TransactionStarted) {
        this->state = State::Preparing;
    } else if (event == Event::ChargingPausedEV) {
        this->state = State::ChargingPausedEV;
    } else if (event == Event::ChargingPausedEVSE) {
        this->state = State::ChargingPausedEVSE;
    } else if (event == Event::ChargingStarted) {
        this->state = State::Charging;
    } else if (event == Event::ChargingResumed) {
        this->state = State::Charging;
    } else if (event == Event::TransactionFinished) {
        this->state = State::Finished;
    } else if (event == Event::SessionFinished) {
        this->state = State::Unplugged;
    } else if (event == Event::PermanentFault) {
        this->active_permanent_faults.push_back(error);
    } else if (event == Event::Error) {
        this->active_errors.push_back(error);
    } else if (event == Event::PermanentFaultCleared or event == Event::ErrorCleared) {
        remove_error_from_list(this->active_permanent_faults, error.type);
    } else if (event == Event::AllErrorsCleared) {
        this->active_permanent_faults.clear();
        this->active_errors.clear();
    }
}

std::string SessionInfo::state_to_string(SessionInfo::State s) {
    switch (s) {
    case SessionInfo::State::Unplugged:
        return "Unplugged";
    case SessionInfo::State::Disabled:
        return "Disabled";
    case SessionInfo::State::Preparing:
        return "Preparing";
    case SessionInfo::State::Reserved:
        return "Reserved";
    case SessionInfo::State::AuthRequired:
        return "AuthRequired";
    case SessionInfo::State::WaitingForEnergy:
        return "WaitingForEnergy";
    case SessionInfo::State::ChargingPausedEV:
        return "ChargingPausedEV";
    case SessionInfo::State::ChargingPausedEVSE:
        return "ChargingPausedEVSE";
    case SessionInfo::State::Charging:
        return "Charging";
    case SessionInfo::State::Finished:
        return "Finished";
    }
    return "Unknown";
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

void SessionInfo::set_uk_random_delay_remaining(const types::uk_random_delay::CountDown& cd) {
    std::lock_guard<std::mutex> lock(this->session_info_mutex);
    this->uk_random_delay_remaining = cd;
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

    json session_info = json::object({{"state", state_to_string(this->state)},
                                      {"active_permanent_faults", this->active_permanent_faults},
                                      {"active_errors", this->active_errors},
                                      {"charged_energy_wh", charged_energy_wh},
                                      {"discharged_energy_wh", discharged_energy_wh},
                                      {"latest_total_w", this->latest_total_w},
                                      {"charging_duration_s", charging_duration_s.count()},
                                      {"datetime", Everest::Date::to_rfc3339(now)}});
    if (uk_random_delay_remaining.countdown_s > 0) {
        json random_delay =
            json::object({{"remaining_s", uk_random_delay_remaining.countdown_s},
                          {"current_limit_after_delay_A", uk_random_delay_remaining.current_limit_after_delay_A},
                          {"current_limit_during_delay_A", uk_random_delay_remaining.current_limit_during_delay_A}});
        session_info["uk_random_delay"] = random_delay;
    }

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
                SessionInfo::Error error;
                if (session_event.error.has_value()) {
                    error.type = types::evse_manager::error_enum_to_string(session_event.error.value().error_code);
                    error.description = session_event.error.value().error_description;
                    error.severity =
                        types::evse_manager::error_severity_to_string(session_event.error.value().error_severity);
                }

                session_info->update_state(session_event.event, error);

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

        std::string cmd_enable = cmd_base + "enable";
        this->mqtt.subscribe(cmd_enable, [&evse](const std::string& data) {
            auto connector_id = 0;
            if (!data.empty()) {
                try {
                    connector_id = std::stoi(data);
                } catch (const std::exception& e) {
                    EVLOG_error << "Could not parse connector id for enable connector, ignoring command, error: "
                                << e.what();
                    return;
                }
            }
            evse->call_enable(connector_id);
        });

        std::string cmd_disable = cmd_base + "disable";
        this->mqtt.subscribe(cmd_disable, [&evse](const std::string& data) {
            auto connector_id = 0;
            if (!data.empty()) {
                try {
                    connector_id = std::stoi(data);
                } catch (const std::exception& e) {
                    EVLOG_error << "Could not parse connector id for disable connector, ignoring command, error: "
                                << e.what();
                    return;
                }
            }
            evse->call_disable(connector_id);
        });

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

        // Check if a uk_random_delay is connected that matches this evse_manager
        for (auto& random_delay : this->r_random_delay) {
            if (random_delay->module_id == evse->module_id) {

                random_delay->subscribe_countdown([&session_info](const types::uk_random_delay::CountDown& s) {
                    session_info->set_uk_random_delay_remaining(s);
                });

                std::string cmd_uk_random_delay = cmd_base + "uk_random_delay";
                this->mqtt.subscribe(cmd_uk_random_delay, [&random_delay](const std::string& data) {
                    if (data == "enable") {
                        random_delay->call_enable();
                    } else if (data == "disable") {
                        random_delay->call_disable();
                    } else if (data == "cancel") {
                        random_delay->call_cancel();
                    }
                });

                std::string uk_random_delay_set_max_duration_s = cmd_base + "uk_random_delay_set_max_duration_s";
                this->mqtt.subscribe(uk_random_delay_set_max_duration_s, [&random_delay](const std::string& data) {
                    int seconds = 600;
                    try {
                        seconds = std::stoi(data);
                    } catch (const std::exception& e) {
                        EVLOG_error
                            << "Could not parse connector duration value for uk_random_delay_set_max_duration_s: "
                            << e.what();
                    }
                    random_delay->call_set_duration_s(seconds);
                });
            }
        }
    }

    std::string var_ocpp_connection_status = api_base + "ocpp/var/connection_status";
    std::string var_ocpp_schedule = api_base + "ocpp/var/charging_schedules";

    if (this->r_ocpp.size() == 1) {

        this->r_ocpp.at(0)->subscribe_is_connected([this](bool is_connected) {
            std::scoped_lock lock(ocpp_data_mutex);
            if (is_connected) {
                this->ocpp_connection_status = "connected";
            } else {
                this->ocpp_connection_status = "disconnected";
            }
        });

        this->r_ocpp.at(0)->subscribe_charging_schedules([this, &var_ocpp_schedule](json schedule) {
            std::scoped_lock lock(ocpp_data_mutex);
            this->ocpp_charging_schedule = schedule;
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

    this->api_threads.push_back(
        std::thread([this, var_connectors, connectors, var_info, var_ocpp_connection_status, var_ocpp_schedule]() {
            auto next_tick = std::chrono::steady_clock::now();
            while (this->running) {
                json connectors_array = connectors;
                this->mqtt.publish(var_connectors, connectors_array.dump());
                this->mqtt.publish(var_info, this->charger_information.dump());
                {
                    std::scoped_lock lock(ocpp_data_mutex);
                    this->mqtt.publish(var_ocpp_connection_status, this->ocpp_connection_status);
                    this->mqtt.publish(var_ocpp_schedule, ocpp_charging_schedule.dump());
                }

                next_tick += NOTIFICATION_PERIOD;
                std::this_thread::sleep_until(next_tick);
            }
        }));
}

void API::ready() {
    invoke_ready(*p_main);
}

} // namespace module
