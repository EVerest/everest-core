// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "API.hpp"
#include <utils/date.hpp>
#include <utils/yaml_loader.hpp>

#include <cmath>

#include <c4/format.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>

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
        auto& session_info = this->info.emplace_back(std::make_unique<SessionInfo>());
        auto& hw_caps = this->hw_capabilities_str.emplace_back("");
        std::string evse_base = api_base + evse->module_id;
        connectors.push_back(evse->module_id);

        // API variables
        std::string var_base = evse_base + "/var/";

        std::string var_hw_caps = var_base + "hardware_capabilities";
        evse->subscribe_hw_capabilities(
            [this, var_hw_caps, &hw_caps](types::board_support::HardwareCapabilities hw_capabilities) {
                hw_caps = this->limit_decimal_places(hw_capabilities);
                this->mqtt.publish(var_hw_caps, hw_caps);
            });

        std::string var_powermeter = var_base + "powermeter";
        evse->subscribe_powermeter([this, var_powermeter, &session_info](types::powermeter::Powermeter powermeter) {
            this->mqtt.publish(var_powermeter, this->limit_decimal_places(powermeter));
            session_info->set_latest_energy_import_wh(powermeter.energy_Wh_import.total);
            if (powermeter.energy_Wh_export.has_value()) {
                session_info->set_latest_energy_export_wh(powermeter.energy_Wh_export.value().total);
            }
            if (powermeter.power_W.has_value())
                session_info->set_latest_total_w(powermeter.power_W.value().total);
        });

        std::string var_limits = var_base + "limits";
        evse->subscribe_limits([this, var_limits](types::evse_manager::Limits limits) {
            this->mqtt.publish(var_limits, this->limit_decimal_places(limits));
        });

        std::string var_telemetry = var_base + "telemetry";
        evse->subscribe_telemetry([this, var_telemetry](types::board_support::Telemetry telemetry) {
            this->mqtt.publish(var_telemetry, this->limit_decimal_places(telemetry));
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
                if (session_event.error) {
                    session_info->update_state(
                        event, types::evse_manager::error_enum_to_string(session_event.error.value().error_code));
                } else {
                    session_info->update_state(event, "");
                }
                if (session_event.event == types::evse_manager::SessionEventEnum::SessionStarted) {
                    if (session_event.session_started.has_value()) {
                        auto session_started = session_event.session_started.value();
                        if (session_started.logging_path.has_value()) {
                            this->mqtt.publish(var_logging_path, session_started.logging_path.value());
                        }
                    }
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

    this->api_threads.push_back(std::thread([this, var_connectors, connectors, var_info]() {
        auto next_tick = std::chrono::steady_clock::now();
        while (this->running) {
            json connectors_array = connectors;
            this->mqtt.publish(var_connectors, connectors_array.dump());
            this->mqtt.publish(var_info, this->charger_information.dump());

            next_tick += NOTIFICATION_PERIOD;
            std::this_thread::sleep_until(next_tick);
        }
    }));
}

void API::ready() {
    invoke_ready(*p_main);
}

std::string API::limit_decimal_places(const types::powermeter::Powermeter& powermeter) {
    ryml::Tree tree;
    ryml::NodeRef root = tree.rootref();
    root |= ryml::MAP;

    // add informative power meter entries
    root["timestamp"] << powermeter.timestamp;

    if (powermeter.meter_id.has_value()) {
        root["meter_id"] << powermeter.meter_id.value();
    }

    if (powermeter.phase_seq_error.has_value()) {
        root["phase_seq_error"] << ryml::fmt::boolalpha(powermeter.phase_seq_error.value());
    }

    // limit decimal places

    // energy_Wh_import always exists
    root["energy_Wh_import"] |= ryml::MAP;
    root["energy_Wh_import"]["total"] << ryml::fmt::real(
        this->round_to_nearest_step(powermeter.energy_Wh_import.total, this->config.powermeter_energy_import_round_to),
        this->config.powermeter_energy_import_decimal_places);
    if (powermeter.energy_Wh_import.L1.has_value()) {
        root["energy_Wh_import"]["L1"] << ryml::fmt::real(
            this->round_to_nearest_step(powermeter.energy_Wh_import.L1.value(),
                                        this->config.powermeter_energy_import_round_to),
            this->config.powermeter_energy_import_decimal_places);
    }
    if (powermeter.energy_Wh_import.L2.has_value()) {
        root["energy_Wh_import"]["L2"] << ryml::fmt::real(
            this->round_to_nearest_step(powermeter.energy_Wh_import.L2.value(),
                                        this->config.powermeter_energy_import_round_to),
            this->config.powermeter_energy_import_decimal_places);
    }
    if (powermeter.energy_Wh_import.L3.has_value()) {
        root["energy_Wh_import"]["L3"] << ryml::fmt::real(
            this->round_to_nearest_step(powermeter.energy_Wh_import.L3.value(),
                                        this->config.powermeter_energy_import_round_to),
            this->config.powermeter_energy_import_decimal_places);
    }

    // everything else in the power meter is optional
    if (powermeter.energy_Wh_export.has_value()) {
        auto& energy_Wh_export = powermeter.energy_Wh_export.value();
        root["energy_Wh_export"] |= ryml::MAP;
        root["energy_Wh_export"]["total"] << ryml::fmt::real(
            this->round_to_nearest_step(energy_Wh_export.total, this->config.powermeter_energy_export_round_to),
            this->config.powermeter_energy_export_decimal_places);
        if (energy_Wh_export.L1.has_value()) {
            root["energy_Wh_export"]["L1"]
                << ryml::fmt::real(this->round_to_nearest_step(energy_Wh_export.L1.value(),
                                                               this->config.powermeter_energy_export_round_to),
                                   this->config.powermeter_energy_export_decimal_places);
        }
        if (energy_Wh_export.L2.has_value()) {
            root["energy_Wh_export"]["L2"]
                << ryml::fmt::real(this->round_to_nearest_step(energy_Wh_export.L2.value(),
                                                               this->config.powermeter_energy_export_round_to),
                                   this->config.powermeter_energy_export_decimal_places);
        }
        if (energy_Wh_export.L3.has_value()) {
            root["energy_Wh_export"]["L3"]
                << ryml::fmt::real(this->round_to_nearest_step(energy_Wh_export.L3.value(),
                                                               this->config.powermeter_energy_export_round_to),
                                   this->config.powermeter_energy_export_decimal_places);
        }
    }

    if (powermeter.power_W.has_value()) {
        auto& power_W = powermeter.power_W.value();
        root["power_W"] |= ryml::MAP;
        root["power_W"]["total"] << ryml::fmt::real(
            this->round_to_nearest_step(power_W.total, this->config.powermeter_power_round_to),
            this->config.powermeter_power_decimal_places);
        if (power_W.L1.has_value()) {
            root["power_W"]["L1"] << ryml::fmt::real(
                this->round_to_nearest_step(power_W.L1.value(), this->config.powermeter_power_round_to),
                this->config.powermeter_power_decimal_places);
        }
        if (power_W.L2.has_value()) {
            root["power_W"]["L2"] << ryml::fmt::real(
                this->round_to_nearest_step(power_W.L2.value(), this->config.powermeter_power_round_to),
                this->config.powermeter_power_decimal_places);
        }
        if (power_W.L3.has_value()) {
            root["power_W"]["L3"] << ryml::fmt::real(
                this->round_to_nearest_step(power_W.L3.value(), this->config.powermeter_power_round_to),
                this->config.powermeter_power_decimal_places);
        }
    }

    if (powermeter.voltage_V.has_value()) {
        auto& voltage_V = powermeter.voltage_V.value();
        root["voltage_V"] |= ryml::MAP;
        if (voltage_V.DC.has_value()) {
            root["voltage_V"]["DC"] << ryml::fmt::real(
                this->round_to_nearest_step(voltage_V.L1.value(), this->config.powermeter_voltage_round_to),
                this->config.powermeter_voltage_decimal_places);
        }
        if (voltage_V.L1.has_value()) {
            root["voltage_V"]["L1"] << ryml::fmt::real(
                this->round_to_nearest_step(voltage_V.L1.value(), this->config.powermeter_voltage_round_to),
                this->config.powermeter_voltage_decimal_places);
        }
        if (voltage_V.L2.has_value()) {
            root["voltage_V"]["L2"] << ryml::fmt::real(
                this->round_to_nearest_step(voltage_V.L2.value(), this->config.powermeter_voltage_round_to),
                this->config.powermeter_voltage_decimal_places);
        }
        if (voltage_V.L3.has_value()) {
            root["voltage_V"]["L3"] << ryml::fmt::real(
                this->round_to_nearest_step(voltage_V.L3.value(), this->config.powermeter_voltage_round_to),
                this->config.powermeter_voltage_decimal_places);
        }
    }

    if (powermeter.VAR.has_value()) {
        auto& VAR = powermeter.VAR.value();
        root["VAR"] |= ryml::MAP;
        root["VAR"]["total"] << ryml::fmt::real(
            this->round_to_nearest_step(VAR.total, this->config.powermeter_VAR_round_to),
            this->config.powermeter_VAR_decimal_places);
        if (VAR.L1.has_value()) {
            root["VAR"]["L1"] << ryml::fmt::real(
                this->round_to_nearest_step(VAR.L1.value(), this->config.powermeter_VAR_round_to),
                this->config.powermeter_VAR_decimal_places);
        }
        if (VAR.L2.has_value()) {
            root["VAR"]["L2"] << ryml::fmt::real(
                this->round_to_nearest_step(VAR.L2.value(), this->config.powermeter_VAR_round_to),
                this->config.powermeter_VAR_decimal_places);
        }
        if (VAR.L3.has_value()) {
            root["VAR"]["L3"] << ryml::fmt::real(
                this->round_to_nearest_step(VAR.L3.value(), this->config.powermeter_VAR_round_to),
                this->config.powermeter_VAR_decimal_places);
        }
    }

    if (powermeter.current_A.has_value()) {
        auto& current_A = powermeter.current_A.value();
        root["current_A"] |= ryml::MAP;
        if (current_A.DC.has_value()) {
            root["current_A"]["DC"] << ryml::fmt::real(
                this->round_to_nearest_step(current_A.L1.value(), this->config.powermeter_current_round_to),
                this->config.powermeter_current_decimal_places);
        }
        if (current_A.L1.has_value()) {
            root["current_A"]["L1"] << ryml::fmt::real(
                this->round_to_nearest_step(current_A.L1.value(), this->config.powermeter_current_round_to),
                this->config.powermeter_current_decimal_places);
        }
        if (current_A.L2.has_value()) {
            root["current_A"]["L2"] << ryml::fmt::real(
                this->round_to_nearest_step(current_A.L2.value(), this->config.powermeter_current_round_to),
                this->config.powermeter_current_decimal_places);
        }
        if (current_A.L3.has_value()) {
            root["current_A"]["L3"] << ryml::fmt::real(
                this->round_to_nearest_step(current_A.L3.value(), this->config.powermeter_current_round_to),
                this->config.powermeter_current_decimal_places);
        }

        if (current_A.N.has_value()) {
            root["current_A"]["N"] << ryml::fmt::real(
                this->round_to_nearest_step(current_A.N.value(), this->config.powermeter_current_round_to),
                this->config.powermeter_current_decimal_places);
        }
    }

    if (powermeter.frequency_Hz.has_value()) {
        auto& frequency_Hz = powermeter.frequency_Hz.value();
        root["frequency_Hz"] |= ryml::MAP;
        root["frequency_Hz"]["L1"] << ryml::fmt::real(
            this->round_to_nearest_step(frequency_Hz.L1, this->config.powermeter_frequency_round_to),
            this->config.powermeter_frequency_decimal_places);

        if (frequency_Hz.L2.has_value()) {
            root["frequency_Hz"]["L2"] << ryml::fmt::real(
                this->round_to_nearest_step(frequency_Hz.L2.value(), this->config.powermeter_frequency_round_to),
                this->config.powermeter_frequency_decimal_places);
        }
        if (frequency_Hz.L3.has_value()) {
            root["frequency_Hz"]["L3"] << ryml::fmt::real(
                this->round_to_nearest_step(frequency_Hz.L3.value(), this->config.powermeter_frequency_round_to),
                this->config.powermeter_frequency_decimal_places);
        }
    }

    std::stringstream power_meter_stream;
    power_meter_stream << ryml::as_json(tree);
    return power_meter_stream.str();
}

std::string API::limit_decimal_places(const types::board_support::HardwareCapabilities& hw_capabilities) {
    ryml::Tree tree;
    ryml::NodeRef root = tree.rootref();
    root |= ryml::MAP;

    // add informative hardware capabilities entries
    root["max_phase_count_import"] << hw_capabilities.max_phase_count_import;
    root["min_phase_count_import"] << hw_capabilities.min_phase_count_import;
    root["max_phase_count_export"] << hw_capabilities.max_phase_count_export;
    root["min_phase_count_export"] << hw_capabilities.min_phase_count_export;
    root["supports_changing_phases_during_charging"]
        << ryml::fmt::boolalpha(hw_capabilities.supports_changing_phases_during_charging);

    // limit decimal places

    root["max_current_A_import"] << ryml::fmt::real(
        this->round_to_nearest_step(hw_capabilities.max_current_A_import,
                                    this->config.hw_caps_max_current_import_round_to),
        this->config.hw_caps_max_current_import_decimal_places);
    root["min_current_A_import"] << ryml::fmt::real(
        this->round_to_nearest_step(hw_capabilities.min_current_A_import,
                                    this->config.hw_caps_min_current_import_round_to),
        this->config.hw_caps_max_current_import_decimal_places);
    root["max_current_A_export"] << ryml::fmt::real(
        this->round_to_nearest_step(hw_capabilities.max_current_A_export,
                                    this->config.hw_caps_max_current_export_round_to),
        this->config.hw_caps_max_current_import_decimal_places);
    root["min_current_A_export"] << ryml::fmt::real(
        this->round_to_nearest_step(hw_capabilities.min_current_A_export,
                                    this->config.hw_caps_min_current_export_round_to),
        this->config.hw_caps_min_current_export_decimal_places);

    std::stringstream hardware_capabilities_stream;
    hardware_capabilities_stream << ryml::as_json(tree);
    return hardware_capabilities_stream.str();
}

std::string API::limit_decimal_places(const types::evse_manager::Limits& limits) {
    ryml::Tree tree;
    ryml::NodeRef root = tree.rootref();
    root |= ryml::MAP;

    // add informative limits entries
    if (limits.uuid.has_value()) {
        root["uuid"] << limits.uuid.value();
    }
    root["nr_of_phases_available"] << limits.nr_of_phases_available;

    // limit decimal places
    root["max_current"] << ryml::fmt::real(
        this->round_to_nearest_step(limits.max_current, this->config.limits_max_current_round_to),
        this->config.limits_max_current_decimal_places);

    std::stringstream limits_stream;
    limits_stream << ryml::as_json(tree);
    return limits_stream.str();
}

std::string API::limit_decimal_places(const types::board_support::Telemetry& telemetry) {
    ryml::Tree tree;
    ryml::NodeRef root = tree.rootref();
    root |= ryml::MAP;

    root["phase_seq_error"] << ryml::fmt::boolalpha(telemetry.relais_on);

    // limit decimal places
    root["temperature"] << ryml::fmt::real(
        this->round_to_nearest_step(telemetry.temperature, this->config.telemetry_temperature_round_to),
        this->config.telemetry_temperature_decimal_places);
    root["fan_rpm"] << ryml::fmt::real(
        this->round_to_nearest_step(telemetry.fan_rpm, this->config.telemetry_fan_rpm_round_to),
        this->config.telemetry_fan_rpm_decimal_places);
    root["supply_voltage_12V"] << ryml::fmt::real(
        this->round_to_nearest_step(telemetry.supply_voltage_12V, this->config.telemetry_supply_voltage_12V_round_to),
        this->config.telemetry_supply_voltage_12V_decimal_places);
    root["supply_voltage_minus_12V"] << ryml::fmt::real(
        this->round_to_nearest_step(telemetry.supply_voltage_minus_12V,
                                    this->config.telemetry_supply_voltage_minus_12V_round_to),
        this->config.telemetry_supply_voltage_minus_12V_decimal_places);
    root["rcd_current"] << ryml::fmt::real(
        this->round_to_nearest_step(telemetry.rcd_current, this->config.telemetry_rcd_current_round_to),
        this->config.telemetry_rcd_current_decimal_places);
    std::stringstream telemetry_stream;
    telemetry_stream << ryml::as_json(tree);
    return telemetry_stream.str();
}

float API::round_to_nearest_step(float value, float step) {
    if (step <= 0) {
        return value;
    }
    return std::round(value / step) * step;
}

} // namespace module
