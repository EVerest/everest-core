// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "session_cost_API.hpp"

#include <everest_api_types/session_cost/API.hpp>
#include <everest_api_types/session_cost/codec.hpp>
#include <everest_api_types/session_cost/wrapper.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include <everest/logging.hpp>

namespace module {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::session_cost;
using ev_API::deserialize;

void session_cost_API::init() {
    invoke_init(*p_main);
    invoke_init(*p_generic_error);

    topics.setTargetApiModuleID(info.id, "session_cost");
}

void session_cost_API::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_generic_error);

    generate_api_var_tariff_message();
    generate_api_var_session_cost();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void session_cost_API::generate_api_var_tariff_message() {
    subscribe_api_topic("tariff_message", [=](const std::string& data) {
        API_types_ext::TariffMessage payload;
        if (deserialize(data, payload)) {
            p_main->publish_tariff_message(to_internal_api(payload));
            return true;
        }
        return false;
    });
}

void session_cost_API::generate_api_var_session_cost() {
    subscribe_api_topic("session_cost", [=](const std::string& data) {
        API_types_ext::SessionCost payload;
        if (deserialize(data, payload)) {
            p_main->publish_session_cost(to_internal_api(payload));
            return true;
        }
        return false;
    });
}

void session_cost_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void session_cost_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void session_cost_API::subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish) {
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

const ev_API::Topics& session_cost_API::get_topics() const {
    return topics;
}

} // namespace module
