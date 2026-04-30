// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "include/lifecycle_api.hpp"
#include <everest/logging.hpp>
#include <utils/date.hpp>

#include <algorithm>
#include <iterator>

#include "include/lifecycle_type_wrapper.hpp"
#include <everest_api_types/lifecycle/codec.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>

namespace Everest::api::lifecycle {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::lifecycle;
namespace API_generic = API_types::generic;
namespace API_wrapper = ::Everest::api::types::lifecycle;
using ev_API::deserialize;

LifecycleAPI::LifecycleAPI(MQTTAbstraction& mqtt_abstraction, ::Everest::config::ConfigServiceInterface& config_service,
                           bool config_api_enabled) :
    m_mqtt_abstraction(mqtt_abstraction), m_config_service(config_service), m_config_api_enabled(config_api_enabled) {

    m_topics.setup("_unused_", "lifecycle", 0);

    generate_api_cmd_stop_modules();
    generate_api_cmd_start_modules();

    generate_api_var_status();
}

void LifecycleAPI::generate_api_cmd_stop_modules() {
    subscribe_api_topic("stop_modules", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            auto res = m_config_service.stop_modules();
            API_types_ext::StopModulesResult ext_res{API_wrapper::to_external_api(res)};
            m_mqtt_abstraction.publish(msg.replyTo, serialize(ext_res));
            return true;
        }
        EVLOG_warning << "Failed to deserialize stop_modules request.";
        return false;
    });
}

void LifecycleAPI::generate_api_cmd_start_modules() {
    subscribe_api_topic("start_modules", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            auto res = m_config_service.restart_modules();
            API_types_ext::StartModulesResult ext_res{API_wrapper::to_external_api(res)};
            m_mqtt_abstraction.publish(msg.replyTo, serialize(ext_res));
            return true;
        }
        EVLOG_warning << "Failed to deserialize start_modules request.";
        return false;
    });
}

void LifecycleAPI::generate_api_var_status() {
    auto topic = m_topics.extern_to_nonmodule("status");

    API_types_ext::ExecutionStatusUpdateNotice initial_update{};
    initial_update.tstamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    initial_update.status =
        API_types_ext::ModuleExecutionStatusEnum::Running; // TODO(CB): This should be queried from the config_service,
                                                           // but it currently does not offer this

    EVLOG_warning << "Status topic: " << topic;
    m_mqtt_abstraction.publish(topic, serialize(initial_update), QOS::QOS2, false);

    // TODO(CB): register a callback somewhere and publish whenever the modules start/stop

    // TODO(CB): This topics should be written to on disconnect via LWT, but the mqtt abstraction currently does not
    // offer this
}

void LifecycleAPI::subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish) {
    auto topic = m_topics.extern_to_nonmodule(var);
    auto handler = std::make_shared<TypedHandler>(
        HandlerType::ExternalMQTT, std::make_shared<Handler>([=](std::string const& topic, nlohmann::json data) {
            try {
                if (not parse_and_publish(data)) {
                    EVLOG_warning << "Invalid data: Deserialization failed.\n" << topic << "\n" << data;
                }
            } catch (const std::exception& e) {
                EVLOG_warning << "Topic: '" << topic << "' failed with -> " << e.what() << "\n => " << data;
            } catch (...) {
                EVLOG_warning << "Invalid data: Failed to parse JSON or to get data from it.\n" << topic;
            }
        }));
    m_mqtt_abstraction.register_handler(topic, handler, QOS::QOS2);
}
} // namespace Everest::api::lifecycle
