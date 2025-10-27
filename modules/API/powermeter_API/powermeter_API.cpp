// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "powermeter_API.hpp"

#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/generic/string.hpp>
#include <everest_api_types/powermeter/API.hpp>
#include <everest_api_types/powermeter/codec.hpp>
#include <everest_api_types/powermeter/wrapper.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include <utility>

#include <everest/logging.hpp>

namespace module {

namespace ev_API_types = ev_API::V1_0::types;
namespace API_types_ext = ev_API_types::powermeter;
namespace API_generic = ev_API_types::generic;
using ev_API::deserialize;

void powermeter_API::init() {
    invoke_init(*p_main);

    topics.setup(info.id, "powermeter", 1);
}

void powermeter_API::ready() {
    invoke_ready(*p_main);

    generate_api_var_powermeter_values();
    generate_api_var_public_key_ocmf();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void powermeter_API::generate_api_var_powermeter_values() {
    subscribe_api_topic("powermeter_values", [this](std::string const& data) {
        API_types_ext::PowermeterValues payload;
        if (deserialize(data, payload)) {
            p_main->publish_powermeter(to_internal_api(payload));
            return true;
        }
        return false;
    });
}

void powermeter_API::generate_api_var_public_key_ocmf() {
    subscribe_api_topic("public_key_ocmf", [this](std::string const& data) {
        std::string val;
        if (deserialize(data, val)) {
            p_main->publish_public_key_ocmf(std::move(val));
            return true;
        }
        return false;
    });
}

void powermeter_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void powermeter_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void powermeter_API::subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish) {
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

const ev_API::Topics& powermeter_API::get_topics() const {
    return topics;
}

} // namespace module
