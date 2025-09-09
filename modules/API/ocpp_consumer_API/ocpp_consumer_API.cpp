// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ocpp_consumer_API.hpp"
#include <everest_api_types/generic/API.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/ocpp/API.hpp>
#include <everest_api_types/ocpp/codec.hpp>
#include <everest_api_types/ocpp/wrapper.hpp>
#include <everest_api_types/utilities/codec.hpp>

namespace {
template <class T> T const& to_external_api(T const& val) {
    return val;
}
} // namespace

namespace module {

namespace ns_ev_api = everest::lib::API;
namespace generic = ns_ev_api::V1_0::types::generic;
namespace ns_types_ext = ns_ev_api::V1_0::types::ocpp;
using ns_ev_api::deserialize;

void ocpp_consumer_API::init() {
    invoke_init(*p_main);
    topics.setTargetApiModuleID(info.id, "ocpp_consumer");

    generate_api_cmd_data_transfer();
    generate_api_cmd_get_variables();
    generate_api_cmd_set_variables();
    generate_api_var_security_event();
    generate_api_var_is_connected();
    generate_api_var_boot_notification_response();
    generate_api_var_ocpp_transaction_event();
}

void ocpp_consumer_API::ready() {
    invoke_ready(*p_main);

    comm_check.start(config.cfg_communication_check_to_s);
    generate_api_var_communication_check();

    setup_heartbeat_generator();
}

auto ocpp_consumer_API::forward_api_var(std::string const& var) {
    using namespace ns_types_ext;
    using namespace generic;
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

void ocpp_consumer_API::generate_api_cmd_data_transfer() {
    using namespace ns_types_ext;
    subscribe_api_topic("data_transfer_outgoing", [=](std::string const& data) {
        generic::RequestReply msg;
        if (deserialize(data, msg)) {
            ns_types_ext::DataTransferRequest request;
            auto payload_ok = deserialize(msg.payload, request);
            if (payload_ok) {
                auto int_reply = r_data_transfer->call_data_transfer(to_internal_api(request));
                auto reply = to_external_api(int_reply);
                mqtt.publish(msg.replyTo, serialize(reply));
                return true;
            }
        }
        return false;
    });
}

void ocpp_consumer_API::generate_api_cmd_get_variables() {
    using namespace ns_types_ext;
    subscribe_api_topic("get_variables", [=](std::string const& data) {
        generic::RequestReply msg;
        if (deserialize(data, msg)) {
            ns_types_ext::GetVariableRequestList request;
            auto payload_ok = deserialize(msg.payload, request);
            if (payload_ok) {
                auto int_reply = r_ocpp->call_get_variables(to_internal_api(request));
                auto reply = to_external_api(int_reply);
                mqtt.publish(msg.replyTo, serialize(reply));
                return true;
            }
        }
        return false;
    });
}

void ocpp_consumer_API::generate_api_cmd_set_variables() {
    using namespace ns_types_ext;
    subscribe_api_topic("set_variables", [=](std::string const& data) {
        generic::RequestReply msg;
        if (deserialize(data, msg)) {
            ns_types_ext::SetVariablesArgs request;
            auto payload_ok = deserialize(msg.payload, request);
            if (payload_ok) {
                auto int_reply = r_ocpp->call_set_variables(to_internal_api(request.variables), request.source);
                auto reply = to_external_api(int_reply);
                mqtt.publish(msg.replyTo, serialize(reply));
                return true;
            }
        }
        return false;
    });
}

void ocpp_consumer_API::generate_api_var_security_event() {
    r_ocpp->subscribe_security_event(forward_api_var("security_event"));
}

void ocpp_consumer_API::generate_api_var_is_connected() {
    r_ocpp->subscribe_is_connected(forward_api_var("is_connected"));
}

void ocpp_consumer_API::generate_api_var_boot_notification_response() {
    r_ocpp->subscribe_boot_notification_response(forward_api_var("boot_notification_response"));
}

void ocpp_consumer_API::generate_api_var_ocpp_transaction_event() {
    r_ocpp->subscribe_ocpp_transaction_event(forward_api_var("ocpp_transaction_event"));
}

void ocpp_consumer_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](const json& data) {
        auto val = generic::deserialize<bool>(data);
        comm_check.set_value(val);
        return true;
    });
}

void ocpp_consumer_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void ocpp_consumer_API::subscribe_api_topic(const std::string& var, const ParseAndPublishFtor& parse_and_publish) {
    auto topic = topics.extern_to_everest(var);
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

const ns_ev_api::Topics& ocpp_consumer_API::get_topics() const {
    return topics;
}

} // namespace module
