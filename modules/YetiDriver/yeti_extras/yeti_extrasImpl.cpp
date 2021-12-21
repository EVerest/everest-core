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
#include "yeti_extrasImpl.hpp"

namespace module {
namespace yeti_extras {

void yeti_extrasImpl::init() {
    mod->serial.signalKeepAliveLo.connect([this](const KeepAliveLo& k) {
        publish_time_stamp(k.time_stamp);
        publish_hw_type(k.hw_type);
        publish_hw_revision(k.hw_revision);
        publish_protocol_version_major(k.protocol_version_major);
        publish_protocol_version_minor(k.protocol_version_minor);
        publish_sw_version_string(k.sw_version_string);
    });
}

void yeti_extrasImpl::ready() {
}

void yeti_extrasImpl::handle_firmware_update(std::string& firmware_binary){
    // your code for cmd firmware_update goes here
};

} // namespace yeti_extras
} // namespace module
