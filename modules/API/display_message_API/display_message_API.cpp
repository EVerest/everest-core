// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "display_message_API.hpp"
#include <everest_api_types/display_message/API.hpp>
#include <everest_api_types/display_message/codec.hpp>
#include <everest_api_types/display_message/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>
#include <generated/types/display_message.hpp>

namespace module {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::display_message;
namespace API_generic = API_types::generic;
using ev_API::deserialize;

void display_message_API::init() {
    invoke_init(*p_main);
    invoke_init(*p_generic_error);

    topics.setTargetApiModuleID(info.id, "display_message");
}

void display_message_API::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_generic_error);

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void display_message_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
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

void display_message_API::subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish) {
    auto topic = topics.extern_to_everest(var);
    mqtt.subscribe(topic, [=](std::string const& data) {
        try {
            if (not parse_and_publish(data)) {
                EVLOG_warning << "Invalid data: Deserialization failed.\n" << topic << "\n" << data;
            }
        } catch (const std::exception& e) {
            EVLOG_warning << "Topic: '" << topic << "' failed with -> " << e.what() << "\n => " << data;
        } catch (...) {
            EVLOG_warning << "Invalid data: Failed to parse JSON or to get data from it.\n" << topic;
        }
    });
}

const ev_API::Topics& display_message_API::get_topics() const {
    return topics;
}

} // namespace module
