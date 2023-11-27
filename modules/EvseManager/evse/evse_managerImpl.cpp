// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "evse_managerImpl.hpp"
#include <utils/date.hpp>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
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
            charger->switchThreePhasesWhileCharging(str_to_bool(data));
        });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/pause_charging", mod->config.connector_id),
                        [&charger = mod->charger](const std::string& data) { charger->pauseCharging(); });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/resume_charging", mod->config.connector_id),
                        [&charger = mod->charger](const std::string& data) { charger->resumeCharging(); });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/stop_transaction", mod->config.connector_id),
                        [this](const std::string& data) {
                            types::evse_manager::StopTransactionRequest request;
                            request.reason = types::evse_manager::StopTransactionReason::Local;
                            mod->charger->cancelTransaction(request);
                        });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/emergency_stop", mod->config.connector_id),
                        [this](const std::string& data) {
                            if (mod->get_hlc_enabled()) {
                                mod->r_hlc[0]->call_send_error(
                                    types::iso15118_charger::EvseError::Error_EmergencyShutdown);
                            }
                            types::evse_manager::StopTransactionRequest request;
                            request.reason = types::evse_manager::StopTransactionReason::EmergencyStop;
                            mod->charger->cancelTransaction(request);
                        });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/evse_malfunction", mod->config.connector_id),
                        [this](const std::string& data) {
                            if (mod->get_hlc_enabled()) {
                                mod->r_hlc[0]->call_send_error(types::iso15118_charger::EvseError::Error_Malfunction);
                            }
                            types::evse_manager::StopTransactionRequest request;
                            request.reason = types::evse_manager::StopTransactionReason::Other;
                            mod->charger->cancelTransaction(request);
                        });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/evse_utility_int", mod->config.connector_id),
                        [this](const std::string& data) {
                            if (mod->get_hlc_enabled()) {
                                mod->r_hlc[0]->call_send_error(
                                    types::iso15118_charger::EvseError::Error_UtilityInterruptEvent);
                            }
                            types::evse_manager::StopTransactionRequest request;
                            request.reason = types::evse_manager::StopTransactionReason::Other;
                            mod->charger->cancelTransaction(request);
                        });

    // /Interface to Node-RED debug UI

    if (mod->r_powermeter_billing().size() > 0) {
        mod->r_powermeter_billing()[0]->subscribe_powermeter([this](const types::powermeter::Powermeter p) {
            // Republish data on proxy powermeter struct
            publish_powermeter(p);
        });
    }
}

void evse_managerImpl::set_session_uuid() {
    if (session_uuid.empty()) {
        session_uuid = generate_session_uuid();
    }
}

