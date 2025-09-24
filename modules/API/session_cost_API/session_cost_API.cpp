// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "session_cost_API.hpp"

#include <everest_api_types/session_cost/API.hpp>
#include <everest_api_types/session_cost/json_codec.hpp>
#include <everest_api_types/session_cost/wrapper.hpp>

#include <everest/logging.hpp>

namespace module {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::session_cost;

void session_cost_API::init() {
    invoke_init(*p_main);

    topics.setTargetApiModuleID(info.id, "session_cost");
}

void session_cost_API::ready() {
    invoke_ready(*p_main);

    generate_api_var_tariff_message();
    generate_api_var_session_cost();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void session_cost_API::generate_api_var_communication_check() {
    subscribe_api_var("communication_check", [this](const json& data) {
        auto val = data.template get<bool>();
        comm_check.set_value(val);
    });
}

void session_cost_API::generate_api_var_tariff_message() {
    subscribe_api_var("tariff_message", [=](const json& data) {
        API_types_ext::TariffMessage external = data;
        auto internal = to_internal_api(external);
        p_main->publish_tariff_message(internal);
    });
}

void session_cost_API::generate_api_var_session_cost() {
    subscribe_api_var("session_cost", [=](const json& data) {
        API_types_ext::SessionCost external = data;
        auto internal = to_internal_api(external);
        p_main->publish_session_cost(internal);
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

void session_cost_API::subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish) {
    auto topic = topics.extern_to_everest(var);
    mqtt.subscribe(topic, [=](std::string const& raw_data) {
        try {
            auto data = json::parse(raw_data);
            parse_and_publish(data);
        } catch (const std::exception& e) {
            EVLOG_warning << "Variable: '" << topic << "' failed with -> " << e.what();
        } catch (...) {
            EVLOG_warning << "Invalid data: Failed to parse JSON or to get data from it.\n" << topic;
        }
    });
}

const ev_API::Topics& session_cost_API::get_topics() const {
    return topics;
}

} // namespace module
