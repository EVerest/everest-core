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
    limits["nr_of_phases_available"] = 1;
    limits["max_current"] = 0.;

    // Interface to Node-RED debug UI

    mod->mqtt.subscribe(
        fmt::format("everest_external/nodered/{}/cmd/set_max_current", mod->config.connector_id),
        [&charger = mod->charger, this](std::string data) { mod->updateLocalMaxCurrentLimit(std::stof(data)); });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/enable", mod->config.connector_id),
                        [&charger = mod->charger](const std::string& data) { charger->enable(); });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/disable", mod->config.connector_id),
                        [&charger = mod->charger](const std::string& data) { charger->disable(); });

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

    // /Interface to Node-RED debug UI

    if (mod->r_powermeter.size() > 0) {
        mod->r_powermeter[0]->subscribe_powermeter([this](const json p) {
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
            limits["nr_of_phases_available"] = n;
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
    mod->signalReservationEvent.connect([this](json j) {
        if (j["event"] == "ReservationStart") {
            set_session_uuid();
        }

        j["uuid"] = session_uuid;
        publish_session_event(j);
    });

    mod->charger->signalEvent.connect([this](const types::evse_manager::SessionEventEnum& e) {
        types::evse_manager::SessionEvent se;

        se.event = e;

        if (e == types::evse_manager::SessionEventEnum::SessionStarted) {
            types::evse_manager::SessionStarted session_started;

            session_started.timestamp =
                date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(date::utc_clock::now()));

            auto reason = mod->charger->getSessionStartedReason();

            session_started.reason = reason;

            set_session_uuid();

            session_log.startSession(session_uuid);

            session_log.evse(
                false, fmt::format("Session Started: {}", types::evse_manager::start_session_reason_to_string(reason)));

            se.session_started = session_started;
        }

        if (e == types::evse_manager::SessionEventEnum::SessionFinished) {
            session_log.evse(false, fmt::format("Session Finished"));
        }

        if (e == types::evse_manager::SessionEventEnum::TransactionStarted) {
            types::evse_manager::TransactionStarted transaction_started;
            transaction_started.timestamp =
                date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(date::utc_clock::now()));
            json p = mod->get_latest_powermeter_data();
            if (p.contains("energy_Wh_import") && p["energy_Wh_import"].contains("total")) {
                transaction_started.energy_Wh_import = p["energy_Wh_import"]["total"];
            } else {
                transaction_started.energy_Wh_import = 0;
            }

            if (p.contains("energy_Wh_export") && p["energy_Wh_export"].contains("total")) {
                transaction_started.energy_Wh_export.emplace(p["energy_Wh_export"]["total"]);
            }

            if (mod->is_reserved()) {
                transaction_started.reservation_id.emplace(mod->get_reservation_id());
            }

            transaction_started.id_tag = mod->charger->getAuthTag();

            double energy_import = transaction_started.energy_Wh_import;

            session_log.evse(false, fmt::format("Transaction Started ({} kWh)", energy_import / 1000.));

            se.transaction_started.emplace(transaction_started);
        }

        se.uuid = session_uuid;

        if (e == types::evse_manager::SessionEventEnum::TransactionFinished) {
            types::evse_manager::TransactionFinished transaction_finished;

            transaction_finished.timestamp =
                date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(date::utc_clock::now()));
            json p = mod->get_latest_powermeter_data();
            if (p.contains("energy_Wh_import") && p["energy_Wh_import"].contains("total")) {
                transaction_finished.energy_Wh_import = p["energy_Wh_import"]["total"];
            } else {
                transaction_finished.energy_Wh_import = 0;
            }

            if (p.contains("energy_Wh_export") && p["energy_Wh_export"].contains("total")) {
                transaction_finished.energy_Wh_export.emplace(p["energy_Wh_export"]["total"]);
            }

            auto reason = mod->charger->getTransactionFinishedReason();
            const auto id_tag = mod->charger->getStopTransactionIdTag();

            transaction_finished.reason.emplace(reason);
            if (!id_tag.empty()) {
                transaction_finished.id_tag.emplace(id_tag);
            }

            double energy_import = transaction_finished.energy_Wh_import;

            session_log.evse(false, fmt::format("Transaction Finished: {} ({} kWh)",
                                                types::evse_manager::stop_transaction_reason_to_string(reason),
                                                energy_import / 1000.));

            session_uuid = "";
            session_log.stopSession();

            se.transaction_finished.emplace(transaction_finished);
        }

        if (e == types::evse_manager::SessionEventEnum::Error) {
            se.error = mod->charger->getErrorState();
        }

        publish_session_event(se);
    });

    // Note: Deprecated. Only kept for Node red compatibility, will be removed in the future
    // Legacy external mqtt pubs
    mod->charger->signalMaxCurrent.connect([this](float c) {
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/max_current", mod->config.connector_id), c);

        limits["uuid"] = mod->info.id;
        limits["max_current"] = c;
        publish_limits(limits);
    });

    mod->charger->signalState.connect([this](Charger::EvseState s) {
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/state_string", mod->config.connector_id),
                          mod->charger->evseStateToString(s));
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/state", mod->config.connector_id),
                          static_cast<int>(s));
    });

    mod->charger->signalError.connect([this](types::evse_manager::Error s) {
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/error_type", mod->config.connector_id),
                          static_cast<int>(s));
        mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/error_string", mod->config.connector_id),
                          types::evse_manager::error_to_string(s));
    });
    // /Deprecated
}

