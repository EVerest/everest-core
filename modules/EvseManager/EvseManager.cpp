// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "EvseManager.hpp"

namespace module {

void EvseManager::init() {
    local_three_phases = config.three_phases;
    invoke_init(*p_evse);
    invoke_init(*p_energy_grid);
    authorization_available = false;
    // check if a slac module is connected to the optional requirement
    slac_enabled = !r_slac.empty();
    // if hlc is disabled in config, disable slac even if requirement is connected
    if (!config.ac_hlc_enabled)
        slac_enabled = false;
    hlc_enabled = !r_hlc.empty();
    reserved = false;
    reservation_id = 0;

    reservationThreadHandle = std::thread([this]() {
        while (true) {
            // check reservation status on regular intervals to see if it expires.
            // This will generate the ReservationEnd event.
            reservation_valid();
            sleep(1);
        }
    });
}

void EvseManager::ready() {
    if (hlc_enabled) {
        r_hlc[0]->call_set_evseid(config.evse_id);
        r_hlc[0]->call_set_evse_notification("None", 10);
        r_hlc[0]->call_set_receipt_required(false);
        r_hlc[0]->call_set_nominal_voltage(230);
        r_hlc[0]->call_set_rcd(false);
        r_hlc[0]->call_set_max_current(0);
    }

    hw_capabilities = r_bsp->call_get_hw_capabilities();

    // Maybe override with user setting for this EVSE
    if (config.max_current < hw_capabilities.at("max_current_A")) {
        hw_capabilities.at("max_current_A") = config.max_current;
    }

    local_max_current_limit = hw_capabilities.at("max_current_A");

    // Maybe limit to single phase by user setting if possible with HW
    if (!config.three_phases && hw_capabilities.at("min_phase_count") == 1) {
        hw_capabilities.at("max_phase_count") = 1;
        local_three_phases = false;
    } else if (hw_capabilities.at("max_phase_count") == 3) {
        local_three_phases = true; // other configonfigurations currently not supported by HW
    }

    charger = std::unique_ptr<Charger>(new Charger(r_bsp));

    r_bsp->subscribe_event([this](std::string event) {
        charger->processEvent(event);

        // Forward events from BSP to SLAC module
        if (slac_enabled) {
            if (event == "EnterBCD")
                r_slac[0]->call_enter_bcd();
            else if (event == "LeaveBCD")
                r_slac[0]->call_leave_bcd();
            else if (event == "CarPluggedIn")
                r_slac[0]->call_reset(true);
            else if (event == "CarUnplugged")
                r_slac[0]->call_reset(false);
        }
    });

    r_bsp->subscribe_nr_of_phases_available([this](int n) { signalNrOfPhasesAvailable(n); });

    r_powermeter->subscribe_powermeter([this](json p) {
        // Inform charger about current charging current. This is used for slow OC detection.
        charger->setCurrentDrawnByVehicle(p["current_A"]["L1"], p["current_A"]["L2"], p["current_A"]["L3"]);

        // Store local cache
        latest_powermeter_data = p;
    });

    r_auth->subscribe_authorization_available([this](bool a) {
        // Listen to authorize events and cache locally.
        authorization_available = a;
        bool res_valid = reservation_valid();
        std::lock_guard<std::mutex> lock(reservation_mutex);
        if (res_valid) {
            EVLOG(info) << "Reservation ends because the id tag was presented";
            reserved = false;

            // publish event to other modules
            json se;
            se["event"] = "ReservationEnd";
            se["reservation_end"]["reason"] = "UsedToStartCharging";
            se["reservation_end"]["reservation_id"] = reservation_id;

            signalReservationEvent(se);
        }
        reserved = false;
    });

    if (slac_enabled)
        r_slac[0]->subscribe_state([this](const std::string& s) {
            // Notify charger whether matching was started (or is done) or not
            if (s == "UNMATCHED")
                charger->setMatchingStarted(false);
            else
                charger->setMatchingStarted(true);
        });

    charger->signalAuthRequired.connect([this]() {
        // The charger indicates it requires auth now. It will retry if we cannot give auth right now.
        if (authorization_available) {
            boost::variant<boost::blank, std::string> auth = r_auth->call_get_authorization();
            if (auth.which() == 1) {
                // FIXME: This is currently only for EIM!
                charger->Authorize(true, boost::get<std::string>(auth), false);
                const std::string& token = boost::get<std::string>(auth);
                // we got an auth token, check if it matches our reservation
                if (reserved_for_different_token(token)) {
                    // throw an error event that can e.g. be displayed
                    json se;
                    se["event"] = "ReservationAuthtokenMismatch";
                    signalReservationEvent(se);
                } else {
                    charger->Authorize(true, token, false);
                }
            }
        }
    });

    charger->signalMaxCurrent.connect([this](float ampere) {
        // The charger changed the max current setting. Forward to HLC
        if (hlc_enabled) {
            r_hlc[0]->call_set_max_current(ampere);
        }
    });

    if (slac_enabled)
        r_slac[0]->subscribe_request_error_routine([this]() {
            printf("Received request error routine in evsemanager\n");
            charger->requestErrorSequence();
        });

    // Cancel reservations if charger is disabled or faulted
    charger->signalEvent.connect([this](Charger::EvseEvent s) {
        if (s == Charger::EvseEvent::Disabled || s == Charger::EvseEvent::PermanentFault) {
            cancel_reservation();
        }
    });

    invoke_ready(*p_evse);
    invoke_ready(*p_energy_grid);

    charger->setup(local_three_phases, config.has_ventilation, config.country_code, config.rcd_enabled,
                   config.charge_mode, slac_enabled, config.ac_hlc_use_5percent, config.ac_enforce_hlc);
    //  start with a limit of 0 amps. We will get a budget from EnergyManager that is locally limited by hw caps.
    charger->setMaxCurrent(0.0F, date::utc_clock::now());
    charger->run();
    charger->enable();
}

json EvseManager::get_latest_powermeter_data() {
    return latest_powermeter_data;
}

json EvseManager::get_hw_capabilities() {
    return hw_capabilities;
}

int32_t EvseManager::get_reservation_id() {
    return reservation_id;
}

std::string EvseManager::reserve_now(const int _reservation_id, const std::string& token,
                                     const std::chrono::time_point<date::utc_clock>& valid_until,
                                     const std::string& parent_id) {

    // is the evse Unavailable?
    if (charger->getCurrentState() == Charger::EvseState::Disabled) {
        return "Unavailable";
    }

    // is the evse faulted?
    if (charger->getCurrentState() == Charger::EvseState::Faulted) {
        return "Faulted";
    }

    // is the reservation still valid in time?
    if (date::utc_clock::now() > valid_until) {
        return "Rejected";
    }

    // is the connector currently ready to accept a new car?
    if (charger->getCurrentState() != Charger::EvseState::Idle) {
        return "Occupied";
    }

    // is it already reserved
    if (reservation_valid()) {
        return "Occupied";
    }

    std::lock_guard<std::mutex> lock(reservation_mutex);

    // accept new reservation
    reservation_id = _reservation_id;
    reserved_auth_token = token;
    reservation_valid_until = valid_until;
    reserved_auth_token_parent_id = parent_id;
    reserved = true;

    // publish event to other modules
    json se;
    se["event"] = "ReservationStart";
    se["reservation_start"]["reservation_id"] = reservation_id;
    se["reservation_start"]["id_tag"] = reserved_auth_token;
    se["reservation_start"]["parent_id"] = parent_id;

    signalReservationEvent(se);

    return "Accepted";

    // FIXME TODO:
    /*
    parent_id needs to be implemented in new auth manager arch

    reservationEnd with reason Charging Started with it is not implemented yet
    */
}

bool EvseManager::updateLocalMaxCurrentLimit(float max_current) {
    if (max_current >= 0.0F && max_current < EVSE_ABSOLUTE_MAX_CURRENT) {
        local_max_current_limit = max_current;

        // wait for EnergyManager to assign optimized current on next opimizer run

        return true;
    }
    return false;
}

bool EvseManager::cancel_reservation() {
    bool res_valid = reservation_valid();

    std::lock_guard<std::mutex> lock(reservation_mutex);
    if (res_valid) {
        reserved = false;

        // publish event to other modules
        json se;
        se["event"] = "ReservationEnd";
        se["reservation_end"]["reason"] = "Cancelled";
        se["reservation_end"]["reservation_id"] = reservation_id;

        signalReservationEvent(se);
        return true;
    }
    reserved = false;
    return false;
}

float EvseManager::getLocalMaxCurrentLimit() {
    return local_max_current_limit;
}

bool EvseManager::reservation_valid() {
    std::lock_guard<std::mutex> lock(reservation_mutex);
    if (reserved) {
        if (date::utc_clock::now() < reservation_valid_until) {
            // still valid
            return true;
        } else {
            // expired
            // publish event to other modules
            json se;
            se["event"] = "ReservationEnd";
            se["reservation_end"]["reason"] = "Expired";
            se["reservation_end"]["reservation_id"] = reservation_id;

            signalReservationEvent(se);
            reserved = false;
        }
    }
    // no active reservation
    return false;
}

/*
  Returns false if not reserved or reservation matches the token.
  Returns true if reserved for a different token.
*/
bool EvseManager::reserved_for_different_token(const std::string& token) {
    std::lock_guard<std::mutex> lock(reservation_mutex);
    if (!reserved) {
        return false;
    }

    if (reserved_auth_token == token) {
        return false;
    } else {
        return true;
    }
}

} // namespace module
