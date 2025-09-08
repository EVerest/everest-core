// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "evse_board_supportImpl.hpp"
#include "basecamp/evse_board_support/API.hpp"
#include "basecamp/evse_board_support/codec.hpp"
#include "basecamp/evse_board_support/json_codec.hpp"
#include "basecamp/evse_board_support/wrapper.hpp"
#include "basecamp/generic/codec.hpp"
#include "companion/asyncapi/AsyncApiRequestReply.hpp"
#include "everest/logging.hpp"
#include "generated/types/board_support_common.hpp"
#include "generated/types/evse_board_support.hpp"

namespace module {
namespace board_support {

using namespace basecamp::companion;
namespace generic = basecamp::API::V1_0::types::generic;

void evse_board_supportImpl::init() {
    timeout_s = mod->config.cfg_request_reply_to_s;
}

void evse_board_supportImpl::ready() {
}

template <class T, class ReqT>
auto evse_board_supportImpl::generic_request_reply(T const& default_value, ReqT const& request,
                                                   std::string const& topic) {
    using namespace ns_types_ext;
    using ExtT = decltype(toExternalApi(std::declval<T>()));
    auto result = request_reply_handler<ExtT>(mod->mqtt, mod->get_topics(), request, topic, timeout_s);
    if (!result) {
        return default_value;
    }
    return result.value();
}

void evse_board_supportImpl::handle_enable(bool& value) {
    auto topic = mod->get_topics().basecamp_to_extern("enable");
    auto data = generic::serialize(value);
    mod->mqtt.publish(topic, data);
}

void evse_board_supportImpl::handle_pwm_on(double& value) {
    auto topic = mod->get_topics().basecamp_to_extern("pwm_on");
    auto data = generic::serialize(value);
    mod->mqtt.publish(topic, data);
}

void evse_board_supportImpl::handle_pwm_off() {
    auto topic = mod->get_topics().basecamp_to_extern("pwm_off");
    mod->mqtt.publish(topic, "");
}

void evse_board_supportImpl::handle_pwm_F() {
    auto topic = mod->get_topics().basecamp_to_extern("pwm_F");
    mod->mqtt.publish(topic, "");
}

void evse_board_supportImpl::handle_allow_power_on(types::evse_board_support::PowerOnOff& value) {
    auto topic = mod->get_topics().basecamp_to_extern("allow_power_on");
    auto ext = ns_types_ext::toExternalApi(value);
    auto data = ns_types_ext::serialize(ext);
    mod->mqtt.publish(topic, data);
}

void evse_board_supportImpl::handle_ac_switch_three_phases_while_charging(bool& value) {
    auto topic = mod->get_topics().basecamp_to_extern("ac_switch_three_phases_while_charging");
    std::string raw_data = value ? "ThreePhases" : "SinglePhase";
    auto data = generic::serialize(raw_data);
    mod->mqtt.publish(topic, data);
}

void evse_board_supportImpl::handle_evse_replug(int& value) {
    auto topic = mod->get_topics().basecamp_to_extern("evse_replug");
    auto data = generic::serialize(value);
    mod->mqtt.publish(topic, data);
}

types::board_support_common::ProximityPilot evse_board_supportImpl::handle_ac_read_pp_ampacity() {
    static const types::board_support_common::ProximityPilot default_response =
        types::board_support_common::ProximityPilot{types::board_support_common::Ampacity::None};
    return generic_request_reply(default_response, internal::empty_payload, "ac_pp_ampacity");
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
    auto topic = mod->get_topics().basecamp_to_extern("ac_overcurrent_limit");
    auto data = generic::serialize(value);
    mod->mqtt.publish(topic, data);
}

} // namespace board_support
} // namespace module
