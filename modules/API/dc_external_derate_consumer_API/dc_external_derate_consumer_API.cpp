// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "dc_external_derate_consumer_API.hpp"
#include "everest_api_types/dc_external_derate/API.hpp"
#include "everest_api_types/dc_external_derate/codec.hpp"
#include "everest_api_types/dc_external_derate/wrapper.hpp"
#include "everest_api_types/generic/codec.hpp"
#include "everest_api_types/generic/string.hpp"
#include "everest_api_types/utilities/codec.hpp"

namespace module {
namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::dc_external_derate;
namespace API_generic = API_types::generic;
using ev_API::deserialize;

namespace {
double to_external_api(double val) {
    return val;
}
} // namespace

void dc_external_derate_consumer_API::init() {
    invoke_init(*p_generic_error);

    topics.setup(info.id, "dc_external_derate_consumer", 1);
}

void dc_external_derate_consumer_API::ready() {
    invoke_ready(*p_generic_error);

    generate_api_cmd_set_external_derating();
    generate_api_var_plug_temperature_C();

    generate_api_var_communication_check();

    comm_check.start(config.cfg_communication_check_to_s);
    setup_heartbeat_generator();
}

auto dc_external_derate_consumer_API::forward_api_var(std::string const& var) {
    using namespace API_types_ext;
    using namespace API_generic;
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

void dc_external_derate_consumer_API::generate_api_var_plug_temperature_C() {
    r_derate->subscribe_plug_temperature_C(forward_api_var("plug_temperature_C"));
}

void dc_external_derate_consumer_API::generate_api_cmd_set_external_derating() {
    subscribe_api_topic("set_external_derating", [=](std::string const& data) {
        API_types_ext::ExternalDerating external;
        if (deserialize(data, external)) {
            r_derate->call_set_external_derating(to_internal_api(external));
            return true;
        }
        return false;
    });
}

void dc_external_derate_consumer_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        bool val = false;
        if (deserialize(data, val)) {
            comm_check.set_value(val);
            return true;
        }
        return false;
    });
}

void dc_external_derate_consumer_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void dc_external_derate_consumer_API::subscribe_api_topic(std::string const& var,
                                                          ParseAndPublishFtor const& parse_and_publish) {
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

} // namespace module
