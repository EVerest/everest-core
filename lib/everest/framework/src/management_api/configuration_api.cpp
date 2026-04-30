// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include <configuration_api.hpp>
#include <everest/logging.hpp>
#include <utils/date.hpp>

#include <algorithm>
#include <iterator>

#include <configuration_type_wrapper.hpp>
#include <everest_api_types/configuration/codec.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>

namespace {
template <class SrcT, class ConvT> auto srcToTarVec(std::vector<SrcT> const& src, ConvT const& converter) {
    using TarT = decltype(converter(src[0]));

    std::vector<TarT> result;
    result.reserve(src.size());

    std::transform(src.begin(), src.end(), std::back_inserter(result), converter);

    return result;
}
} // namespace

namespace Everest::api::configuration {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::configuration;
namespace API_generic = API_types::generic;
namespace API_wrapper = ::Everest::api::types::configuration;
using ev_API::deserialize;

ConfigurationAPI::ConfigurationAPI(MQTTAbstraction& mqtt_abstraction,
                                   Everest::config::ConfigServiceInterface& config_service, bool readonly) :
    m_mqtt_abstraction(mqtt_abstraction), m_config_service(config_service), m_readonly(readonly) {
    m_topics.setup("_unused_", "configuration", 0);

    generate_api_cmd_list_all_slots();
    generate_api_cmd_get_active_slot();
    generate_api_cmd_mark_active_slot();

    generate_api_cmd_delete_slot();
    generate_api_cmd_duplicate_slot();
    generate_api_cmd_load_from_yaml();
    generate_api_cmd_set_config_parameters();
    generate_api_cmd_get_configuration();

    generate_api_var_active_slot();
    generate_api_var_config_updates();
}

void ConfigurationAPI::generate_api_cmd_list_all_slots() {
    subscribe_api_topic("list_all_slots", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            auto all_slots = m_config_service.list_all_slots();
            m_mqtt_abstraction.publish(msg.replyTo, serialize(API_wrapper::to_external_api(all_slots)));
            return true;
        }
        EVLOG_warning << "Failed to deserialize list_all_slots request.";
        return false;
    });
}

void ConfigurationAPI::generate_api_cmd_get_active_slot() {
    subscribe_api_topic("get_active_slot", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            auto slot_id = m_config_service.get_active_slot_id();
            API_types_ext::GetActiveSlotIdResult payload{slot_id};
            m_mqtt_abstraction.publish(msg.replyTo, serialize(payload));
            return true;
        }
        EVLOG_warning << "Failed to deserialize get_active_slot request.";
        return false;
    });
}

void ConfigurationAPI::generate_api_cmd_mark_active_slot() {
    subscribe_api_topic("mark_active_slot", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::MarkActiveSlotRequest payload;
            if (deserialize(msg.payload, payload)) {
                if (m_readonly) {
                    API_types_ext::MarkActiveSlotResult response{API_types_ext::MarkActiveSlotResultEnum::Rejected};
                    m_mqtt_abstraction.publish(msg.replyTo, serialize(response));
                } else {
                    auto int_res = m_config_service.mark_active_slot(payload.slot_id);
                    auto ext_res = API_wrapper::to_external_api(int_res);

                    API_types_ext::MarkActiveSlotResult response{ext_res};
                    m_mqtt_abstraction.publish(msg.replyTo, serialize(response));
                }
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize mark_active_slot request.";
        return false;
    });
}

void ConfigurationAPI::generate_api_cmd_delete_slot() {
    subscribe_api_topic("delete_slot", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::DeleteSlotRequest payload;
            if (deserialize(msg.payload, payload)) {
                if (m_readonly) {
                    API_types_ext::DeleteSlotResult response{API_types_ext::DeleteSlotResultEnum::Rejected};
                    m_mqtt_abstraction.publish(msg.replyTo, serialize(response));
                } else {
                    auto int_res = m_config_service.delete_slot(payload.slot_id);
                    auto ext_res = API_wrapper::to_external_api(int_res);

                    API_types_ext::DeleteSlotResult response{ext_res};
                    m_mqtt_abstraction.publish(msg.replyTo, serialize(response));
                }
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize delete_slot request.";
        return false;
    });
}

