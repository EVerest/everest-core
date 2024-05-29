// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "evse_managerImpl.hpp"
#include <utils/date.hpp>

#include <date/date.h>
#include <date/tz.h>
#include <utils/date.hpp>

#include <fmt/core.h>

#include "SessionLog.hpp"

namespace module {

namespace evse {

bool str_to_bool(const std::string& data) {
    if (data == "true") {
        return true;
    }
    return false;
}

void evse_managerImpl::init() {
    limits.nr_of_phases_available = 1;
    limits.max_current = 0.;

    // Interface to Node-RED debug UI

    mod->mqtt.subscribe(
        fmt::format("everest_external/nodered/{}/cmd/set_max_current", mod->config.connector_id),
        [&charger = mod->charger, this](std::string data) { mod->updateLocalMaxCurrentLimit(std::stof(data)); });

    mod->mqtt.subscribe(
        fmt::format("everest_external/nodered/{}/cmd/set_max_watt", mod->config.connector_id),
        [&charger = mod->charger, this](std::string data) { mod->updateLocalMaxWattLimit(std::stof(data)); });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/enable", mod->config.connector_id),
                        [&charger = mod->charger](const std::string& data) { charger->enable(0); });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/disable", mod->config.connector_id),
                        [&charger = mod->charger](const std::string& data) { charger->disable(0); });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/faulted", mod->config.connector_id),
                        [&charger = mod->charger](const std::string& data) { charger->set_faulted(); });

    mod->mqtt.subscribe(
        fmt::format("everest_external/nodered/{}/cmd/switch_three_phases_while_charging", mod->config.connector_id),
        [&charger = mod->charger](const std::string& data) {
            charger->switch_three_phases_while_charging(str_to_bool(data));
        });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/pause_charging", mod->config.connector_id),
                        [&charger = mod->charger](const std::string& data) { charger->pause_charging(); });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/resume_charging", mod->config.connector_id),
                        [&charger = mod->charger](const std::string& data) { charger->resume_charging(); });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/stop_transaction", mod->config.connector_id),
                        [this](const std::string& data) {
                            types::evse_manager::StopTransactionRequest request;
                            request.reason = types::evse_manager::StopTransactionReason::Local;
                            mod->charger->cancel_transaction(request);
                        });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/emergency_stop", mod->config.connector_id),
                        [this](const std::string& data) {
                            if (mod->get_hlc_enabled()) {
                                mod->r_hlc[0]->call_send_error(
                                    types::iso15118_charger::EvseError::Error_EmergencyShutdown);
                            }
                            types::evse_manager::StopTransactionRequest request;
                            request.reason = types::evse_manager::StopTransactionReason::EmergencyStop;
                            mod->charger->cancel_transaction(request);
                        });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/evse_malfunction", mod->config.connector_id),
                        [this](const std::string& data) {
                            if (mod->get_hlc_enabled()) {
                                mod->r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
                            }
                            types::evse_manager::StopTransactionRequest request;
                            request.reason = types::evse_manager::StopTransactionReason::Other;
                            mod->charger->cancel_transaction(request);
                        });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/evse_utility_int", mod->config.connector_id),
                        [this](const std::string& data) {
                            if (mod->get_hlc_enabled()) {
                                mod->r_hlc[0]->call_send_error(
                                    types::iso15118_charger::EvseError::Error_UtilityInterruptEvent);
                            }
                            types::evse_manager::StopTransactionRequest request;
                            request.reason = types::evse_manager::StopTransactionReason::Other;
                            mod->charger->cancel_transaction(request);
                        });

    // /Interface to Node-RED debug UI

    if (mod->r_powermeter_billing().size() > 0) {
        mod->r_powermeter_billing()[0]->subscribe_powermeter([this](const types::powermeter::Powermeter p) {
            // Republish data on proxy powermeter struct
            publish_powermeter(p);
        });
    }
}

