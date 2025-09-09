// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "display_message_API.hpp"
#include <everest_api_types/display_message/API.hpp>
#include <everest_api_types/display_message/codec.hpp>
#include <everest_api_types/display_message/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>
#include "generated/types/display_message.hpp"

namespace module {

namespace ns_types_ext = ns_ev_api::V1_0::types::display_message;
namespace generic = ns_ev_api::V1_0::types::generic;

void display_message_API::init() {
    invoke_init(*p_main);
    invoke_init(*p_generic_error);

    topics.setTargetApiModuleID(info.id, "display_message");
}

void display_message_API::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_generic_error);

    comm_check.start(config.cfg_communication_check_to_s);
    generate_api_var_communication_check();
    setup_heartbeat_generator();
}

void display_message_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        auto val = generic::deserialize<bool>(data);
        comm_check.set_value(val);
        return true;
    });
}

void display_message_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void display_message_API::subscribe_api_topic(const std::string& var, const ParseAndPublishFtor& parse_and_publish) {
    auto topic = topics.extern_to_everest(var);
    mqtt.subscribe(topic, [=](std::string const& data) {
        try {
            if (not parse_and_publish(data)) {
                EVLOG_warning << "Invalid data: Deserialization failed.\n" << topic << "\n" << data;
            }
        } catch (const std::exception& e) {
            EVLOG_warning << "Cmd/Var: '" << topic << "' failed with -> " << e.what();
        } catch (...) {
            EVLOG_warning << "Invalid data: Failed to parse JSON or to get data from it.\n" << topic;
        }
    });
}

const ns_ev_api::Topics& display_message_API::get_topics() const {
    return topics;
}

} // namespace module
