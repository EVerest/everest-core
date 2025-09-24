// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "auth_token_provider_API.hpp"

#include <everest_api_types/auth/API.hpp>
#include <everest_api_types/auth/json_codec.hpp>
#include <everest_api_types/auth/wrapper.hpp>

#include <everest/logging.hpp>

namespace module {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::auth;

void auth_token_provider_API::init() {
    invoke_init(*p_main);

    topics.setTargetApiModuleID(info.id, "auth_token_provider");
}

void auth_token_provider_API::ready() {
    invoke_ready(*p_main);

    generate_api_var_provided_token();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void auth_token_provider_API::generate_api_var_communication_check() {
    subscribe_api_var("communication_check", [this](const json& data) {
        auto val = data.template get<bool>();
        comm_check.set_value(val);
    });
}

void auth_token_provider_API::generate_api_var_provided_token() {
    subscribe_api_var("provided_token", [=](const json& data) {
        API_types_ext::ProvidedIdToken external = data;
        auto internal = to_internal_api(external);
        p_main->publish_provided_token(internal);
    });
}

void auth_token_provider_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void auth_token_provider_API::subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish) {
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

const ev_API::Topics& auth_token_provider_API::get_topics() const {
    return topics;
}

} // namespace module
