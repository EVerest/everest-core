// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "power_supply_DC_API.hpp"

#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/generic/string.hpp>
#include <everest_api_types/power_supply_DC/API.hpp>
#include <everest_api_types/power_supply_DC/codec.hpp>
#include <everest_api_types/power_supply_DC/wrapper.hpp>
#include <everest_api_types/utilities/Topics.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include "utils/error.hpp"
#include <everest/logging.hpp>

namespace module {

namespace API_generic = API_types::generic;
using ev_API::deserialize;

void power_supply_DC_API::init() {
    invoke_init(*p_main);

    topics.setup(info.id, "power_supply_DC", 1);
}

void power_supply_DC_API::ready() {
    invoke_ready(*p_main);

    generate_api_var_mode();
    generate_api_var_voltage_current();
    generate_api_var_capabilities();

    generate_api_var_raise_error();
    generate_api_var_clear_error();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void power_supply_DC_API::generate_api_var_mode() {
    subscribe_api_topic("mode", [this](const std::string& data) {
        API_types_ext::Mode payload;
        if (deserialize(data, payload)) {
            p_main->publish_mode(to_internal_api(payload));
            return true;
        }
        return false;
    });
}

void power_supply_DC_API::generate_api_var_voltage_current() {
    subscribe_api_topic("voltage_current", [this](const std::string& data) {
        API_types_ext::VoltageCurrent payload;
        if (deserialize(data, payload)) {
            p_main->publish_voltage_current(to_internal_api(payload));
            return true;
        }
        return false;
    });
}

void power_supply_DC_API::generate_api_var_capabilities() {
    subscribe_api_topic("capabilities", [this](const std::string& data) {
        API_types_ext::Capabilities payload;
        if (deserialize(data, payload)) {
            p_main->publish_capabilities(to_internal_api(payload));
            return true;
        }
        return false;
    });
}

void power_supply_DC_API::generate_api_var_raise_error() {
    subscribe_api_topic("raise_error", [this](const std::string& data) {
        API_types_ext::Error payload;
        if (deserialize(data, payload)) {
            auto sub_type_str = payload.sub_type ? payload.sub_type.value() : "";
            auto message_str = payload.message ? payload.message.value() : "";
            auto error_str = make_error_string(payload);
            auto ev_error = p_main->error_factory->create_error(error_str, sub_type_str, message_str,
                                                                Everest::error::Severity::High);
            p_main->raise_error(ev_error);
            return true;
        }
        return false;
    });
}

void power_supply_DC_API::generate_api_var_clear_error() {
    subscribe_api_topic("clear_error", [this](const std::string& data) {
        API_types_ext::Error payload;
        if (deserialize(data, payload)) {
            std::string error_str = make_error_string(payload);
            if (payload.sub_type) {
                p_main->clear_error(error_str, payload.sub_type.value());
            } else {
                p_main->clear_error(error_str);
            }
            return true;
        }
        return false;
    });
}

std::string power_supply_DC_API::make_error_string(API_types_ext::Error const& error) {
    auto error_str = API_generic::trimmed(serialize(error.type));
    auto result = "power_supply_DC/" + error_str;
    return result;
}

void power_supply_DC_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void power_supply_DC_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, API_generic::serialize(hb_id++));
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void power_supply_DC_API::subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish) {
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

const ev_API::Topics& power_supply_DC_API::get_topics() const {
    return topics;
}

} // namespace module
