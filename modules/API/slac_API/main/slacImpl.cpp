// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "slacImpl.hpp"

#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/slac/API.hpp>
#include <everest_api_types/slac/codec.hpp>

namespace module {
namespace main {

using namespace everest::lib::API;
namespace generic = everest::lib::API::V1_0::types::generic;

void slacImpl::init() {
}

void slacImpl::ready() {
}

void slacImpl::handle_reset(bool& enable) {
    const auto topic = mod->get_topics().everest_to_extern("reset");
    const auto data = generic::serialize(enable);
    mod->mqtt.publish(topic, data);
}

void slacImpl::handle_enter_bcd() {
    const auto topic = mod->get_topics().everest_to_extern("enter_bcd");
    mod->mqtt.publish(topic, "{}");
}

void slacImpl::handle_leave_bcd() {
    const auto topic = mod->get_topics().everest_to_extern("leave_bcd");
    mod->mqtt.publish(topic, "{}");
}

void slacImpl::handle_dlink_terminate() {
    const auto topic = mod->get_topics().everest_to_extern("dlink_terminate");
    mod->mqtt.publish(topic, "{}");
}

void slacImpl::handle_dlink_error() {
    const auto topic = mod->get_topics().everest_to_extern("dlink_error");
    mod->mqtt.publish(topic, "{}");
}

void slacImpl::handle_dlink_pause() {
    const auto topic = mod->get_topics().everest_to_extern("dlink_pause");
    mod->mqtt.publish(topic, "{}");
}

} // namespace main
} // namespace module
