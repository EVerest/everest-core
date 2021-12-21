// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "debug_jsonImpl.hpp"

namespace module {
namespace debug_state {

void debug_jsonImpl::init() {
    mod->serial.signalStateUpdate.connect(
        [this](const StateUpdate& s) { publish_debug_json(state_update_to_json(s)); });
}

void debug_jsonImpl::ready() {
}

} // namespace debug_state
} // namespace module
