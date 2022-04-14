// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "EvseManager.hpp"
#include <chrono>

namespace module {

void EvseManager::init() {
    local_three_phases = config.three_phases;
    invoke_init(*p_evse);
    invoke_init(*p_energy_grid);
}

void EvseManager::ready() {
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
    r_bsp->subscribe_event([this](std::string event) { charger->processEvent(event); });

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

    charger->signalAuthRequired.connect([this]() {
        // The charger indicates it requires auth now. It will retry if we cannot give auth right now.
        if (authorization_available) {
            boost::variant<boost::blank, std::string> auth = r_auth->call_get_authorization();
            if (auth.which() == 1) {
                charger->Authorize(true, boost::get<std::string>(auth));
            }
        }
    });

    invoke_ready(*p_evse);
    invoke_ready(*p_energy_grid);

    charger->setup(local_three_phases, config.has_ventilation, config.country_code, config.rcd_enabled);
    //  start with a limit of 0 amps. We will get a budget from EnergyManager that is locally limited by hw caps.
    charger->setMaxCurrent(0.0F, std::chrono::system_clock::now());
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
        double current_max_current_A = charger->getMaxCurrent();
        /*
        // update charger limit only if it further reduces current limit
        // i.e. act now instead of waiting for energy manager to react.
        // when increasing the limit we need to wait for energy manager.
        if (current_max_current_A > local_max_current_limit) {
            charger->setMaxCurrent(local_max_current_limit,
                                   (std::chrono::system_clock::now() + std::chrono::seconds(10)));
        } else if (current_max_current_A == 0.0F) {
            charger->setMaxCurrent(local_max_current_limit,
                                   (std::chrono::system_clock::now() + std::chrono::seconds(10)));
            // TODO(LAD): where else to get validUntil from???
            // CC: We cannot get a new validUntil here, only the energy_manager can give us more time. We can however
            // update the max current and not update the validUntil.
            // To keep things simple I'd comment this shortcut out and always go through EM as this potentially hides
            // bugs. It can be readded once everything is stable
        } else {
            // wait for EnergyManager to assign optimized current on next opimizer run
        }
        */
        return true;
    }
    return false;
}

float EvseManager::getLocalMaxCurrentLimit() {
    return local_max_current_limit;
}

} // namespace module