void evse_managerImpl::ready() {

    // publish evse id at least once
    publish_evse_id(mod->config.evse_id);

    mod->signalNrOfPhasesAvailable.connect([this](const int n) {
        if (n >= 1 && n <= 3) {
            limits.nr_of_phases_available = n;
            publish_limits(limits);
        }
    });

    mod->r_bsp->subscribe_telemetry([this](types::board_support::Telemetry telemetry) {
        // external Nodered interface
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/temperature", mod->config.connector_id),
                          telemetry.temperature);
        // /external Nodered interface
        publish_telemetry(telemetry);
    });

    // The module code generates the reservation events and we merely publish them here
    mod->signalReservationEvent.connect([this](types::evse_manager::SessionEvent j) {
        if (j.event == types::evse_manager::SessionEventEnum::ReservationStart) {
            set_session_uuid();
        }

        j.uuid = session_uuid;
        publish_session_event(j);
    });

    mod->charger->signalEvent.connect([this](const types::evse_manager::SessionEventEnum& e) {
        types::evse_manager::SessionEvent se;

        se.event = e;

        if (e == types::evse_manager::SessionEventEnum::SessionStarted) {
            this->mod->selected_protocol = "IEC61851-1";
            types::evse_manager::SessionStarted session_started;

            session_started.timestamp =
                date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(date::utc_clock::now()));

            auto reason = mod->charger->getSessionStartedReason();

            if (mod->config.disable_authentication && reason == types::evse_manager::StartSessionReason::EVConnected) {
                // Free service, authorize immediately
                types::authorization::ProvidedIdToken provided_token;
                provided_token.authorization_type = types::authorization::AuthorizationType::RFID;
                provided_token.id_token = "FREESERVICE";
                provided_token.prevalidated = true;
                mod->charger->Authorize(true, provided_token);
                mod->charger_was_authorized();
            }

            session_started.reason = reason;

            set_session_uuid();

            session_started.logging_path = session_log.startSession(
                mod->config.logfile_suffix == "session_uuid" ? session_uuid : mod->config.logfile_suffix);

            session_log.evse(
                false, fmt::format("Session Started: {}", types::evse_manager::start_session_reason_to_string(reason)));

            mod->telemetry.publish("session", "events",
                                   {
                                       {"timestamp", Everest::Date::to_rfc3339(date::utc_clock::now())},
                                       {"type", "session_started"},
                                       {"session_id", session_uuid},
                                       {"reason", types::evse_manager::start_session_reason_to_string(reason)},
                                   });

            se.session_started = session_started;
        } else if (e == types::evse_manager::SessionEventEnum::SessionFinished) {
            session_log.evse(false, fmt::format("Session Finished"));
            session_log.stopSession();
            mod->telemetry.publish("session", "events",
                                   {{"timestamp", Everest::Date::to_rfc3339(date::utc_clock::now())},
                                    {"type", "session_finished"},
                                    {"session_id", session_uuid}});
        } else if (e == types::evse_manager::SessionEventEnum::TransactionStarted) {
            types::evse_manager::TransactionStarted transaction_started;
            transaction_started.timestamp =
                date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(date::utc_clock::now()));

            transaction_started.meter_value = mod->get_latest_powermeter_data_billing();
            if (mod->is_reserved()) {
                transaction_started.reservation_id.emplace(mod->get_reservation_id());
                mod->cancel_reservation(false);
            }

            transaction_started.id_tag = mod->charger->getIdToken();

            double energy_import = transaction_started.meter_value.energy_Wh_import.total;

            session_log.evse(false, fmt::format("Transaction Started ({} kWh)", energy_import / 1000.));

            Everest::TelemetryMap telemetry_data = {
                {"timestamp", Everest::Date::to_rfc3339(date::utc_clock::now())},
                {"type", "transaction_started"},
                {"session_id", session_uuid},
                {"energy_counter_import_wh", transaction_started.meter_value.energy_Wh_import.total},
                {"id_tag", transaction_started.id_tag.id_token}};

            if (transaction_started.meter_value.energy_Wh_export.has_value()) {
                telemetry_data["energy_counter_export_wh"] =
                    transaction_started.meter_value.energy_Wh_export.value().total;
            }
            mod->telemetry.publish("session", "events", telemetry_data);

            se.transaction_started.emplace(transaction_started);
        } else if (e == types::evse_manager::SessionEventEnum::TransactionFinished) {
            this->mod->selected_protocol = "Unknown";
            types::evse_manager::TransactionFinished transaction_finished;

            transaction_finished.timestamp =
                date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(date::utc_clock::now()));

            transaction_finished.meter_value = mod->get_latest_powermeter_data_billing();

            auto reason = mod->charger->getTransactionFinishedReason();
            const auto id_tag = mod->charger->getStopTransactionIdTag();

            transaction_finished.reason.emplace(reason);
            if (!id_tag.empty()) {
                transaction_finished.id_tag.emplace(id_tag);
            }

            double energy_import = transaction_finished.meter_value.energy_Wh_import.total;

            session_log.evse(false, fmt::format("Transaction Finished: {} ({} kWh)",
                                                types::evse_manager::stop_transaction_reason_to_string(reason),
                                                energy_import / 1000.));

            Everest::TelemetryMap telemetry_data = {
                {"timestamp", Everest::Date::to_rfc3339(date::utc_clock::now())},
                {"type", "transaction_finished"},
                {"session_id", session_uuid},
                {"energy_counter_import_wh", energy_import},
                {"reason", types::evse_manager::stop_transaction_reason_to_string(reason)}};

            if (transaction_finished.meter_value.energy_Wh_export.has_value()) {
                telemetry_data["energy_counter_export_wh"] =
                    transaction_finished.meter_value.energy_Wh_export.value().total;
            }

            mod->telemetry.publish("session", "events", telemetry_data);

            se.transaction_finished.emplace(transaction_finished);
        } else if (e == types::evse_manager::SessionEventEnum::Error) {
            types::evse_manager::Error error;
            error.error_code = mod->charger->getErrorState();
            se.error = error;
        } else if (e == types::evse_manager::SessionEventEnum::Enabled or
                   e == types::evse_manager::SessionEventEnum::Disabled) {
            if (connector_status_changed) {
                se.connector_id = 1;
            }
        }

        se.uuid = session_uuid;

        publish_session_event(se);
        if (e == types::evse_manager::SessionEventEnum::SessionFinished) {
            session_uuid = "";
            this->mod->selected_protocol = "Unknown";
        }

        publish_selected_protocol(this->mod->selected_protocol);
    });

    // Note: Deprecated. Only kept for Node red compatibility, will be removed in the future
    // Legacy external mqtt pubs
    mod->charger->signalMaxCurrent.connect([this](float c) {
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/max_current", mod->config.connector_id), c);

        limits.uuid = mod->info.id;
        limits.max_current = c;
        publish_limits(limits);
    });

    mod->charger->signalState.connect([this](Charger::EvseState s) {
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/state_string", mod->config.connector_id),
                          mod->charger->evseStateToString(s));
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/state", mod->config.connector_id),
                          static_cast<int>(s));
    });

    mod->charger->signalError.connect([this](types::evse_manager::ErrorEnum s) {
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/error_type", mod->config.connector_id),
                          static_cast<int>(s));
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/error_string", mod->config.connector_id),
                          types::evse_manager::error_enum_to_string(s));
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

        this->mod->charger->Authorize(true, provided_token);
        mod->charger_was_authorized();
    }

    if (pnc) {
        this->mod->r_hlc[0]->call_authorization_response(
            validation_result.authorization_status,
            validation_result.certificate_status.value_or(types::authorization::CertificateStatus::Accepted));
    }
};

