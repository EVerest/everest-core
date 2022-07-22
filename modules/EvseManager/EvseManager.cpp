// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "EvseManager.hpp"
#include <fmt/core.h>

namespace module {

void EvseManager::init() {
    local_three_phases = config.three_phases;

    session_log.setPath(config.session_logging_path);
    if (config.session_logging) {
        session_log.enable();
    }
    session_log.xmlOutput(config.session_logging_xml);

    invoke_init(*p_evse);
    invoke_init(*p_energy_grid);
    authorization_available = false;
    // check if a slac module is connected to the optional requirement
    slac_enabled = !r_slac.empty();

    // if hlc is disabled in config, disable slac even if requirement is connected
    if (!(config.ac_hlc_enabled || config.ac_with_soc || config.charge_mode == "DC")) {
        slac_enabled = false;
    }
    hlc_enabled = !r_hlc.empty();
    reserved = false;
    reservation_id = 0;

    hlc_waiting_for_auth_eim = false;
    hlc_waiting_for_auth_pnc = false;

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
    if (get_hlc_enabled()) {

        // Set up EVSE ID
        r_hlc[0]->call_set_EVSEID(config.evse_id);

        // Set up auth options for HLC
        Array payment_options;
        if (config.payment_enable_eim) {
            payment_options.insert(payment_options.end(), "ExternalPayment");
        }
        if (config.payment_enable_contract) {
            payment_options.insert(payment_options.end(), "Contract");
        }
        r_hlc[0]->call_set_PaymentOptions(payment_options);

        // Set up energy transfer modes for HLC. For now we only support either DC or AC, not both at the same time.
        Array transfer_modes;
        if (config.charge_mode == "AC") {
            r_hlc[0]->call_set_AC_EVSENominalVoltage(config.ac_nominal_voltage);

            if (config.three_phases) {
                transfer_modes.insert(transfer_modes.end(), "AC_three_phase_core");
                transfer_modes.insert(transfer_modes.end(), "AC_single_phase_core");
            } else {
                transfer_modes.insert(transfer_modes.end(), "AC_single_phase_core");
            }
        } else if (config.charge_mode == "DC") {
            transfer_modes.insert(transfer_modes.end(), "DC_core");
            transfer_modes.insert(transfer_modes.end(), "DC_extended");
            transfer_modes.insert(transfer_modes.end(), "DC_combo_core");
            transfer_modes.insert(transfer_modes.end(), "DC_unique");
            r_hlc[0]->call_set_DC_EVSECurrentRegulationTolerance(config.dc_current_regulation_tolerance);
            r_hlc[0]->call_set_DC_EVSEPeakCurrentRipple(config.dc_peak_current_ripple);
            r_hlc[0]->call_set_DC_EVSEPresentVoltage(400); // FIXME: set a correct values
            r_hlc[0]->call_set_DC_EVSEPresentCurrent(0);
            r_hlc[0]->call_set_DC_EVSEMaximumCurrentLimit(400);
            r_hlc[0]->call_set_DC_EVSEMaximumPowerLimit(200000);
            r_hlc[0]->call_set_DC_EVSEMaximumVoltageLimit(1000);
            r_hlc[0]->call_set_DC_EVSEMinimumCurrentLimit(0);
            r_hlc[0]->call_set_DC_EVSEMinimumVoltageLimit(0);

            if (!r_imd.empty()) {
                r_hlc[0]->subscribe_Require_EVSEIsolationCheck([this]() {
                    // start measurement with IMD
                    EVLOG_info << "Start isolation testing...";
                    r_imd[0]->call_startIsolationTest();
                });

                r_imd[0]->subscribe_IsolationStatus([this](std::string s) {
                    // Callback after isolation measurement finished
                    EVLOG_info << fmt::format("Isolation testing finsihed with status {}.", s);
                    r_hlc[0]->call_set_EVSEIsolationStatus(s);
                });
            }
            /*
                        r_hlc[0]->subscribe_DC_EVTargetCurrent([this](double amps) {
                            r_hlc[0]->call_set_DC_EVSEPresentCurrent(amps * 0.9); // FIXME
                        });
            */
            // later
            // set_EVSEIsolationStatus

        } else {
            EVLOG_error << "Unsupported charging mode.";
            exit(255);
        }
        r_hlc[0]->call_set_SupportedEnergyTransferMode(transfer_modes);

        r_hlc[0]->call_set_ReceiptRequired(config.ev_receipt_required);

        // We always go through the auth step with EIM even if it is free. This way EVerest auth manager has full
        // control.
        r_hlc[0]->call_set_FreeService(false);

        // set up debug mode for HLC
        if (config.session_logging) {
            r_hlc[0]->call_enable_debug_mode("Full");
        }

        // reset error flags
        r_hlc[0]->call_set_FAILED_ContactorError(false);
        r_hlc[0]->call_set_RCD_Error(false);

        // implement Auth handlers
        r_hlc[0]->subscribe_Require_Auth_EIM([this]() {
            // Do we have auth already (i.e. delayed HLC after charging already running)?
            if ((config.dbg_hlc_auth_after_tstep && charger->Authorized_EIM_ready_for_HLC()) ||
                (!config.dbg_hlc_auth_after_tstep && charger->Authorized_EIM())) {
                r_hlc[0]->call_set_Auth_Okay_EIM(true);
            }
        });

        // implement Auth handlers
        r_hlc[0]->subscribe_EVCCIDD([this](const std::string& token) {
            json autocharge_token;

            autocharge_token["token"] = "VID:" + token;
            autocharge_token["type"] = "autocharge";
            autocharge_token["timeout"] = 60;

            p_token_provider->publish_token(autocharge_token);
        });

        r_hlc[0]->subscribe_Require_Auth_PnC([this]() {
            // Do we have auth already (i.e. delayed HLC after charging already running)?
            if (charger->Authorized_PnC()) {
                r_hlc[0]->call_set_Auth_Okay_PnC(true);
            }
        });

        // Install debug V2G Messages handler if session logging is enabled
        if (config.session_logging) {
            r_hlc[0]->subscribe_V2G_Messages([this](Object v2g_message) { log_v2g_message(v2g_message); });
        }
        // switch to DC mode for first session for AC with SoC
        if (config.ac_with_soc) {

            r_bsp->subscribe_event([this](const std::string& event) {
                if (event == "CarUnplugged") {
                    // configure for DC again for next session. Will reset to AC when SoC is received
                    switch_DC_mode();
                }
            });

            charger->signalACWithSoCTimeout.connect([this]() { switch_DC_mode(); });

            r_hlc[0]->subscribe_DC_EVRESSSOC([this](double soc) {
                EVLOG_info << fmt::format("SoC received: {}.", soc);
                switch_AC_mode();
            });
        }

        // Do not implement for now:
        // set_EVSEEnergyToBeDelivered
        // Require_EVSEIsolationCheck ignored for now
        // AC_Close_Contactor/AC_Open_Contactor: we ignore these events, we still switch on/off with state C/D
        // for now. May not work if PWM switch on comes before request through HLC, so implement soon! All DC
        // stuff missing V2G_Setup_Finished is useful but ignored for now All other telemetry type info is
        // ignored for now
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

        // Forward some events to HLC
        if (get_hlc_enabled()) {
            // Reset HLC auth waiting flags on new session
            if (event == "CarPluggedIn") {
                r_hlc[0]->call_set_FAILED_ContactorError(false);
                r_hlc[0]->call_set_RCD_Error(false);
                r_hlc[0]->call_set_EVSE_Malfunction(false);
                r_hlc[0]->call_set_EVSE_EmergencyShutdown(false);
                r_hlc[0]->call_contactor_open(true);
                r_hlc[0]->call_stop_charging(false);
            }

            if (event == "ErrorRelais") {
                session_log.evse(false, "Error Relais");
                r_hlc[0]->call_set_FAILED_ContactorError(true);
            }

            if (event == "ErrorRCD") {
                session_log.evse(false, "Error RCD");
                r_hlc[0]->call_set_RCD_Error(true);
            }

            if (event == "PermanentFault") {
                session_log.evse(false, "Error Permanent Fault");
                r_hlc[0]->call_set_EVSE_Malfunction(true);
            }

            if (event == "ErrorOverCurrent") {
                session_log.evse(false, "Error Over Current");
                r_hlc[0]->call_set_EVSE_EmergencyShutdown(true);
            }

            if (event == "PowerOn") {
                r_hlc[0]->call_contactor_closed(true);
            }

            if (event == "PowerOff") {
                r_hlc[0]->call_contactor_open(true);
            }
        }

        // Forward events from BSP to SLAC module
        if (slac_enabled) {
            if (event == "EnterBCD") {
                r_slac[0]->call_enter_bcd();
            } else if (event == "LeaveBCD") {
                r_slac[0]->call_leave_bcd();
            } else if (event == "CarPluggedIn") {
                r_slac[0]->call_reset(true);
            } else if (event == "CarUnplugged") {
                r_slac[0]->call_reset(false);
            }
        }

        if (config.ac_with_soc)
            charger->signalACWithSoCTimeout.connect([this]() {
                EVLOG_info << "AC with SoC timeout";
                switch_DC_mode();
            });
    });