void evse_managerImpl::ready() {

    // Register callbacks for errors/permanent faults
    mod->error_handling->signal_error.connect([this](const types::evse_manager::Error e, const bool prevent_charging) {
        types::evse_manager::SessionEvent se;

        se.error = e;
        se.uuid = mod->charger->get_session_id();

        if (prevent_charging) {
            se.event = types::evse_manager::SessionEventEnum::PermanentFault;
        } else {
            se.event = types::evse_manager::SessionEventEnum::Error;
        }
        publish_session_event(se);
    });

    mod->error_handling->signal_error_cleared.connect(
        [this](const types::evse_manager::Error e, const bool prevent_charging) {
            types::evse_manager::SessionEvent se;

            se.error = e;
            se.uuid = mod->charger->get_session_id();

            if (prevent_charging) {
                se.event = types::evse_manager::SessionEventEnum::PermanentFaultCleared;
            } else {
                se.event = types::evse_manager::SessionEventEnum::ErrorCleared;
            }
            publish_session_event(se);
        });

    // publish evse id at least once
    publish_evse_id(mod->config.evse_id);

    mod->signalNrOfPhasesAvailable.connect([this](const int n) {
        if (n >= 1 && n <= 3) {
            limits.nr_of_phases_available = n;
            publish_limits(limits);
        }
    });

    mod->r_bsp->subscribe_telemetry([this](types::evse_board_support::Telemetry telemetry) {
        // external Nodered interface
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/temperature", mod->config.connector_id),
                          telemetry.evse_temperature_C);
        // /external Nodered interface
        publish_telemetry(telemetry);
    });

    // The module code generates the reservation events and we merely publish them here
    mod->signalReservationEvent.connect([this](types::evse_manager::SessionEvent j) {
        j.uuid.clear(); // Reservation is not part of a session
        publish_session_event(j);
    });

    mod->charger->signal_session_started_event.connect(
        [this](const types::evse_manager::StartSessionReason& start_reason,
               const std::optional<types::authorization::ProvidedIdToken>& provided_id_token) {
            types::evse_manager::SessionEvent se;
            se.event = types::evse_manager::SessionEventEnum::SessionStarted;
            this->mod->selected_protocol = "IEC61851-1";
            types::evse_manager::SessionStarted session_started;

            se.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
            session_started.meter_value = mod->get_latest_powermeter_data_billing();

            if (mod->config.disable_authentication &&
                start_reason == types::evse_manager::StartSessionReason::EVConnected) {

                // Free service, authorize immediately in a separate thread (to avoid dead lock with charger state
                // machine, this signal handler runs in state machine context)
                std::thread authorize_thread([this]() {
                    types::authorization::ProvidedIdToken provided_token;
                    provided_token.authorization_type = types::authorization::AuthorizationType::RFID;
                    provided_token.id_token = {"FREESERVICE", types::authorization::IdTokenType::Local};
                    provided_token.prevalidated = true;
                    mod->charger->authorize(true, provided_token);
                    mod->charger_was_authorized();
                });
                authorize_thread.detach();
            }

            session_started.reason = start_reason;
            const auto session_uuid = this->mod->charger->get_session_id();
            session_started.meter_value = mod->get_latest_powermeter_data_billing();
            session_started.id_tag = provided_id_token;
            if (mod->is_reserved()) {
                session_started.reservation_id = mod->get_reservation_id();
            }

            session_started.logging_path = session_log.startSession(
                mod->config.logfile_suffix == "session_uuid" ? session_uuid : mod->config.logfile_suffix);

            session_log.evse(false, fmt::format("Session Started: {}",
                                                types::evse_manager::start_session_reason_to_string(start_reason)));

            mod->telemetry.publish("session", "events",
                                   {
                                       {"timestamp", Everest::Date::to_rfc3339(date::utc_clock::now())},
                                       {"type", "session_started"},
                                       {"session_id", session_uuid},
                                       {"reason", types::evse_manager::start_session_reason_to_string(start_reason)},
                                   });

            se.session_started = session_started;
            se.uuid = session_uuid;
            publish_session_event(se);
        });

    mod->charger->signal_transaction_started_event.connect([this](
                                                               const types::authorization::ProvidedIdToken& id_token) {
        types::evse_manager::SessionEvent se;
        se.event = types::evse_manager::SessionEventEnum::TransactionStarted;
        types::evse_manager::TransactionStarted transaction_started;
        se.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

        transaction_started.meter_value = mod->get_latest_powermeter_data_billing();
        if (mod->is_reserved()) {
            transaction_started.reservation_id.emplace(mod->get_reservation_id());
            mod->cancel_reservation(false);
        }

        transaction_started.id_tag = id_token;

        double energy_import = transaction_started.meter_value.energy_Wh_import.total;

        session_log.evse(false, fmt::format("Transaction Started ({} kWh)", energy_import / 1000.));
        const auto session_uuid = this->mod->charger->get_session_id();

        Everest::TelemetryMap telemetry_data = {
            {"timestamp", Everest::Date::to_rfc3339(date::utc_clock::now())},
            {"type", "transaction_started"},
            {"session_id", session_uuid},
            {"energy_counter_import_wh", transaction_started.meter_value.energy_Wh_import.total},
            {"id_tag", transaction_started.id_tag.id_token.value}};

        if (transaction_started.meter_value.energy_Wh_export.has_value()) {
            telemetry_data["energy_counter_export_wh"] = transaction_started.meter_value.energy_Wh_export.value().total;
        }
        mod->telemetry.publish("session", "events", telemetry_data);

        se.transaction_started.emplace(transaction_started);
        se.uuid = session_uuid;
        publish_session_event(se);
    });

    mod->charger->signal_transaction_finished_event.connect(
        [this](const types::evse_manager::StopTransactionReason& finished_reason,
               std::optional<types::authorization::ProvidedIdToken> finish_token) {
            types::evse_manager::SessionEvent se;

            se.event = types::evse_manager::SessionEventEnum::TransactionFinished;
            this->mod->selected_protocol = "Unknown";
            types::evse_manager::TransactionFinished transaction_finished;

            se.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

            transaction_finished.meter_value = mod->get_latest_powermeter_data_billing();

            transaction_finished.reason.emplace(finished_reason);
            transaction_finished.id_tag = finish_token;

            double energy_import = transaction_finished.meter_value.energy_Wh_import.total;

            session_log.evse(false, fmt::format("Transaction Finished: {} ({} kWh)",
                                                types::evse_manager::stop_transaction_reason_to_string(finished_reason),
                                                energy_import / 1000.));
            const auto session_uuid = this->mod->charger->get_session_id();
            Everest::TelemetryMap telemetry_data = {
                {"timestamp", Everest::Date::to_rfc3339(date::utc_clock::now())},
                {"type", "transaction_finished"},
                {"session_id", session_uuid},
                {"energy_counter_import_wh", energy_import},
                {"reason", types::evse_manager::stop_transaction_reason_to_string(finished_reason)}};

            if (transaction_finished.meter_value.energy_Wh_export.has_value()) {
                telemetry_data["energy_counter_export_wh"] =
                    transaction_finished.meter_value.energy_Wh_export.value().total;
            }

            transaction_finished.start_signed_meter_value = mod->charger->get_start_signed_meter_value();
            transaction_finished.signed_meter_value = mod->charger->get_stop_signed_meter_value();
            mod->telemetry.publish("session", "events", telemetry_data);

            se.transaction_finished.emplace(transaction_finished);
            se.uuid = session_uuid;

            publish_session_event(se);
        });

    mod->charger->signal_simple_event.connect([this](const types::evse_manager::SessionEventEnum& e) {
        types::evse_manager::SessionEvent se;

        se.event = e;
        se.timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());

        const auto session_uuid = this->mod->charger->get_session_id();
        if (e == types::evse_manager::SessionEventEnum::SessionFinished) {
            types::evse_manager::SessionFinished session_finished;
            session_finished.meter_value = mod->get_latest_powermeter_data_billing();
            se.session_finished = session_finished;
            session_log.evse(false, fmt::format("Session Finished"));
            session_log.stopSession();
            mod->telemetry.publish("session", "events",
                                   {{"timestamp", Everest::Date::to_rfc3339(date::utc_clock::now())},
                                    {"type", "session_finished"},
                                    {"session_id", session_uuid}});
        } else if (e == types::evse_manager::SessionEventEnum::Enabled or
                   e == types::evse_manager::SessionEventEnum::Disabled) {
            if (connector_status_changed) {
                se.connector_id = 1;
            }
        } else if (e == types::evse_manager::SessionEventEnum::ChargingPausedEV or
                   e == types::evse_manager::SessionEventEnum::ChargingPausedEVSE or
                   e == types::evse_manager::SessionEventEnum::ChargingStarted or
                   e == types::evse_manager::SessionEventEnum::ChargingResumed) {
            types::evse_manager::ChargingStateChangedEvent charging_state_changed_event;
            charging_state_changed_event.meter_value = mod->get_latest_powermeter_data_billing();
            se.charging_state_changed_event = charging_state_changed_event;
        } else if (e == types::evse_manager::SessionEventEnum::Authorized or
                   e == types::evse_manager::SessionEventEnum::Deauthorized) {
            types::evse_manager::AuthorizationEvent authorization_event;
            authorization_event.meter_value = mod->get_latest_powermeter_data_billing();
            se.authorization_event = authorization_event;
        }

        se.uuid = session_uuid;

        publish_session_event(se);

        if (e == types::evse_manager::SessionEventEnum::SessionFinished) {
            this->mod->selected_protocol = "Unknown";
        }

        publish_selected_protocol(this->mod->selected_protocol);
    });

    // Note: Deprecated. Only kept for Node red compatibility, will be removed in the future
    // Legacy external mqtt pubs
    mod->charger->signal_max_current.connect([this](float c) {
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/max_current", mod->config.connector_id), c);

        limits.uuid = mod->info.id;
        limits.max_current = c;
        publish_limits(limits);
    });

    mod->charger->signal_state.connect([this](Charger::EvseState s) {
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/state_string", mod->config.connector_id),
                          mod->charger->evse_state_to_string(s));
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/state", mod->config.connector_id),
                          static_cast<int>(s));
    });
    // /Deprecated
}

