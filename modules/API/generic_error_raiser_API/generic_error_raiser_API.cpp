// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "generic_error_raiser_API.hpp"
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/generic/string.hpp>
#include <everest_api_types/utilities/codec.hpp>

namespace module {

using ns_ev_api::deserialize;

void generic_error_raiser_API::init() {
    invoke_init(*p_main);
    topics.setTargetApiModuleID(info.id, "generic_error_raiser");

    generate_api_var_raise_error();
    generate_api_var_clear_error();
}

void generic_error_raiser_API::ready() {
    invoke_ready(*p_main);

    comm_check.start(config.cfg_communication_check_to_s);
    generate_api_var_communication_check();
    setup_heartbeat_generator();
}

void generic_error_raiser_API::generate_api_var_raise_error() {
    subscribe_api_topic("raise_error", [=](const std::string& data) {
        generic::Error error;
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

std::string generic_error_raiser_API::make_error_string(generic::Error const& error) {
    auto error_str = generic::trimmed(serialize(error.type));
    auto result = "generic/" + error_str;
    return result;
}

void generic_error_raiser_API::generate_api_var_clear_error() {
    subscribe_api_topic("clear_error", [=](const std::string& data) {
        generic::Error error;
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

void generic_error_raiser_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        auto val = generic::deserialize<bool>(data);
        comm_check.set_value(val);
        return true;
    });
}

void generic_error_raiser_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void generic_error_raiser_API::subscribe_api_topic(const std::string& var,
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
