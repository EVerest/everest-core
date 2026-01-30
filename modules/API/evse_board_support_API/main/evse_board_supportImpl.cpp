// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "evse_board_supportImpl.hpp"

#include <everest/logging.hpp>
#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/evse_board_support/codec.hpp>
#include <everest_api_types/evse_board_support/json_codec.hpp>
#include <everest_api_types/evse_board_support/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/AsyncApiRequestReply.hpp>

#include <generated/types/board_support_common.hpp>
#include <generated/types/evse_board_support.hpp>

namespace module {
namespace main {

using namespace everest::lib::API;
namespace generic = everest::lib::API::V1_0::types::generic;

void evse_board_supportImpl::init() {
    timeout_s = mod->config.cfg_request_reply_to_s;
}

void evse_board_supportImpl::ready() {
}

void evse_board_supportImpl::handle_enable(bool& value) {
    auto topic = mod->get_topics().everest_to_extern("enable");
    auto data = generic::serialize(value);
    mod->mqtt.publish(topic, data);
}

void evse_board_supportImpl::handle_pwm_on(double& value) {
    auto topic = mod->get_topics().everest_to_extern("pwm_on");
    auto data = generic::serialize(value);
    mod->mqtt.publish(topic, data);
}

void evse_board_supportImpl::handle_cp_state_X1() {
    auto topic = mod->get_topics().everest_to_extern("cp_state_X1");
    mod->mqtt.publish(topic, "");
}

void evse_board_supportImpl::handle_cp_state_F() {
    auto topic = mod->get_topics().everest_to_extern("cp_state_F");
    mod->mqtt.publish(topic, "");
}

void evse_board_supportImpl::handle_cp_state_E() {
    auto topic = mod->get_topics().everest_to_extern("cp_state_E");
    mod->mqtt.publish(topic, "");
}

void evse_board_supportImpl::handle_allow_power_on(types::evse_board_support::PowerOnOff& value) {
    auto topic = mod->get_topics().everest_to_extern("allow_power_on");
    auto ext = API_types_ext::to_external_api(value);
    auto data = API_types_ext::serialize(ext);
    mod->mqtt.publish(topic, data);
}

void evse_board_supportImpl::handle_ac_switch_three_phases_while_charging(bool& value) {
    auto topic = mod->get_topics().everest_to_extern("ac_switch_three_phases_while_charging");
    std::string raw_data = value ? "ThreePhases" : "SinglePhase";
    auto data = generic::serialize(raw_data);
    mod->mqtt.publish(topic, data);
}

void evse_board_supportImpl::handle_evse_replug(int& value) {
    auto topic = mod->get_topics().everest_to_extern("evse_replug");
    auto data = generic::serialize(value);
    mod->mqtt.publish(topic, data);
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
    auto topic = mod->get_topics().everest_to_extern("ac_overcurrent_limit");
    auto data = generic::serialize(value);
    mod->mqtt.publish(topic, data);
}

} // namespace main
} // namespace module
