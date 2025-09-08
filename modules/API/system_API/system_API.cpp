// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "system_API.hpp"
#include "basecamp/generic/codec.hpp"
#include "basecamp/generic/string.hpp"
#include "basecamp/system/API.hpp"
#include "basecamp/system/codec.hpp"
#include "basecamp/system/wrapper.hpp"
#include "basecamp/utilities/codec.hpp"
#include "everest/logging.hpp"

namespace module {

namespace ns_types_ext = basecamp::API::V1_0::types::system;
namespace generic = basecamp::API::V1_0::types::generic;
using basecamp::API::deserialize;

void system_API::init() {
    invoke_init(*p_if_system);
    topics.setTargetApiModuleID(info.id, "system");

    generate_api_var_firmware_update_status();
    generate_api_var_log_status();

    generate_api_var_communication_check();
}

void system_API::ready() {
    invoke_ready(*p_if_system);
    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void system_API::generate_api_var_firmware_update_status() {
    subscribe_api_var("firmware_update_status", [=](std::string const& data) {
        ns_types_ext::FirmwareUpdateStatus ext;
        if (deserialize(data, ext)) {
            auto value = toInternalApi(ext);
            p_if_system->publish_firmware_update_status(value);
        }
    });
}

void system_API::generate_api_var_log_status() {
    subscribe_api_var("log_status", [=](std::string const& data) {
        ns_types_ext::LogStatus ext;
        if (deserialize(data, ext)) {
            auto value = toInternalApi(ext);
            p_if_system->publish_log_status(value);
        }
    });
}

void system_API::generate_api_var_communication_check() {
    subscribe_api_var("communication_check", [this](std::string const& data) {
        auto val = generic::deserialize<bool>(data);
        comm_check.set_value(val);
    });
}

void system_API::setup_heartbeat_generator() {
    auto topic = topics.basecamp_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void system_API::subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish) {
    auto topic = topics.extern_to_basecamp(var);
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

const ns_bc::Topics& system_API::get_topics() const {
    return topics;
}

} // namespace module
