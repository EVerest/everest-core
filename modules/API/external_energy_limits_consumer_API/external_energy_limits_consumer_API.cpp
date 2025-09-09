// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "external_energy_limits_consumer_API.hpp"
#include <everest_api_types/energy/API.hpp>
#include <everest_api_types/energy/codec.hpp>
#include <everest_api_types/energy/wrapper.hpp>
#include <everest_api_types/utilities/codec.hpp>

namespace module {
namespace ns_types_ext = everest::lib::API::V1_0::types::energy;
using ns_ev_api::deserialize;

void external_energy_limits_consumer_API::init() {
    invoke_init(*p_main);
    topics.setTargetApiModuleID(info.id, "external_energy_limits_consumer");

    generate_api_var_enforced_limits();
    generate_api_cmd_set_external_limits();
}

void external_energy_limits_consumer_API::ready() {
    invoke_ready(*p_main);

    comm_check.start(config.cfg_communication_check_to_s);
    generate_api_var_communication_check();

    setup_heartbeat_generator();
}

auto external_energy_limits_consumer_API::forward_api_var(std::string const& var) {
    using namespace ns_types_ext;
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

void external_energy_limits_consumer_API::generate_api_var_enforced_limits() {
    r_energy_node->subscribe_enforced_limits(forward_api_var("enforced_limits"));
}

void external_energy_limits_consumer_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void external_energy_limits_consumer_API::generate_api_cmd_set_external_limits() {
    subscribe_api_topic("set_external_limits", [=](std::string const& data) {
        ns_types_ext::ExternalLimits val;
        if (deserialize(data, val)) {
            r_energy_node->call_set_external_limits(to_internal_api(val));
            return true;
        }
        return false;
    });
}

void external_energy_limits_consumer_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void external_energy_limits_consumer_API::subscribe_api_topic(const std::string& var,
                                                              const ParseAndPublishFtor& parse_and_publish) {
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

} // namespace module