types::evse_manager::Evse evse_managerImpl::handle_get_evse() {
    types::evse_manager::Evse evse;
    evse.id = this->mod->config.connector_id;

    // EvseManager currently only supports a single connector with id: 1;
    std::vector<types::evse_manager::Connector> connectors;
    types::evse_manager::Connector connector;
    connector.id = 1;
    connectors.push_back(connector);
    return evse;
}

bool evse_managerImpl::handle_enable(int& connector_id) {
    connector_status_changed = connector_id != 0;
    return mod->charger->enable(connector_id);
};

void evse_managerImpl::handle_authorize_response(types::authorization::ProvidedIdToken& provided_token,
                                                 types::authorization::ValidationResult& validation_result) {
    const auto pnc = provided_token.authorization_type == types::authorization::AuthorizationType::PlugAndCharge;

    if (validation_result.authorization_status == types::authorization::AuthorizationStatus::Accepted) {

        if (this->mod->get_hlc_waiting_for_auth_pnc() && !pnc) {
            EVLOG_info
                << "EvseManager received Authorization other than PnC while waiting for PnC. This has no effect.";
            return;
        }

        this->mod->charger->authorize(true, provided_token);
        mod->charger_was_authorized();
    }

    if (pnc) {
        this->mod->r_hlc[0]->call_authorization_response(
            validation_result.authorization_status,
            validation_result.certificate_status.value_or(types::authorization::CertificateStatus::Accepted));
    }
};

