// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "connector_lockImpl.hpp"

namespace module {
namespace connector_lock {

using namespace std::literals::chrono_literals;

void connector_lockImpl::init() {
}

void connector_lockImpl::ready() {
}

void connector_lockImpl::handle_lock() {
    auto topic = mod->get_topics().everest_to_extern("lock");
    mod->mqtt.publish(topic, "");
}

void connector_lockImpl::handle_unlock() {
    auto topic = mod->get_topics().everest_to_extern("unlock");
    mod->mqtt.publish(topic, "");
}

} // namespace connector_lock
} // namespace module
