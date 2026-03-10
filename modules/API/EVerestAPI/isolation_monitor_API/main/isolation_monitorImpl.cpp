// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "isolation_monitorImpl.hpp"

#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/Topics.hpp>

namespace module {
namespace API_generic = API_types::generic;

namespace main {

void isolation_monitorImpl::init() {
}

void isolation_monitorImpl::ready() {
}

void isolation_monitorImpl::handle_start() {
    mod->mqtt.publish(mod->get_topics().everest_to_extern("start"), "{}");
}

void isolation_monitorImpl::handle_stop() {
    mod->mqtt.publish(mod->get_topics().everest_to_extern("stop"), "{}");
}

void isolation_monitorImpl::handle_start_self_test(double& test_voltage_V) {
    auto value = API_generic::serialize(test_voltage_V);
    mod->mqtt.publish(mod->get_topics().everest_to_extern("start_self_test"), value);
}

} // namespace main
} // namespace module
