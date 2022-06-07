// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "evse_managerImpl.hpp"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace module {
namespace evse {

std::chrono::time_point<date::utc_clock> from_rfc3339(std::string t) {
    std::istringstream infile{t};
    std::chrono::time_point<date::utc_clock> tp;
    infile >> date::parse("%FT%T", tp);
    return tp;
}

bool str_to_bool(std::string data) {
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

    mod->r_bsp->subscribe_telemetry([this](json telemetry) { publish_telemetry(telemetry); });

    // The module code generates the reservation events and we merely publish them here
    mod->signalReservationEvent.connect([this](json j) {
        if (j["event"] == "ReservationStart") {
            set_session_uuid();
        }

        j["uuid"] = session_uuid;
        publish_session_events(j);
    });

    mod->charger->signalEvent.connect([this](const Charger::EvseEvent& e) {
        json se;

        se["event"] = mod->charger->evseEventToString(e);

        if (e == Charger::EvseEvent::SessionStarted) {
            se["session_started"]["timestamp"] = std::chrono::seconds(std::time(NULL)).count();
            json p = mod->get_latest_powermeter_data();
            if (p.contains("energy_Wh_import") && p["energy_Wh_import"].contains("total"))
                se["session_started"]["energy_Wh_import"] = p["energy_Wh_import"]["total"];

            if (p.contains("energy_Wh_export") && p["energy_Wh_export"].contains("total"))
                se["session_started"]["energy_Wh_export"] = p["energy_Wh_export"]["total"];

            set_session_uuid();
        }

        se["uuid"] = session_uuid;

        if (e == Charger::EvseEvent::SessionFinished) {

            se["session_finished"]["timestamp"] = std::chrono::seconds(std::time(NULL)).count();
            json p = mod->get_latest_powermeter_data();
            if (p.contains("energy_Wh_import") && p["energy_Wh_import"].contains("total"))
                se["session_finished"]["energy_Wh_import"] = p["energy_Wh_import"]["total"];

            if (p.contains("energy_Wh_export") && p["energy_Wh_export"].contains("total"))
                se["session_finished"]["energy_Wh_export"] = p["energy_Wh_export"]["total"];

            session_uuid = "";
        }

        if (e == Charger::EvseEvent::SessionCancelled) {

            se["session_cancelled"]["timestamp"] = std::chrono::seconds(std::time(NULL)).count();
            json p = mod->get_latest_powermeter_data();
            if (p.contains("energy_Wh_import") && p["energy_Wh_import"].contains("total"))
                se["session_cancelled"]["energy_Wh_import"] = p["energy_Wh_import"]["total"];

            if (p.contains("energy_Wh_export") && p["energy_Wh_export"].contains("total"))
                se["session_cancelled"]["energy_Wh_export"] = p["energy_Wh_export"]["total"];

            {
                std::lock_guard<std::mutex> lock(session_mutex);
                se["session_cancelled"]["reason"] = cancel_session_reason;
            }
        }

        if (e == Charger::EvseEvent::Error) {
            se["error"] = mod->charger->errorStateToString(mod->charger->getErrorState());
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

bool evse_managerImpl::handle_cancel_charging(std::string& reason) {
    {
        std::lock_guard<std::mutex> lock(session_mutex);
        cancel_session_reason = reason;
    }
    return mod->charger->cancelCharging();
};

bool evse_managerImpl::handle_accept_new_session() {
    return mod->charger->restart();
};

bool evse_managerImpl::handle_force_unlock() {
    return mod->charger->forceUnlock();
};

std::string evse_managerImpl::handle_reserve_now(int& reservation_id, std::string& auth_token, std::string& expiry_date,
                                                 std::string& parent_id) {
    return mod->reserve_now(reservation_id, auth_token, from_rfc3339(expiry_date), parent_id);
};

bool evse_managerImpl::handle_cancel_reservation() {
    return mod->cancel_reservation();
};

std::string evse_managerImpl::generate_session_uuid() {
    return boost::uuids::to_string(boost::uuids::random_generator()());
}

std::string evse_managerImpl::handle_set_local_max_current(double& max_current) {
    // FIXME this is the same as external mqtt current limit. This needs to set a local limit instead which is
    // advertised in schedule
    if (mod->updateLocalMaxCurrentLimit(static_cast<float>(max_current)))
        return "Success";
    else
        return "Error_OutOfRange";
};

std::string evse_managerImpl::handle_switch_three_phases_while_charging(bool& three_phases) {
    // FIXME implement more sophisticated error code return once feature is really implemented
    if (mod->charger->switchThreePhasesWhileCharging(three_phases))
        return "Success";
    else
        return "Error_NotSupported";
};

std::string evse_managerImpl::handle_get_signed_meter_value() {
    return mod->r_powermeter->call_get_signed_meter_value("FIXME");
}

} // namespace evse
} // namespace module
