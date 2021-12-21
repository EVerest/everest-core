/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
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

    r_powermeter->subscribe_powermeter([this](json p) {
        charger->setCurrentDrawnByVehicle(p["current_A"]["L1"], p["current_A"]["L2"], p["current_A"]["L3"]);
    });

    charger->setup(config.three_phases, config.has_ventilation, config.country_code, config.rcd_enabled);
    invoke_ready(*p_evse);
    charger->run();
    charger->enable();
}

} // namespace module
