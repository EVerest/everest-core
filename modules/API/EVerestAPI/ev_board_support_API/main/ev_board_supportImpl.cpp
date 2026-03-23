// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ev_board_supportImpl.hpp"

#include <everest/logging.hpp>
#include <everest_api_types/ev_board_support/API.hpp>
#include <everest_api_types/ev_board_support/codec.hpp>
#include <everest_api_types/ev_board_support/json_codec.hpp>
#include <everest_api_types/ev_board_support/wrapper.hpp>
#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/evse_board_support/codec.hpp>
#include <everest_api_types/evse_board_support/json_codec.hpp>
#include <everest_api_types/evse_board_support/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>

#include <generated/types/board_support_common.hpp>
#include <generated/types/evse_board_support.hpp>

using namespace everest::lib::API;
namespace generic = everest::lib::API::V1_0::types::generic;
namespace API_types = ev_API::V1_0::types;

namespace module {
namespace main {

void ev_board_supportImpl::init() {
}

void ev_board_supportImpl::ready() {
}

void ev_board_supportImpl::handle_enable(bool& value) {
    const auto topic = mod->get_topics().everest_to_extern("enable");
    const auto data = generic::serialize(value);
    mod->mqtt.publish(topic, data);
}

void ev_board_supportImpl::handle_set_cp_state(types::ev_board_support::EvCpState& cp_state) {
    const auto topic = mod->get_topics().everest_to_extern("set_cp_state");
    const auto ext = API_types::ev_board_support::to_external_api(cp_state);
    const auto data = serialize(ext);
    mod->mqtt.publish(topic, data);
}

void ev_board_supportImpl::handle_allow_power_on(bool& value) {
    const auto topic = mod->get_topics().everest_to_extern("allow_power_on");
    const auto data = generic::serialize(value);
    mod->mqtt.publish(topic, data);
}

void ev_board_supportImpl::handle_diode_fail(bool& value) {
    const auto topic = mod->get_topics().everest_to_extern("diode_fail");
    const auto data = generic::serialize(value);
    mod->mqtt.publish(topic, data);
}

void ev_board_supportImpl::handle_set_ac_max_current(double& current) {
    const auto topic = mod->get_topics().everest_to_extern("set_ac_max_current");
    const auto data = generic::serialize(current);
    mod->mqtt.publish(topic, data);
}

void ev_board_supportImpl::handle_set_three_phases(bool& three_phases) {
    const auto topic = mod->get_topics().everest_to_extern("set_three_phases");
    const auto data = generic::serialize(three_phases);
    mod->mqtt.publish(topic, data);
}

void ev_board_supportImpl::handle_set_rcd_error(double& rcd_current_mA) {
    const auto topic = mod->get_topics().everest_to_extern("set_rcd_error");
    const auto data = generic::serialize(rcd_current_mA);
    mod->mqtt.publish(topic, data);
}

} // namespace main
} // namespace module
