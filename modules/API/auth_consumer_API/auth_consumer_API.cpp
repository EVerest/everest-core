// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "auth_consumer_API.hpp"
#include "basecamp/auth/API.hpp"
#include "basecamp/auth/codec.hpp"
#include "basecamp/auth/wrapper.hpp"
#include "basecamp/generic/codec.hpp"
#include "basecamp/utilities/codec.hpp"

using basecamp::API::deserialize;
namespace generic = basecamp::API::V1_0::types::generic;

namespace module {

namespace ns_types_ext = basecamp::API::V1_0::types::auth;

void auth_consumer_API::init() {
    invoke_init(*p_main);
    topics.setTargetApiModuleID(info.id, "auth_consumer");

    generate_api_cmd_withdraw_authorization();
    generate_api_var_token_validation_status();
    generate_api_var_communication_check();
}

void auth_consumer_API::ready() {
    invoke_ready(*p_main);

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

auto auth_consumer_API::forward_api_var(std::string const& var) {
    using namespace ns_types_ext;
    auto topic = topics.basecamp_to_extern(var);
    return [this, topic](auto const& val) {
        try {
            auto&& external = toExternalApi(val);
            auto&& payload = serialize(external);
            mqtt.publish(topic, payload);
        } catch (const std::exception& e) {
            EVLOG_warning << "Variable: '" << topic << "' failed with -> " << e.what();
        } catch (...) {
            EVLOG_warning << "Invalid data: Cannot convert internal to external or serialize it.\n" << topic;
        }
    };
}

void auth_consumer_API::generate_api_cmd_withdraw_authorization() {
    subscribe_api_topic("withdraw_authorization", [=](std::string const& data) {
        generic::RequestReply msg;
        if (deserialize(data, msg)) {
            ns_types_ext::WithdrawAuthorizationRequest payload;
            if (deserialize(msg.payload, payload)) {
                auto int_arg = toInternalApi(payload);
                auto int_res = r_auth->call_withdraw_authorization(int_arg);
                auto ext_res = ns_types_ext::toExternalApi(int_res);
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
    subscribe_api_topic("communication_check", [this](const json& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void auth_consumer_API::setup_heartbeat_generator() {
    auto topic = topics.basecamp_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void auth_consumer_API::subscribe_api_topic(const std::string& var, const ParseAndPublishFtor& parse_and_publish) {
    auto topic = topics.extern_to_basecamp(var);
    mqtt.subscribe(topic, [=](std::string const& data) {
        try {
            if (not parse_and_publish(data)) {
                EVLOG_warning << "Invalid data: Deserialization failed.\n" << topic << "\n" << data;
            }
        } catch (const std::exception& e) {
            EVLOG_warning << "Cmd/Var: '" << topic << "' failed with -> " << e.what();
        } catch (...) {
            EVLOG_warning << "Invalid data: Failed to parse JSON or to get data from it.\n" << topic;
        }
    });
}

} // namespace module
