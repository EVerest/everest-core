// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "auth_consumer_API.hpp"

#include <everest_api_types/auth/API.hpp>
#include <everest_api_types/auth/codec.hpp>
#include <everest_api_types/auth/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include <utility>

namespace module {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::auth;
namespace API_generic = API_types::generic;
using ev_API::deserialize;

void auth_consumer_API::init() {
    invoke_init(*p_main);

    topics.setTargetApiModuleID(info.id, "auth_consumer");
}

void auth_consumer_API::ready() {
    invoke_ready(*p_main);

    generate_api_cmd_set_connection_timeout();
    generate_api_cmd_set_master_pass_group_id();
    generate_api_cmd_withdraw_authorization();
    generate_api_var_token_validation_status();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

auto auth_consumer_API::forward_api_var(std::string const& var) {
    using namespace API_types_ext;
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

void auth_consumer_API::generate_api_cmd_set_connection_timeout() {
    subscribe_api_topic("set_connection_timeout", [this](std::string const& data) {
        int connection_timeout = 0;
        if (deserialize(data, connection_timeout)) {
            r_auth->call_set_connection_timeout(connection_timeout);
            return true;
        }
        return false;
    });
}

void auth_consumer_API::generate_api_cmd_set_master_pass_group_id() {
    subscribe_api_topic("set_master_pass_group_id", [this](std::string const& data) {
        std::string master_pass_group_id;
        if (deserialize(data, master_pass_group_id)) {
            r_auth->call_set_master_pass_group_id(std::move(master_pass_group_id));
            return true;
        }
        return false;
    });
}

void auth_consumer_API::generate_api_cmd_withdraw_authorization() {
    subscribe_api_topic("withdraw_authorization", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::WithdrawAuthorizationRequest payload;
            if (deserialize(msg.payload, payload)) {
                auto int_res = r_auth->call_withdraw_authorization(to_internal_api(payload));
                auto ext_res = API_types_ext::to_external_api(int_res);
                mqtt.publish(msg.replyTo, serialize(ext_res));
                return true;
            }
        }
        return false;
    });
}

void auth_consumer_API::generate_api_var_token_validation_status() {
    r_auth->subscribe_token_validation_status(forward_api_var("token_validation_status"));
}

void auth_consumer_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void auth_consumer_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void auth_consumer_API::subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish) {
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
