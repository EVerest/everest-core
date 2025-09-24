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

    topics.setTargetApiModuleID(info.id, "system");
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
    subscribe_api_var("firmware_update_status", [=](std::string const& data) {
        API_types_ext::FirmwareUpdateStatus ext;
        if (deserialize(data, ext)) {
            auto value = to_internal_api(ext);
            p_main->publish_firmware_update_status(value);
        }
    });
}

void system_API::generate_api_var_log_status() {
    subscribe_api_var("log_status", [=](std::string const& data) {
        API_types_ext::LogStatus ext;
        if (deserialize(data, ext)) {
            auto value = to_internal_api(ext);
            p_main->publish_log_status(value);
        }
    });
}

void system_API::generate_api_var_communication_check() {
    subscribe_api_var("communication_check", [this](std::string const& data) {
        auto val = API_generic::deserialize<bool>(data);
        comm_check.set_value(val);
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

void system_API::subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish) {
    auto topic = topics.extern_to_everest(var);
    mqtt.subscribe(topic, [=](std::string const& data) {
        try {
            parse_and_publish(data);
        } catch (const std::exception& e) {
            EVLOG_warning << "Variable: '" << topic << "' failed with -> " << e.what() << "\n => " << data;
            ;
        } catch (...) {
            EVLOG_warning << "Invalid data: Failed to parse JSON or to get data from it.\n" << topic;
        }
    });
}

const ev_API::Topics& system_API::get_topics() const {
    return topics;
}

} // namespace module
