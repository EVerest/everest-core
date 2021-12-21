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
#include "power_resultImpl.hpp"

namespace module {
namespace main {

void power_resultImpl::init() {
    mod->r_powerin->subscribe_max_current([this](double max_current) {
        EVLOG(debug) << "Incoming power_in current: " << max_current;
        std::unique_lock<std::mutex> lock(max_current_mutex);
        max_currents["powerin"] = max_current;
        lock.unlock();
        update_max_current();
    });

    mod->r_powerin->subscribe_phase_count([this](int phase_count) {
        // we just forward this for now
        EVLOG(debug) << "Incoming power_in phase_count: " << phase_count;
        publish_phase_count(phase_count);
    });

    mod->ro_solar->subscribe_max_current([this](double max_current) {
        EVLOG(debug) << "Incoming solar current: " << max_current;
        std::unique_lock<std::mutex> lock(max_current_mutex);
        max_currents["optional:solar"] = max_current;
        lock.unlock();
        update_max_current();
    });
}

void power_resultImpl::ready() {
}

void power_resultImpl::update_max_current() {
    std::unique_lock<std::mutex> lock(max_current_mutex);
    double min = std::min_element(max_currents.begin(), max_currents.end(), [](const auto& left, const auto& right) {
                     return left.second < right.second;
                 })->second;
    lock.unlock();
    publish_max_current(min);
}

} // namespace main
} // namespace module
