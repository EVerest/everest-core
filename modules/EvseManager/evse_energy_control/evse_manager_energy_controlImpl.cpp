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
#include "evse_manager_energy_controlImpl.hpp"

namespace module {
namespace evse_energy_control {

void evse_manager_energy_controlImpl::init() {

}

void evse_manager_energy_controlImpl::ready() {

}

std::string evse_manager_energy_controlImpl::handle_set_max_current(double& max_current){
    if (mod->charger->setMaxCurrent(static_cast<float>(max_current)))
        return "Success";
    else return "Error_OutOfRange";
};

std::string evse_manager_energy_controlImpl::handle_switch_three_phases_while_charging(bool& three_phases){
    // FIXME implement more sophisticated error code return once feature is really implemented
    if (mod->charger->switchThreePhasesWhileCharging(three_phases)) return "Success";
    else return "Error_NotSupported";
};

} // namespace evse_energy_control
} // namespace module