int evse_managerImpl::handle_get_id() {
    return this->mod->config.connector_id;
};

bool evse_managerImpl::handle_enable() {
    return mod->charger->enable();
};

void evse_managerImpl::handle_authorize(std::string& id_tag, bool& pnc) {
    this->mod->charger->Authorize(true, id_tag, pnc);
    // maybe we need to inform HLC layer as well in case it is waiting for auth
    mod->charger_was_authorized();
};

void evse_managerImpl::handle_withdraw_authorization() {
    this->mod->charger->Authorize(false, "", false);
};

bool evse_managerImpl::handle_reserve(int& reservation_id) {
    return mod->reserve(reservation_id);
};

void evse_managerImpl::handle_cancel_reservation() {
    mod->cancel_reservation();
};

bool evse_managerImpl::handle_disable() {
    return mod->charger->disable();
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

    if (mod->get_hlc_enabled()) {
        mod->r_hlc[0]->call_stop_charging(true);
    }
    return mod->charger->cancelTransaction(request);
};

bool evse_managerImpl::handle_force_unlock() {
    return mod->charger->forceUnlock();
};

std::string evse_managerImpl::generate_session_uuid() {
    return boost::uuids::to_string(boost::uuids::random_generator()());
}

types::evse_manager::SetLocalMaxCurrentResult evse_managerImpl::handle_set_local_max_current(double& max_current) {
    if (mod->updateLocalMaxCurrentLimit(static_cast<float>(max_current))) {
        return types::evse_manager::SetLocalMaxCurrentResult::Success;
    } else {
        return types::evse_manager::SetLocalMaxCurrentResult::Error_OutOfRange;
    }
};

types::evse_manager::SwitchThreePhasesWhileChargingResult
evse_managerImpl::handle_switch_three_phases_while_charging(bool& three_phases) {
    // FIXME implement more sophisticated error code return once feature is really implemented
    if (mod->charger->switchThreePhasesWhileCharging(three_phases)) {
        return types::evse_manager::SwitchThreePhasesWhileChargingResult::Success;
    } else {
        return types::evse_manager::SwitchThreePhasesWhileChargingResult::Error_NotSupported;
    }
};

std::string evse_managerImpl::handle_get_signed_meter_value() {
    if (mod->r_powermeter.size() > 0) {
        return mod->r_powermeter[0]->call_get_signed_meter_value("FIXME");
    } else {
        return "NOT_AVAILABLE";
    }
}

} // namespace evse
} // namespace module
