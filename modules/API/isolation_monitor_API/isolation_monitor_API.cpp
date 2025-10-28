// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "isolation_monitor_API.hpp"

#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/generic/string.hpp>
#include <everest_api_types/isolation_monitor/API.hpp>
#include <everest_api_types/isolation_monitor/codec.hpp>
#include <everest_api_types/isolation_monitor/wrapper.hpp>
#include <everest_api_types/utilities/codec.hpp>

namespace module {

namespace API_generic = API_types::generic;
using ev_API::deserialize;

void isolation_monitor_API::init() {
    invoke_init(*p_main);

    topics.setup(info.id, "isolation_monitor", 1);
}

void isolation_monitor_API::ready() {
    invoke_ready(*p_main);

    generate_api_var_isolation_measurement();
    generate_api_var_self_test_result();

    generate_api_var_raise_error();
    generate_api_var_clear_error();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void isolation_monitor_API::generate_api_var_isolation_measurement() {
    subscribe_api_topic("isolation_measurement", [=](std::string const& data) {
        API_types_ext::IsolationMeasurement payload;
        if (deserialize(data, payload)) {
            p_main->publish_isolation_measurement(to_internal_api(payload));
            return true;
        }
        return false;
    });
}

void isolation_monitor_API::generate_api_var_self_test_result() {
    subscribe_api_topic("self_test_result", [=](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            p_main->publish_self_test_result(val);
            return true;
        }
        return false;
    });
}

void isolation_monitor_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void isolation_monitor_API::generate_api_var_raise_error() {
    subscribe_api_topic("raise_error", [=](std::string const& data) {
        API_types_ext::Error error;
        if (deserialize(data, error)) {
            auto sub_type_str = error.sub_type ? error.sub_type.value() : "";
            auto message_str = error.message ? error.message.value() : "";
            auto error_str = make_error_string(error);
            auto ev_error = p_main->error_factory->create_error(error_str, sub_type_str, message_str,
                                                                Everest::error::Severity::High);
            p_main->raise_error(ev_error);
            return true;
        }
        return false;
    });
}

void isolation_monitor_API::generate_api_var_clear_error() {
    subscribe_api_topic("clear_error", [=](std::string const& data) {
        API_types_ext::Error error;
        if (deserialize(data, error)) {
            std::string error_str = make_error_string(error);
            if (error.sub_type) {
                p_main->clear_error(error_str, error.sub_type.value());
            } else {
                p_main->clear_error(error_str);
            }
            return true;
        }
        return false;
    });
}

void isolation_monitor_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void isolation_monitor_API::subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish) {
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

std::string isolation_monitor_API::make_error_string(API_types_ext::Error const& error) {
    auto error_str = API_generic::trimmed(serialize(error.type));
    auto result = "isolation_monitor/" + error_str;
    return result;
}

const ev_API::Topics& isolation_monitor_API::get_topics() const {
    return topics;
}

} // namespace module
