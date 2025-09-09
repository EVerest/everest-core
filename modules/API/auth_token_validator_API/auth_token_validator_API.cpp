// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "auth_token_validator_API.hpp"
#include <everest_api_types/auth/API.hpp>
#include <everest_api_types/auth/codec.hpp>
#include <everest_api_types/auth/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>
#include "everest/logging.hpp"

namespace module {

namespace ns_types_ext = ns_ev_api::V1_0::types::auth;
namespace generic = ns_ev_api::V1_0::types::generic;
using ns_ev_api::deserialize;

void auth_token_validator_API::init() {
    invoke_init(*p_auth_token_validator);

    topics.setTargetApiModuleID(info.id, "auth_token_validator");

    generate_api_var_communication_check();
    generate_api_var_validation_result_update();
}

void auth_token_validator_API::ready() {
    invoke_ready(*p_auth_token_validator);

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void auth_token_validator_API::generate_api_var_validation_result_update() {
    subscribe_api_var("validate_result_update", [=](std::string const& data) {
        ns_types_ext::ValidationResultUpdate ext;
        if (deserialize(data, ext)) {
            p_auth_token_validator->publish_validate_result_update(to_internal_api(ext));
            return true;
        }
        return false;
    });
}

void auth_token_validator_API::generate_api_var_communication_check() {
    subscribe_api_var("communication_check", [this](std::string const& data) {
        auto val = generic::deserialize<bool>(data);
        comm_check.set_value(val);
        return true;
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

void auth_token_validator_API::subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish) {
    auto topic = topics.extern_to_everest(var);
    mqtt.subscribe(topic, [=](std::string const& data) {
        try {
            if (not parse_and_publish(data)) {
                EVLOG_warning << "Invalid data: Deserialization failed.\n" << topic << "\n" << data;
            }
        } catch (const std::exception& e) {
            EVLOG_warning << "Variable: '" << topic << "' failed with -> " << e.what();
        } catch (...) {
            EVLOG_warning << "Invalid data: Failed to parse JSON or to get data from it.\n" << topic;
        }
    });
}

const ns_ev_api::Topics& auth_token_validator_API::get_topics() const {
    return topics;
}

} // namespace module