void evse_managerImpl::handle_withdraw_authorization() {
    this->mod->charger->deauthorize();
};

bool evse_managerImpl::handle_reserve(int& reservation_id) {
    return mod->reserve(reservation_id);
};

void evse_managerImpl::handle_cancel_reservation() {
    mod->cancel_reservation(true);
};

bool evse_managerImpl::handle_disable(int& connector_id) {
    connector_status_changed = connector_id != 0;
    return mod->charger->disable(connector_id);
};

void evse_managerImpl::handle_set_faulted() {
    mod->charger->set_faulted();
};

bool evse_managerImpl::handle_pause_charging() {
    return mod->charger->pause_charging();
};

bool evse_managerImpl::handle_resume_charging() {
    return mod->charger->resume_charging();
};

bool evse_managerImpl::handle_stop_transaction(types::evse_manager::StopTransactionRequest& request) {
    return mod->charger->cancel_transaction(request);
};

void evse_managerImpl::handle_set_external_limits(types::energy::ExternalLimits& value) {
    mod->updateLocalEnergyLimit(value);
}

types::evse_manager::SwitchThreePhasesWhileChargingResult
evse_managerImpl::handle_switch_three_phases_while_charging(bool& three_phases) {
    // FIXME implement more sophisticated error code return once feature is really implemented
    if (mod->charger->switch_three_phases_while_charging(three_phases)) {
        return types::evse_manager::SwitchThreePhasesWhileChargingResult::Success;
    } else {
        return types::evse_manager::SwitchThreePhasesWhileChargingResult::Error_NotSupported;
    }
};

void evse_managerImpl::handle_set_get_certificate_response(
    types::iso15118_charger::Response_Exi_Stream_Status& certificate_reponse) {
    mod->r_hlc[0]->call_certificate_response(certificate_reponse);
}

bool evse_managerImpl::handle_external_ready_to_start_charging() {
    if (mod->config.external_ready_to_start_charging) {
        EVLOG_info << "Received external ready to start charging command.";
        mod->ready_to_start_charging();
        return true;
    } else {
        EVLOG_warning
            << "Ignoring external ready to start charging command, this could be a configuration issue. Please check "
               "if 'external_ready_to_start_charging' is set to true if you want to use this feature.";
    }

    return false;
}

bool evse_managerImpl::handle_force_unlock(int& connector_id) {
    mod->bsp->connector_force_unlock();
    return true;
};

} // namespace evse
} // namespace module
