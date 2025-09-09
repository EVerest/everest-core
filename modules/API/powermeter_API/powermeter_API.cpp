// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "powermeter_API.hpp"
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/generic/string.hpp>
#include <everest_api_types/powermeter/API.hpp>
#include <everest_api_types/powermeter/codec.hpp>
#include <everest_api_types/powermeter/wrapper.hpp>
#include <everest_api_types/utilities/codec.hpp>
#include "everest/logging.hpp"

namespace module {

namespace ns_ev_api = everest::lib::API;
namespace ns_types_ext = ns_ev_api::V1_0::types::powermeter;
namespace generic = ns_ev_api::V1_0::types::generic;
using ns_ev_api::deserialize;

void powermeter_API::init() {
    invoke_init(*p_if_powermeter);

    topics.setTargetApiModuleID(info.id, "powermeter");
    generate_api_var_powermeter_values();
    generate_api_var_public_key_ocmf();

    generate_api_var_communication_check();
}

void powermeter_API::ready() {
    invoke_ready(*p_if_powermeter);
    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

void powermeter_API::generate_api_var_powermeter_values() {
    subscribe_api_var("powermeter_values", [=](std::string const& data) {
        ns_types_ext::PowermeterValues ext;
        if (deserialize(data, ext)) {
            auto value = to_internal_api(ext);
            p_if_powermeter->publish_powermeter(value);
        }
    });
}

void powermeter_API::generate_api_var_public_key_ocmf() {
    subscribe_api_var("public_key_ocmf", [=](std::string const& data) {
        std::string ext;
        if (deserialize(data, ext)) {
            p_if_powermeter->publish_public_key_ocmf(ext);
        }
    });
}

void powermeter_API::generate_api_var_communication_check() {
    subscribe_api_var("communication_check", [this](std::string const& data) {
        auto val = generic::deserialize<bool>(data);
        comm_check.set_value(val);
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

void powermeter_API::subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish) {
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

const ns_ev_api::Topics& powermeter_API::get_topics() const {
    return topics;
}

} // namespace module
