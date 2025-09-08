// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "isolation_monitorImpl.hpp"
#include "basecamp/generic/codec.hpp"
#include "companion/paths/Topics.hpp"

namespace module {
namespace generic = basecamp::API::V1_0::types::generic;

namespace if_isolation_monitor {

void isolation_monitorImpl::init() {
}

void isolation_monitorImpl::ready() {
}

void isolation_monitorImpl::handle_start() {
    mod->mqtt.publish(mod->get_topics().basecamp_to_extern("receive_start"), "{}");
}

void isolation_monitorImpl::handle_stop() {
    mod->mqtt.publish(mod->get_topics().basecamp_to_extern("receive_stop"), "{}");
}

void isolation_monitorImpl::handle_start_self_test(double& test_voltage_V) {
    auto value = generic::serialize(test_voltage_V);
    mod->mqtt.publish(mod->get_topics().basecamp_to_extern("receive_start_self_test"), value);
}

} // namespace if_isolation_monitor
} // namespace module
