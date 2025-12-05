// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "session_cost_consumer_API.hpp"

#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/session_cost/API.hpp>
#include <everest_api_types/session_cost/codec.hpp>
#include <everest_api_types/session_cost/wrapper.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include <everest/logging.hpp>

namespace module {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::session_cost;
namespace API_generic = API_types::generic;
using ev_API::deserialize;

void session_cost_consumer_API::init() {
    invoke_init(*p_main);

    topics.setup(info.id, "session_cost_consumer", 1);
}

void session_cost_consumer_API::ready() {
    invoke_ready(*p_main);

    generate_api_var_tariff_message();
    generate_api_var_session_cost();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

auto session_cost_consumer_API::forward_api_var(std::string const& var) {
    using namespace API_types_ext;
    using namespace API_generic;
    auto topic = topics.everest_to_extern(var);
    return [this, topic](auto const& val) {
        try {
            auto&& external = to_external_api(val);
            auto&& payload = serialize(external);
            mqtt.publish(topic, payload);
        } catch (const std::exception& e) {
            EVLOG_warning << "Variable: '" << topic << "' failed with -> " << e.what();
        } catch (...) {
            EVLOG_warning << "Invalid data: Cannot convert internal to external or serialize it.\n" << topic;
        }
    };
}

void session_cost_consumer_API::generate_api_var_tariff_message() {
    r_session_cost->subscribe_tariff_message(forward_api_var("tariff_message"));
}

void session_cost_consumer_API::generate_api_var_session_cost() {
    r_session_cost->subscribe_session_cost(forward_api_var("session_cost"));
}

void session_cost_consumer_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void session_cost_consumer_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, API_generic::serialize(hb_id++));
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void session_cost_consumer_API::subscribe_api_topic(std::string const& var,
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
