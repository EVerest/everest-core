// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
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

    // Note: Deprecated. Only kept for Node red compatibility, will be removed in the future
    // Legacy external mqtt pubs
    mod->mqtt.subscribe("/external/cmd/set_max_current", [&charger = mod->charger, this](std::string data) {
        mod->updateLocalMaxCurrentLimit(std::stof(data));
    });

    mod->mqtt.subscribe(
        "/external/" + mod->info.id + ":" + mod->info.name + "/cmd/set_max_current",
        [&charger = mod->charger, this](std::string data) { mod->updateLocalMaxCurrentLimit(std::stof(data)); });

    mod->mqtt.subscribe("/external/cmd/set_auth",
                        [&charger = mod->charger](std::string data) { charger->Authorize(true, data.c_str(), false); });

    mod->mqtt.subscribe("/external/cmd/enable",
                        [&charger = mod->charger](const std::string& data) { charger->enable(); });

    mod->mqtt.subscribe("/external/cmd/disable",
                        [&charger = mod->charger](const std::string& data) { charger->disable(); });

    mod->mqtt.subscribe("/external/cmd/faulted",
                        [&charger = mod->charger](const std::string& data) { charger->set_faulted(); });

    mod->mqtt.subscribe("/external/cmd/switch_three_phases_while_charging",
                        [&charger = mod->charger](const std::string& data) {
                            charger->switchThreePhasesWhileCharging(str_to_bool(data));
                        });

    mod->mqtt.subscribe("/external/cmd/pause_charging",
                        [&charger = mod->charger](const std::string& data) { charger->pauseCharging(); });

    mod->mqtt.subscribe("/external/cmd/resume_charging",
                        [&charger = mod->charger](const std::string& data) { charger->resumeCharging(); });

    mod->mqtt.subscribe("/external/cmd/restart",
                        [&charger = mod->charger](const std::string& data) { charger->restart(); });
    // /Deprecated

    mod->r_powermeter->subscribe_powermeter([this](const json p) {
        // Republish data on proxy powermeter struct
        publish_powermeter(p);
    });
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

    mod->r_bsp->subscribe_telemetry(
        [this](types::board_support::Telemetry telemetry) { publish_telemetry(telemetry); });

    // The module code generates the reservation events and we merely publish them here
    mod->signalReservationEvent.connect([this](json j) {
        if (j["event"] == "ReservationStart") {
            set_session_uuid();
        }

        j["uuid"] = session_uuid;
        publish_session_events(j);
    });

    mod->charger->signalEvent.connect([this](const Charger::EvseEvent& e) {
        types::evse_manager::SessionEvents se;

        se.event = types::evse_manager::string_to_session_event(mod->charger->evseEventToString(e));

        if (e == Charger::EvseEvent::SessionStarted) {
            types::evse_manager::SessionStarted session_started;
            session_started.timestamp = std::chrono::seconds(std::time(NULL)).count();
            json p = mod->get_latest_powermeter_data();
            if (p.contains("energy_Wh_import") && p["energy_Wh_import"].contains("total")) {
                session_started.energy_Wh_import = p["energy_Wh_import"]["total"];
            } else {
                session_started.energy_Wh_import = 0;
            }

            if (p.contains("energy_Wh_export") && p["energy_Wh_export"].contains("total")) {
                session_started.energy_Wh_export.emplace(p["energy_Wh_export"]["total"]);
            }

            if (mod->reservation_valid()) {
                session_started.reservation_id.emplace(mod->get_reservation_id());
            }

            set_session_uuid();

            session_log.startSession(session_uuid);
            session_log.car(false, "Car plugged In");

            double energy_import = session_started.energy_Wh_import;

            session_log.evse(false, fmt::format("Session Started {} ({} kWh)", session_uuid, energy_import / 1000.));

            se.session_started.emplace(session_started);
        }

        se.uuid = session_uuid;

        if (e == Charger::EvseEvent::SessionFinished) {
            types::evse_manager::SessionFinished session_finished;

            session_finished.timestamp = std::chrono::seconds(std::time(NULL)).count();
            json p = mod->get_latest_powermeter_data();
            if (p.contains("energy_Wh_import") && p["energy_Wh_import"].contains("total")) {
                session_finished.energy_Wh_import = p["energy_Wh_import"]["total"];
            } else {
                session_finished.energy_Wh_import = 0;
            }

            if (p.contains("energy_Wh_export") && p["energy_Wh_export"].contains("total")) {
                session_finished.energy_Wh_export.emplace(p["energy_Wh_export"]["total"]);
            }

            double energy_import = session_finished.energy_Wh_import;

            session_log.evse(false, fmt::format("Session Finished ({} kWh)", energy_import / 1000.));

            session_uuid = "";
            session_log.stopSession();

            se.session_finished.emplace(session_finished);
        }

        if (e == Charger::EvseEvent::SessionCancelled) {
            types::evse_manager::SessionCancelled session_cancelled;

            session_cancelled.timestamp = std::chrono::seconds(std::time(NULL)).count();
            json p = mod->get_latest_powermeter_data();
            if (p.contains("energy_Wh_import") && p["energy_Wh_import"].contains("total")) {
                session_cancelled.energy_Wh_import = p["energy_Wh_import"]["total"];
            } else {
                session_cancelled.energy_Wh_import = 0;
            }

            if (p.contains("energy_Wh_export") && p["energy_Wh_export"].contains("total")) {
                session_cancelled.energy_Wh_export.emplace(p["energy_Wh_export"]["total"]);
            }

            {
                std::lock_guard<std::mutex> lock(session_mutex);
                session_cancelled.reason.emplace(cancel_session_reason);
            }

            double energy_import = session_cancelled.energy_Wh_import;

            session_log.evse(false, fmt::format("Session Cancelled ({} kWh)", energy_import / 1000.));

            se.session_cancelled.emplace(session_cancelled);
        }

        if (e == Charger::EvseEvent::Error) {
            se.error.emplace(
                types::evse_manager::string_to_error(mod->charger->errorStateToString(mod->charger->getErrorState())));
        }

        publish_session_events(se);
    });

    // Note: Deprecated. Only kept for Node red compatibility, will be removed in the future
    // Legacy external mqtt pubs
    mod->charger->signalMaxCurrent.connect([this](float c) {
        mod->mqtt.publish("/external/state/max_current", c);

        mod->mqtt.publish("/external/" + mod->info.id + ":" + mod->info.name + "/state/max_current", c);

        limits["uuid"] = mod->info.id;
        limits["max_current"] = c;
        publish_limits(limits);
    });

    mod->charger->signalState.connect([this](Charger::EvseState s) {
        mod->mqtt.publish("/external/state/state_string", mod->charger->evseStateToString(s));
        mod->mqtt.publish("/external/state/state", static_cast<int>(s));

        mod->mqtt.publish("/external/" + mod->info.id + ":" + mod->info.name + "/state/state_string",
                          mod->charger->evseStateToString(s));
        mod->mqtt.publish("/external/" + mod->info.id + ":" + mod->info.name + "/state/state", static_cast<int>(s));
    });

    mod->charger->signalError.connect([this](Charger::ErrorState s) {
        mod->mqtt.publish("/external/" + mod->info.id + ":" + mod->info.name + "/state/error_type",
                          static_cast<int>(s));
        mod->mqtt.publish("/external/" + mod->info.id + ":" + mod->info.name + "/state/error_string",
                          mod->charger->errorStateToString(s));

        mod->mqtt.publish("/external/state/error_type", static_cast<int>(s));
        mod->mqtt.publish("/external/state/error_string", mod->charger->errorStateToString(s));
    });
    // /Deprecated
}

