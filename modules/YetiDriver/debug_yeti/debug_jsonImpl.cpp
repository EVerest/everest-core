// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
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
