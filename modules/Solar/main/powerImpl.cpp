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
#include "powerImpl.hpp"

#include <chrono>

namespace module {
namespace main {

void powerImpl::init() {
}

void powerImpl::ready() {
    simulation_thread = std::thread(&powerImpl::simulation, this);
}

void powerImpl::simulation() {
    double step = 0;
    double pi = std::atan(1.0) * 4.0;

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        double max_current = std::abs(std::sin((pi / 64.0) * step * 3.20)) + 1.0;
        EVLOG(debug) << "Publishing max_current: " << max_current;
        publish_max_current(max_current);
        step += 1;
    }
}

} // namespace main
} // namespace module