void evse_managerImpl::handle_withdraw_authorization() {
    this->mod->charger->DeAuthorize();
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
    return mod->charger->pauseCharging();
};

bool evse_managerImpl::handle_resume_charging() {
    return mod->charger->resumeCharging();
};

bool evse_managerImpl::handle_stop_transaction(types::evse_manager::StopTransactionRequest& request) {
    return mod->charger->cancelTransaction(request);
};

bool evse_managerImpl::handle_force_unlock(int& connector_id) {
    return mod->charger->forceUnlock();
};

std::string evse_managerImpl::generate_session_uuid() {
    return boost::uuids::to_string(boost::uuids::random_generator()());
}

void evse_managerImpl::handle_set_external_limits(types::energy::ExternalLimits& value) {
    mod->updateLocalEnergyLimit(value);
}

types::evse_manager::SwitchThreePhasesWhileChargingResult
evse_managerImpl::handle_switch_three_phases_while_charging(bool& three_phases) {
    // FIXME implement more sophisticated error code return once feature is really implemented
    if (mod->charger->switchThreePhasesWhileCharging(three_phases)) {
        return types::evse_manager::SwitchThreePhasesWhileChargingResult::Success;
    } else {
        return types::evse_manager::SwitchThreePhasesWhileChargingResult::Error_NotSupported;
    }
};

void evse_managerImpl::handle_set_get_certificate_response(
    types::iso15118_charger::Response_Exi_Stream_Status& certificate_reponse) {
    mod->r_hlc[0]->call_certificate_response(certificate_reponse);
}

} // namespace evse
} // namespace module