bool evse_managerImpl::handle_enable() {
    return mod->charger->enable();
};

bool evse_managerImpl::handle_disable() {
    return mod->charger->disable();
};

bool evse_managerImpl::handle_set_faulted() {
    return mod->charger->set_faulted();
};

bool evse_managerImpl::handle_pause_charging() {
    return mod->charger->pauseCharging();
};

bool evse_managerImpl::handle_resume_charging() {
    return mod->charger->resumeCharging();
};

bool evse_managerImpl::handle_cancel_charging(types::evse_manager::SessionCancellationReason& reason) {
    {
        std::lock_guard<std::mutex> lock(session_mutex);
        cancel_session_reason = reason;
    }
    if (mod->get_hlc_enabled()) {
        mod->r_hlc[0]->call_stop_charging(true);
    }
    return mod->charger->cancelCharging();
};

bool evse_managerImpl::handle_accept_new_session() {
    return mod->charger->restart();
};

bool evse_managerImpl::handle_force_unlock() {
    return mod->charger->forceUnlock();
};

types::evse_manager::ReservationResult evse_managerImpl::handle_reserve_now(int& reservation_id,
                                                                            std::string& auth_token,
                                                                            std::string& expiry_date,
                                                                            std::string& parent_id) {
    return mod->reserve_now(reservation_id, auth_token, Everest::Date::from_rfc3339(expiry_date), parent_id);
};

bool evse_managerImpl::handle_cancel_reservation() {
    return mod->cancel_reservation();
};

std::string evse_managerImpl::generate_session_uuid() {
    return boost::uuids::to_string(boost::uuids::random_generator()());
}

types::evse_manager::SetLocalMaxCurrentResult evse_managerImpl::handle_set_local_max_current(double& max_current) {
    // FIXME this is the same as external mqtt current limit. This needs to set a local limit instead which is
    // advertised in schedule
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
    return mod->r_powermeter->call_get_signed_meter_value("FIXME");
}

} // namespace evse
} // namespace module
