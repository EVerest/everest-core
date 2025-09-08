// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "over_voltage_monitorImpl.hpp"
#include "basecamp/generic/codec.hpp"

namespace module {
namespace main {

using namespace basecamp::companion;
namespace generic = basecamp::API::V1_0::types::generic;

void over_voltage_monitorImpl::init() {
}

void over_voltage_monitorImpl::ready() {
}

void over_voltage_monitorImpl::handle_start(double& over_voltage_limit_V) {
    auto topic = mod->get_topics().basecamp_to_extern("start");
    auto data = generic::serialize(over_voltage_limit_V);
    mod->mqtt.publish(topic, data);
}

void over_voltage_monitorImpl::handle_stop() {
    auto topic = mod->get_topics().basecamp_to_extern("stop");
    mod->mqtt.publish(topic, "{}");
}

void over_voltage_monitorImpl::handle_reset_over_voltage_error() {
    auto topic = mod->get_topics().basecamp_to_extern("reset_over_voltage_error");
    mod->mqtt.publish(topic, "{}");
}

} // namespace main
} // namespace module