    r_bsp->subscribe_nr_of_phases_available([this](int n) { signalNrOfPhasesAvailable(n); });

    r_powermeter->subscribe_powermeter([this](json p) {
        // Inform charger about current charging current. This is used for slow OC detection.
        charger->setCurrentDrawnByVehicle(p["current_A"]["L1"], p["current_A"]["L2"], p["current_A"]["L3"]);

        // Inform HLC about the power meter data
        if (get_hlc_enabled()) {
            r_hlc[0]->call_set_MeterInfo(p);
        }

        // Store local cache
        latest_powermeter_data = p;
    });

    r_auth->subscribe_authorization_available([this](bool a) {
        // Listen to authorize events and cache locally.
        authorization_available = a;
        bool res_valid = reservation_valid();
        std::lock_guard<std::mutex> lock(reservation_mutex);
        if (res_valid) {
            EVLOG_info << "Reservation ends because the id tag was presented";
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

    if (slac_enabled) {
        r_slac[0]->subscribe_state([this](const std::string& s) {
            session_log.evse(true, fmt::format("SLAC {}", s));
            // Notify charger whether matching was started (or is done) or not
            if (s == "UNMATCHED") {
                charger->setMatchingStarted(false);
            } else {
                charger->setMatchingStarted(true);
            }
        });
    }

    charger->signalAuthRequired.connect([this]() {
        // The charger indicates it requires auth now. It will retry if we cannot give auth right now.
        if (authorization_available) {
            boost::variant<boost::blank, std::string> auth = r_auth->call_get_authorization();
            if (auth.which() == 1) {
                // FIXME: This is currently only for EIM!
                // charger->Authorize(true, boost::get<std::string>(auth), false);
                const std::string& token = boost::get<std::string>(auth);
                // we got an auth token, check if it matches our reservation
                if (reserved_for_different_token(token)) {
                    // throw an error event that can e.g. be displayed
                    json se;
                    se["event"] = "ReservationAuthtokenMismatch";
                    signalReservationEvent(se);
                    session_log.evse(false, fmt::format("Reservation does not match Auth token {}", token));
                } else {
                    // if reserved: signal to the outside world that this reservation ended because it is being
                    // used
                    use_reservation_to_start_charging();
                    // notify charger about the authorization
                    charger->Authorize(true, token, false);
                    // notify HLC if it was waiting for it as well FIXME: if we notify here, charger is actually not
                    // ready as it is going through t_step_X1 or so
                    /*if (get_hlc_waiting_for_auth_eim() && get_hlc_enabled()) {
                        r_hlc[0]->call_set_Auth_Okay_EIM(true);
                    }*/
                    //  FIXME: PnC not supported here!
                }
            }
        }
    });

    charger->signalMaxCurrent.connect([this](float ampere) {
        // The charger changed the max current setting. Forward to HLC
        if (get_hlc_enabled()) {
            r_hlc[0]->call_set_AC_EVSEMaxCurrent(ampere);
        }
    });

    if (slac_enabled) {
        r_slac[0]->subscribe_request_error_routine([this]() {
            EVLOG_info << "Received request error routine from SLAC in evsemanager\n";
            charger->requestErrorSequence();
        });
    }
    charger->signalEvent.connect([this](Charger::EvseEvent s) {
        // Cancel reservations if charger is disabled or faulted
        if (s == Charger::EvseEvent::Disabled || s == Charger::EvseEvent::PermanentFault) {
            cancel_reservation();
        }
    });

    invoke_ready(*p_evse);
    invoke_ready(*p_energy_grid);
    if (config.ac_with_soc) {
        setup_DC_mode();
    } else {
        charger->setup(local_three_phases, config.has_ventilation, config.country_code, config.rcd_enabled,
                       config.charge_mode, slac_enabled, config.ac_hlc_use_5percent, config.ac_enforce_hlc, false);
    }
    //  start with a limit of 0 amps. We will get a budget from EnergyManager that is locally limited by hw
    //  caps.
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

void EvseManager::switch_DC_mode() {
    charger->evseReplug();
    setup_DC_mode();
}

void EvseManager::switch_AC_mode() {
    charger->evseReplug();
    setup_AC_mode();
}

void EvseManager::setup_DC_mode() {
    charger->setup(local_three_phases, config.has_ventilation, config.country_code, config.rcd_enabled, "DC",
                   slac_enabled, config.ac_hlc_use_5percent, config.ac_enforce_hlc, false);

    // Set up energy transfer modes for HLC. For now we only support either DC or AC, not both at the same time.
    Array transfer_modes;
    transfer_modes.insert(transfer_modes.end(), "DC_core");
    transfer_modes.insert(transfer_modes.end(), "DC_extended");
    transfer_modes.insert(transfer_modes.end(), "DC_combo_core");
    transfer_modes.insert(transfer_modes.end(), "DC_unique");
    r_hlc[0]->call_set_DC_EVSECurrentRegulationTolerance(config.dc_current_regulation_tolerance);
    r_hlc[0]->call_set_DC_EVSEPeakCurrentRipple(config.dc_peak_current_ripple);
    r_hlc[0]->call_set_DC_EVSEPresentVoltage(400); // FIXME: set a correct values
    r_hlc[0]->call_set_DC_EVSEPresentCurrent(0);
    r_hlc[0]->call_set_DC_EVSEMaximumCurrentLimit(400);
    r_hlc[0]->call_set_DC_EVSEMaximumPowerLimit(200000);
    r_hlc[0]->call_set_DC_EVSEMaximumVoltageLimit(1000);
    r_hlc[0]->call_set_DC_EVSEMinimumCurrentLimit(0);
    r_hlc[0]->call_set_DC_EVSEMinimumVoltageLimit(0);

    r_hlc[0]->call_set_SupportedEnergyTransferMode(transfer_modes);
}

void EvseManager::setup_AC_mode() {
    charger->setup(local_three_phases, config.has_ventilation, config.country_code, config.rcd_enabled, "AC",
                   slac_enabled, config.ac_hlc_use_5percent, config.ac_enforce_hlc, true);

    // Set up energy transfer modes for HLC. For now we only support either DC or AC, not both at the same time.
    Array transfer_modes;
    if (config.three_phases) {
        transfer_modes.insert(transfer_modes.end(), "AC_three_phase_core");
        transfer_modes.insert(transfer_modes.end(), "AC_single_phase_core");
    } else {
        transfer_modes.insert(transfer_modes.end(), "AC_single_phase_core");
    }
    r_hlc[0]->call_set_SupportedEnergyTransferMode(transfer_modes);
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

// Signals that reservation was used to start this charging.
// Does nothing if no reservation is active.
void EvseManager::use_reservation_to_start_charging() {

    std::lock_guard<std::mutex> lock(reservation_mutex);
    if (!reserved) {
        return;
    }

    session_log.evse(false, fmt::format("Reservation token used to start charging"));
    // publish event to other modules
    json se;
    se["event"] = "ReservationEnd";
    se["reservation_end"]["reason"] = "UsedToStartCharging";
    se["reservation_end"]["reservation_id"] = reservation_id;

    signalReservationEvent(se);

    reserved = false;
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

bool EvseManager::get_hlc_enabled() {
    std::lock_guard<std::mutex> lock(hlc_mutex);
    return hlc_enabled;
}

void EvseManager::log_v2g_message(Object m) {
    std::string msg = m["V2G_Message_ID"];

    // All messages from EVSE contain Req and all originating from Car contain Res
    if (msg.find("Res") == std::string::npos) {
        session_log.car(true, fmt::format("V2G {}", msg), m["V2G_Message_XML"], m["V2G_Message_EXI_Hex"],
                        m["V2G_Message_EXI_Base64"]);
    } else {
        session_log.evse(true, fmt::format("V2G {}", msg), m["V2G_Message_XML"], m["V2G_Message_EXI_Hex"],
                         m["V2G_Message_EXI_Base64"]);
    }
}

} // namespace module