void ConfigurationAPI::generate_api_cmd_duplicate_slot() {
    subscribe_api_topic("duplicate_slot", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::DuplicateSlotRequest payload;
            if (deserialize(msg.payload, payload)) {
                if (m_readonly) {
                    API_types_ext::DuplicateSlotResult response{false, std::nullopt};
                    m_mqtt_abstraction.publish(msg.replyTo, serialize(response));
                } else {
                    auto res = m_config_service.duplicate_slot(payload.slot_id, payload.new_description);
                    auto ext_res = API_wrapper::to_external_api(res);

                    m_mqtt_abstraction.publish(msg.replyTo, serialize(ext_res));
                }
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize duplicate_slot request.";
        return false;
    });
}

void ConfigurationAPI::generate_api_cmd_load_from_yaml() {
    subscribe_api_topic("load_from_yaml", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::LoadFromYamlRequest payload;
            if (deserialize(msg.payload, payload)) {
                if (m_readonly) {
                    API_types_ext::LoadFromYamlResult response{false, "Not Allowed", std::nullopt};
                    m_mqtt_abstraction.publish(msg.replyTo, serialize(response));
                } else {
                    std::optional<int> slot_id_opt =
                        payload.slot_id.has_value() ? std::optional<int>{payload.slot_id.value()} : std::nullopt;
                    auto res = m_config_service.load_from_yaml(payload.raw_yaml, payload.description, slot_id_opt);
                    auto ext_res = API_wrapper::to_external_api(res);

                    m_mqtt_abstraction.publish(msg.replyTo, serialize(ext_res));
                }
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize load_from_yaml request.";
        return false;
    });
}

void ConfigurationAPI::generate_api_cmd_set_config_parameters() {
    subscribe_api_topic("set_config_parameters", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::ConfigurationParameterUpdateRequest payload;
            if (deserialize(msg.payload, payload)) {
                if (m_readonly) {
                    API_types_ext::ConfigurationParameterUpdateRequestResult response;
                    for (const auto& update : payload.parameter_updates) {
                        response.results.push_back(API_types_ext::ConfigurationParameterUpdateResultEnum::Rejected);
                    }
                    m_mqtt_abstraction.publish(msg.replyTo, serialize(response));
                } else {
                    std::vector<Everest::config::ConfigParameterUpdate> updates_internal = srcToTarVec(
                        payload.parameter_updates, [](const API_types_ext::ConfigurationParameterUpdate& update_ext) {
                            return API_wrapper::to_internal_api(update_ext);
                        });

                    auto int_res = m_config_service.set_config_parameters(payload.slot_id, updates_internal);

                    API_types_ext::ConfigurationParameterUpdateRequestResult response{};
                    response.results =
                        srcToTarVec(int_res, [](const auto& result) { return API_wrapper::to_external_api(result); });
                    m_mqtt_abstraction.publish(msg.replyTo, serialize(response));
                }
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize set_config_parameters request.";
        return false;
    });
}

void ConfigurationAPI::generate_api_cmd_get_configuration() {
    subscribe_api_topic("get_configuration", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::GetConfigurationRequest payload;
            if (deserialize(msg.payload, payload)) {
                auto res = m_config_service.get_configuration(payload.slot_id);
                auto ext_res = API_wrapper::to_external_api(res);

                m_mqtt_abstraction.publish(msg.replyTo, serialize(ext_res));
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize get_configuration request.";
        return false;
    });
}

void ConfigurationAPI::generate_api_var_active_slot() {
    // TODO(CB): The configuration does not offer to query the status of the active slot,
    // so for now we just set it to "Running" here, but we might want to add a query function to the config_service
    API_types_ext::ActiveSlotUpdateNotice initial_update{};
    initial_update.tstamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    initial_update.slot_id = m_config_service.get_active_slot_id();
    initial_update.status = API_types_ext::ActiveSlotStatusEnum::Running;
    m_mqtt_abstraction.publish(m_topics.nonmodule_to_extern("active_slot"), serialize(initial_update));

    m_config_service.register_active_slot_update_handler([this](const Everest::config::ActiveSlotUpdate& update) {
        auto ext_update = API_wrapper::to_external_api(update);
        m_mqtt_abstraction.publish(m_topics.nonmodule_to_extern("active_slot"), serialize(ext_update));
    });
}

void ConfigurationAPI::generate_api_var_config_updates() {
    m_config_service.register_config_update_handler([this](const Everest::config::ConfigurationUpdate& update) {
        auto ext_update = API_wrapper::to_external_api(update);
        m_mqtt_abstraction.publish(m_topics.nonmodule_to_extern("config_updates"), serialize(ext_update));
    });
}

void ConfigurationAPI::subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish) {
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
} // namespace Everest::api::configuration
