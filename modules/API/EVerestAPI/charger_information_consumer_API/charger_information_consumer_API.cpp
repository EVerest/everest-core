// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "charger_information_consumer_API.hpp"

#include <everest_api_types/charger_information/API.hpp>
#include <everest_api_types/charger_information/codec.hpp>
#include <everest_api_types/charger_information/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include <utility>

namespace module {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::charger_information;
namespace API_generic = API_types::generic;
using ev_API::deserialize;

void charger_information_consumer_API::init() {
    invoke_init(*p_main);

    topics.setup(info.id, "charger_information_consumer", 1);
}

void charger_information_consumer_API::ready() {
    invoke_ready(*p_main);

    generate_api_cmd_get_charger_information();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void charger_information_consumer_API::generate_api_cmd_get_charger_information() {
    subscribe_api_topic("get_charger_information", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            auto int_res = r_charger_information->call_get_charger_information();
            auto ext_res = API_types_ext::to_external_api(int_res);
            mqtt.publish(msg.replyTo, serialize(ext_res));
            return true;
        }
        return false;
    });
}

void charger_information_consumer_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void charger_information_consumer_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, API_generic::serialize(hb_id++));
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void charger_information_consumer_API::subscribe_api_topic(std::string const& var,
                                                           ParseAndPublishFtor const& parse_and_publish) {
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

} // namespace module
