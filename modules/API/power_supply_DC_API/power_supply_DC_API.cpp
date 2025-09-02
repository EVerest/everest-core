// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "power_supply_DC_API.hpp"
#include "everest_api_types/generic/codec.hpp"
#include "everest_api_types/generic/string.hpp"
#include "everest_api_types/power_supply_DC/API.hpp"
#include "everest_api_types/power_supply_DC/codec.hpp"
#include "everest_api_types/power_supply_DC/wrapper.hpp"
#include "everest_api_types/utilities/Topics.hpp"
#include "everest/logging.hpp"
#include "utils/error.hpp"
#include <optional>

namespace module {

namespace generic = everest::lib::API::V1_0::types::generic;
using ns_types_ext::deserialize;

void power_supply_DC_API::init() {
    invoke_init(*p_if_power_supply_DC);

    topics.setTargetApiModuleID(info.id, "power_supply_DC");

    generate_api_var_mode();
    generate_api_var_voltage_current();
    generate_api_var_capabilities();

    generate_api_var_raise_error();
    generate_api_var_clear_error();
}

void power_supply_DC_API::ready() {
    invoke_ready(*p_if_power_supply_DC);
    comm_check.start(config.cfg_communication_check_to_s);
    generate_api_var_communication_check();

    setup_heartbeat_generator();
}

void power_supply_DC_API::generate_api_var_mode() {
    subscribe_api_var("mode", [=](const std::string& data) {
        auto ext = deserialize<ns_types_ext::Mode>(data);
        auto arg = toInternalApi(ext);
        p_if_power_supply_DC->publish_mode(arg);
    });
}

void power_supply_DC_API::generate_api_var_voltage_current() {
    subscribe_api_var("voltage_current", [=](const std::string& data) {
        auto ext = deserialize<ns_types_ext::VoltageCurrent>(data);
        auto arg = toInternalApi(ext);
        p_if_power_supply_DC->publish_voltage_current(arg);
    });
}

void power_supply_DC_API::generate_api_var_capabilities() {
    subscribe_api_var("capabilities", [=](const json& data) {
        auto ext = deserialize<ns_types_ext::Capabilities>(data);
        auto arg = toInternalApi(ext);
        p_if_power_supply_DC->publish_capabilities(arg);
    });
}

void power_supply_DC_API::generate_api_var_raise_error() {
    subscribe_api_var("raise_error", [=](const std::string& data) {
        auto error = deserialize<ns_types_ext::Error>(data);
        auto sub_type_str = error.sub_type ? error.sub_type.value() : "";
        auto message_str = error.message ? error.message.value() : "";
        auto error_str = make_error_string(error);
        auto ev_error = p_if_power_supply_DC->error_factory->create_error(error_str, sub_type_str, message_str,
                                                                          Everest::error::Severity::High);
        p_if_power_supply_DC->raise_error(ev_error);
    });
}

void power_supply_DC_API::generate_api_var_clear_error() {
    subscribe_api_var("clear_error", [=](const std::string& data) {
        auto error = deserialize<ns_types_ext::Error>(data);
        std::string error_str = make_error_string(error);
        if (error.sub_type) {
            p_if_power_supply_DC->clear_error(error_str, error.sub_type.value());
        } else {
            p_if_power_supply_DC->clear_error(error_str);
        }
    });
}

void power_supply_DC_API::generate_api_var_communication_check() {
    subscribe_api_var("communication_check", [this](const std::string& data) {
        auto val = generic::deserialize<bool>(data);
        comm_check.set_value(val);
    });
}

std::string power_supply_DC_API::make_error_string(ns_types_ext::Error const& error) {
    auto error_str = generic::trimmed(serialize(error.type));
    auto result = "power_supply_DC/" + error_str;
    return result;
}

void power_supply_DC_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void power_supply_DC_API::subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish) {
    auto topic = topics.extern_to_everest(var);
    mqtt.subscribe(topic, [=](std::string const& data) {
        try {
            parse_and_publish(data);
        } catch (const std::exception& e) {
            EVLOG_warning << "Variable: '" << topic << "' failed with -> " << e.what() << "\n => " << data;
        } catch (...) {
            EVLOG_warning << "Invalid data: Failed to parse JSON or to get data from it.\n" << topic;
        }
    });
}

const ns_bc::Topics& power_supply_DC_API::get_topics() const {
    return topics;
}

} // namespace module
