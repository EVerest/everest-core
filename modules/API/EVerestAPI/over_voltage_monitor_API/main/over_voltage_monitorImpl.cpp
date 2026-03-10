// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "over_voltage_monitorImpl.hpp"

#include <everest_api_types/over_voltage_monitor/API.hpp>
#include <everest_api_types/over_voltage_monitor/codec.hpp>

namespace module {
namespace main {

using namespace everest::lib::API;
namespace API_ovm = API_types::over_voltage_monitor;

void over_voltage_monitorImpl::init() {
}

void over_voltage_monitorImpl::ready() {
}

void over_voltage_monitorImpl::handle_set_limits(double& emergency_over_voltage_limit_V,
                                                 double& error_over_voltage_limit_V) {
    auto topic = mod->get_topics().everest_to_extern("set_limits");
    API_ovm::OverVoltageLimits data;
    data.emergency_limit_V = emergency_over_voltage_limit_V;
    data.error_limit_V = error_over_voltage_limit_V;
    auto msg = API_ovm::serialize(data);
    mod->mqtt.publish(topic, msg);
}

void over_voltage_monitorImpl::handle_start() {
    auto topic = mod->get_topics().everest_to_extern("start");
    mod->mqtt.publish(topic, "{}");
}

void over_voltage_monitorImpl::handle_stop() {
    auto topic = mod->get_topics().everest_to_extern("stop");
    mod->mqtt.publish(topic, "{}");
}

void over_voltage_monitorImpl::handle_reset_over_voltage_error() {
    auto topic = mod->get_topics().everest_to_extern("reset_over_voltage_error");
    mod->mqtt.publish(topic, "{}");
}

} // namespace main
} // namespace module
