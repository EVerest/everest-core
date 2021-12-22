// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "EvseManager.hpp"

namespace module {

void EvseManager::init() {
    invoke_init(*p_evse);       
}

void EvseManager::ready() {
    charger=std::unique_ptr<Charger>(new Charger(r_bsp));
    r_bsp->subscribe_event([this](std::string event) {
        charger->processEvent(event);
    });

    r_bsp->subscribe_nr_of_phases_available([this](int n) {
        signalNrOfPhasesAvailable(n);
    });

    r_powermeter->subscribe_powermeter([this](json p) {
        charger->setCurrentDrawnByVehicle(p["current_A"]["L1"], p["current_A"]["L2"], p["current_A"]["L3"]);
        latest_powermeter_data = p;
    });

    invoke_ready(*p_evse);
    charger->setup(config.three_phases, config.has_ventilation, config.country_code, config.rcd_enabled);
    charger->setMaxCurrent(16.0); // FIXME: make configurable
    charger->run();
    charger->enable();
}

json EvseManager::get_latest_powermeter_data() {
    return latest_powermeter_data;
}

} // namespace module
