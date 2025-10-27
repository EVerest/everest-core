// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "system_API.hpp"

#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/generic/string.hpp>
#include <everest_api_types/system/API.hpp>
#include <everest_api_types/system/codec.hpp>
#include <everest_api_types/system/wrapper.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include <everest/logging.hpp>

namespace module {

namespace ev_API_v = ev_API::V1_0;
namespace API_types_ext = ev_API_v::types::system;
namespace API_generic = ev_API_v::types::generic;
using ev_API::deserialize;

void system_API::init() {
    invoke_init(*p_main);

    topics.setup(info.id, "system", 1);
}

void system_API::ready() {
    invoke_ready(*p_main);

    generate_api_var_firmware_update_status();
    generate_api_var_log_status();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void system_API::generate_api_var_firmware_update_status() {
    subscribe_api_topic("firmware_update_status", [this](std::string const& data) {
        API_types_ext::FirmwareUpdateStatus payload;
        if (deserialize(data, payload)) {
            p_main->publish_firmware_update_status(to_internal_api(payload));
            return true;
        }
        return false;
    });
}

void system_API::generate_api_var_log_status() {
    subscribe_api_topic("log_status", [this](std::string const& data) {
        API_types_ext::LogStatus payload;
        if (deserialize(data, payload)) {
            p_main->publish_log_status(to_internal_api(payload));
            return true;
        }
        return false;
    });
}

void system_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void system_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void system_API::subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish) {
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

const ev_API::Topics& system_API::get_topics() const {
    return topics;
}

} // namespace module
