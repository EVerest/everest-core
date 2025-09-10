// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "error_history_consumer_API.hpp"
#include <everest_api_types/error_history/API.hpp>
#include <everest_api_types/error_history/codec.hpp>
#include <everest_api_types/error_history/wrapper.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>
#include "error_wrapper.hpp"
#include "generated/types/error_history.hpp"

namespace module {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::error_history;
namespace API_generic = API_types::generic;
using ev_API::deserialize;

void error_history_consumer_API::init() {
    invoke_init(*p_main);
    topics.setTargetApiModuleID(info.id, "error_history_consumer");

    generate_api_cmd_active_errors();
    //    generate_api_var_communication_check();  // TODO(CB): Why not here?
    generate_api_var_error_events();
}

void error_history_consumer_API::ready() {
    invoke_ready(*p_main);

    comm_check.start(config.cfg_communication_check_to_s);
    generate_api_var_communication_check();  // TODO(CB): Why not in ready()?
    setup_heartbeat_generator();
}

auto error_history_consumer_API::forward_api_var(std::string const& var) {
    using namespace API_types_ext;
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

void error_history_consumer_API::generate_api_cmd_active_errors() {
    using namespace API_types_ext;
    subscribe_api_topic("active_errors", [=](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            std::string datetime_str = Everest::Date::to_rfc3339(date::utc_clock::now());
            types::error_history::FilterArguments filter;
            filter.state_filter = types::error_history::State::Active;
            auto active_errors = r_error_history->call_get_errors(filter);
            auto reply = to_external_api(active_errors);
            mqtt.publish(msg.replyTo, serialize(reply));
            return true;
        }
        return false;
    });
}

void error_history_consumer_API::generate_api_var_error_events() {
    auto convert = [](auto const& ftor) {
        return [ftor](auto&& elem) { return ftor(error_converter::framework_to_internal_api(elem)); };
    };
    subscribe_global_all_errors(convert(forward_api_var("error_raised")), convert(forward_api_var("error_cleared")));
}

void error_history_consumer_API::generate_api_var_communication_check() {
    subscribe_api_topic("communication_check", [this](std::string const& data) {
        auto val = API_generic::deserialize<bool>(data);
        comm_check.set_value(val);
        return true;
    });
}

void error_history_consumer_API::setup_heartbeat_generator() {
    auto topic = topics.everest_to_extern("heartbeat");
    auto action = [this, topic]() {
        mqtt.publish(topic, "{}");
        return true;
    };
    comm_check.heartbeat(config.cfg_heartbeat_interval_ms, action);
}

void error_history_consumer_API::subscribe_api_topic(const std::string& var,
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
