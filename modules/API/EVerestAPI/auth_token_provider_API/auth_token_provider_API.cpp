// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "auth_token_provider_API.hpp"

#include <everest/logging.hpp>
#include <everest_api_types/auth/API.hpp>
#include <everest_api_types/auth/codec.hpp>
#include <everest_api_types/auth/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>

namespace module {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::auth;
namespace API_generic = API_types::generic;

using ev_API::deserialize;

void auth_token_provider_API::init() {
    invoke_init(*p_main);

    topics.setup(info.id, "auth_token_provider", 1);
}

void auth_token_provider_API::ready() {
    invoke_ready(*p_main);

    generate_api_var_provided_token();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void auth_token_provider_API::generate_api_var_provided_token() {
    subscribe_api_topic("provided_token", [this](const std::string& data) {
        API_types_ext::ProvidedIdToken payload;
        if (deserialize(data, payload)) {
            p_main->publish_provided_token(to_internal_api(payload));
            return true;
        }
        return false;
    });
}

void auth_token_provider_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void auth_token_provider_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, API_generic::serialize(hb_id++));
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void auth_token_provider_API::subscribe_api_topic(std::string const& var,
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

const ev_API::Topics& auth_token_provider_API::get_topics() const {
    return topics;
}

} // namespace module
