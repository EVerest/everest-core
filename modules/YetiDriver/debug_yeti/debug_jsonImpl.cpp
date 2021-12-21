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
#include "debug_jsonImpl.hpp"

namespace module {
namespace debug_yeti {

void debug_jsonImpl::init() {
    mod->serial.signalDebugUpdate.connect([this](const DebugUpdate& d) {
        auto d_json = debug_update_to_json(d);
        mod->mqtt.publish("/external/debug_json", d_json.dump());
        publish_debug_json(d_json);
    });
}

void debug_jsonImpl::ready() {
}

} // namespace debug_yeti
} // namespace module
