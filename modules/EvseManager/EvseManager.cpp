// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "EvseManager.hpp"
#include <chrono>

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

    invoke_ready(*p_evse);
    invoke_ready(*p_energy_grid);

    charger->setup(local_three_phases, config.has_ventilation, config.country_code, config.rcd_enabled,
                   config.charge_mode, slac_enabled, config.ac_hlc_use_5percent, config.ac_enforce_hlc);
    //  start with a limit of 0 amps. We will get a budget from EnergyManager that is locally limited by hw caps.
    charger->setMaxCurrent(0., std::chrono::system_clock::now());

    charger->run();
    charger->enable();
}

json EvseManager::get_latest_powermeter_data() {
    return latest_powermeter_data;
}

json EvseManager::get_hw_capabilities() {
    return hw_capabilities;
}

bool EvseManager::updateLocalMaxCurrentLimit(float max_current) {
    if (max_current >= 0.0F && max_current < EVSE_ABSOLUTE_MAX_CURRENT) {
        local_max_current_limit = max_current;
        
        // wait for EnergyManager to assign optimized current on next opimizer run

        return true;
    }
    return false;
}

float EvseManager::getLocalMaxCurrentLimit() {
    return local_max_current_limit;
}

} // namespace module
