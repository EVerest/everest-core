// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
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
    invoke_init(*p_token_provider);
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
        r_hlc[0]->subscribe_EVCCIDD([this](const std::string& _token) {
            json autocharge_token;

            std::string token = _token;
            token.erase(remove(token.begin(), token.end(), ':'), token.end());
            autocharge_token["id_token"] = "VID:" + token;
            autocharge_token["type"] = "autocharge";
            autocharge_token["timeout"] = 60;

            p_token_provider->publish_provided_token(autocharge_token);
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

            r_bsp->subscribe_event([this](const types::board_support::Event& event) {
                if (event == types::board_support::Event::CarUnplugged) {
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
    if (config.max_current < hw_capabilities.max_current_A) {
        hw_capabilities.max_current_A = config.max_current;
    }

    local_max_current_limit = hw_capabilities.max_current_A;

    // Maybe limit to single phase by user setting if possible with HW
    if (!config.three_phases && hw_capabilities.min_phase_count == 1) {
        hw_capabilities.max_phase_count = 1;
        local_three_phases = false;
    } else if (hw_capabilities.max_phase_count == 3) {
        local_three_phases = true; // other configonfigurations currently not supported by HW
    }

    charger = std::unique_ptr<Charger>(new Charger(r_bsp));

    r_bsp->subscribe_event([this](const types::board_support::Event event) {
        charger->processEvent(event);

        // Forward some events to HLC
        if (get_hlc_enabled()) {
            // Reset HLC auth waiting flags on new session
            if (event == types::board_support::Event::CarPluggedIn) {
                r_hlc[0]->call_set_FAILED_ContactorError(false);
                r_hlc[0]->call_set_RCD_Error(false);
                r_hlc[0]->call_set_EVSE_Malfunction(false);
                r_hlc[0]->call_set_EVSE_EmergencyShutdown(false);
                r_hlc[0]->call_contactor_open(true);
                r_hlc[0]->call_stop_charging(false);
            }

            if (event == types::board_support::Event::ErrorRelais) {
                session_log.evse(false, "Error Relais");
                r_hlc[0]->call_set_FAILED_ContactorError(true);
            }

            if (event == types::board_support::Event::ErrorRCD) {
                session_log.evse(false, "Error RCD");
                r_hlc[0]->call_set_RCD_Error(true);
            }

            if (event == types::board_support::Event::PermanentFault) {
                session_log.evse(false, "Error Permanent Fault");
                r_hlc[0]->call_set_EVSE_Malfunction(true);
            }

            if (event == types::board_support::Event::ErrorOverCurrent) {
                session_log.evse(false, "Error Over Current");
                r_hlc[0]->call_set_EVSE_EmergencyShutdown(true);
            }

            if (event == types::board_support::Event::PowerOn) {
                r_hlc[0]->call_contactor_closed(true);
            }

            if (event == types::board_support::Event::PowerOff) {
                r_hlc[0]->call_contactor_open(true);
            }
        }

        // Forward events from BSP to SLAC module
        if (slac_enabled) {
            if (event == types::board_support::Event::EnterBCD) {
                r_slac[0]->call_enter_bcd();
            } else if (event == types::board_support::Event::LeaveBCD) {
                r_slac[0]->call_leave_bcd();
            } else if (event == types::board_support::Event::CarPluggedIn) {
                r_slac[0]->call_reset(true);
            } else if (event == types::board_support::Event::CarUnplugged) {
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

    r_powermeter->subscribe_powermeter([this](types::powermeter::Powermeter powermeter) {
        json p = powermeter;
        // Inform charger about current charging current. This is used for slow OC detection.
        charger->setCurrentDrawnByVehicle(p["current_A"]["L1"], p["current_A"]["L2"], p["current_A"]["L3"]);

        // Inform HLC about the power meter data
        if (get_hlc_enabled()) {
            r_hlc[0]->call_set_MeterInfo(p);
        }

        // Store local cache
        latest_powermeter_data = p;

        // External Nodered interface
        mqtt.publish(fmt::format("everest_external/nodered/{}/powermeter/phaseSeqError", config.connector_id),
                     powermeter.phase_seq_error.get());
        mqtt.publish(fmt::format("everest_external/nodered/{}/powermeter/time_stamp", config.connector_id),
                     (int)powermeter.timestamp);
        mqtt.publish(fmt::format("everest_external/nodered/{}/powermeter/totalKw", config.connector_id),
                     (powermeter.power_W.get().L1.get() + powermeter.power_W.get().L2.get() + powermeter.power_W.get().L3.get()) / 1000., 1);
        mqtt.publish(fmt::format("everest_external/nodered/{}/powermeter/totalKWattHr", config.connector_id),
                     powermeter.energy_Wh_import.total / 1000.);
        mqtt.publish(fmt::format("everest_external/nodered/{}/powermeter_json", config.connector_id),
                     p.dump());
        // /External Nodered interface
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
    invoke_ready(*p_token_provider);
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

types::board_support::HardwareCapabilities EvseManager::get_hw_capabilities() {
    return hw_capabilities;
}

int32_t EvseManager::get_reservation_id() {
    std::lock_guard<std::mutex> lock(reservation_mutex);
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

bool EvseManager::updateLocalMaxCurrentLimit(float max_current) {
    if (max_current >= 0.0F && max_current < EVSE_ABSOLUTE_MAX_CURRENT) {
        local_max_current_limit = max_current;

        // wait for EnergyManager to assign optimized current on next opimizer run

        return true;
    }
    return false;
}

bool EvseManager::reserve(int32_t id) {

    // is the evse Unavailable?
    if (charger->getCurrentState() == Charger::EvseState::Disabled) {
        return false;
    }

    // is the evse faulted?
    if (charger->getCurrentState() == Charger::EvseState::Faulted) {
        return false;
    }

    // is the connector currently ready to accept a new car?
    if (charger->getCurrentState() != Charger::EvseState::Idle) {
        return false;
    }

    std::lock_guard<std::mutex> lock(reservation_mutex);

    if (!reserved) {
        reserved = true;
        reservation_id = id;

        // publish event to other modules
        json se;
        se["event"] = "ReservationStart";

        signalReservationEvent(se);
        return true;
    }

    return false;
}

void EvseManager::cancel_reservation() {

    std::lock_guard<std::mutex> lock(reservation_mutex);
    if (reserved) {
        reserved = false;
        reservation_id = 0;

        // publish event to other modules
        json se;
        se["event"] = "ReservationEnd";

        signalReservationEvent(se);
    }
}

bool EvseManager::is_reserved() {
    std::lock_guard<std::mutex> lock(reservation_mutex);
    return reserved;
}

float EvseManager::getLocalMaxCurrentLimit() {
    return local_max_current_limit;
}

bool EvseManager::get_hlc_enabled() {
    std::lock_guard<std::mutex> lock(hlc_mutex);
    return hlc_enabled;
}

void EvseManager::log_v2g_message(Object m) {
    std::string msg = m["V2G_Message_ID"];

    std::string xml = "";
    std::string json_str = "";
    if (m["V2G_Message_XML"].is_null() && m["V2G_Message_JSON"].is_string()) {
        json_str = m["V2G_Message_JSON"];
    } else if (m["V2G_Message_XML"].is_string()) {
        xml = m["V2G_Message_XML"];
    }

    // All messages from EVSE contain Req and all originating from Car contain Res
    if (msg.find("Res") == std::string::npos) {
        session_log.car(true, fmt::format("V2G {}", msg), xml, m["V2G_Message_EXI_Hex"],
                        m["V2G_Message_EXI_Base64"], json_str);
    } else {
        session_log.evse(true, fmt::format("V2G {}", msg), xml, m["V2G_Message_EXI_Hex"],
                         m["V2G_Message_EXI_Base64"], json_str);
    }
}

} // namespace module
