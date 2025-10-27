// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "auth_token_validator_API.hpp"

#include <everest_api_types/auth/API.hpp>
#include <everest_api_types/auth/codec.hpp>
#include <everest_api_types/auth/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include <everest/logging.hpp>

namespace module {

namespace API_types = everest::lib::API::V1_0::types;
namespace API_types_ext = API_types::auth;
namespace API_generic = API_types::generic;
using ev_API::deserialize;

void auth_token_validator_API::init() {
    invoke_init(*p_auth_token_validator);

    topics.setup(info.id, "auth_token_validator", 1);
}

void auth_token_validator_API::ready() {
    invoke_ready(*p_auth_token_validator);

    generate_api_var_validation_result_update();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void auth_token_validator_API::generate_api_var_validation_result_update() {
    subscribe_api_topic("validate_result_update", [=](std::string const& data) {
        API_types_ext::ValidationResultUpdate payload;
        if (deserialize(data, payload)) {
            p_auth_token_validator->publish_validate_result_update(to_internal_api(payload));
            return true;
        }
        return false;
    });
}

void auth_token_validator_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void auth_token_validator_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void auth_token_validator_API::subscribe_api_topic(std::string const& var,
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

const ev_API::Topics& auth_token_validator_API::get_topics() const {
    return topics;
}

} // namespace module
