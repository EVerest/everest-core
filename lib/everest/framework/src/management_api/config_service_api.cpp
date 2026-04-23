// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include <config_service_api.hpp>
#include <everest/logging.hpp>
#include <utils/date.hpp>

#include <algorithm>
#include <iterator>

#include <config_service_type_wrapper.hpp>
#include <everest_api_types/config_service/codec.hpp>
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

namespace Everest::api::config_service {

namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::config_service;
namespace API_generic = API_types::generic;
namespace API_wrapper = ::Everest::api::types::config_service;
using ev_API::deserialize;

ConfigServiceAPI::ConfigServiceAPI(MQTTAbstraction& mqtt_abstraction,
                                   Everest::config::ConfigServiceInterface& config_service) :
    mqtt_abstraction(mqtt_abstraction), config_service(config_service) {
    topics.setup("_unused_", "config_service", 0);

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

void ConfigServiceAPI::generate_api_cmd_list_all_slots() {
    subscribe_api_topic("list_all_slots", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            auto all_slots = config_service.list_all_slots();
            mqtt_abstraction.publish(msg.replyTo, serialize(API_wrapper::to_external_api(all_slots)));
            return true;
        }
        EVLOG_warning << "Failed to deserialize list_all_slots request.";
        return false;
    });
}

void ConfigServiceAPI::generate_api_cmd_get_active_slot() {
    subscribe_api_topic("get_active_slot", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            auto slot_id = config_service.get_active_slot_id();
            API_types_ext::GetActiveSlotIdResult payload{slot_id};
            mqtt_abstraction.publish(msg.replyTo, serialize(payload));
            return true;
        }
        EVLOG_warning << "Failed to deserialize get_active_slot request.";
        return false;
    });
}

void ConfigServiceAPI::generate_api_cmd_mark_active_slot() {
    subscribe_api_topic("mark_active_slot", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::MarkActiveSlotRequest payload;
            if (deserialize(msg.payload, payload)) {
                auto int_res = config_service.mark_active_slot(payload.slot_id);
                auto ext_res = API_wrapper::to_external_api(int_res);

                API_types_ext::MarkActiveSlotResult response{ext_res};
                mqtt_abstraction.publish(msg.replyTo, serialize(response));
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize mark_active_slot request.";
        return false;
    });
}

void ConfigServiceAPI::generate_api_cmd_delete_slot() {
    subscribe_api_topic("delete_slot", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::DeleteSlotRequest payload;
            if (deserialize(msg.payload, payload)) {
                auto int_res = config_service.delete_slot(payload.slot_id);
                auto ext_res = API_wrapper::to_external_api(int_res);

                API_types_ext::DeleteSlotResult response{ext_res};
                mqtt_abstraction.publish(msg.replyTo, serialize(response));
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize delete_slot request.";
        return false;
    });
}

void ConfigServiceAPI::generate_api_cmd_duplicate_slot() {
    subscribe_api_topic("duplicate_slot", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::DuplicateSlotRequest payload;
            if (deserialize(msg.payload, payload)) {
                auto res = config_service.duplicate_slot(payload.slot_id, payload.new_description);
                auto ext_res = API_wrapper::to_external_api(res);

                mqtt_abstraction.publish(msg.replyTo, serialize(ext_res));
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize duplicate_slot request.";
        return false;
    });
}

void ConfigServiceAPI::generate_api_cmd_load_from_yaml() {
    subscribe_api_topic("load_from_yaml", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::LoadFromYamlRequest payload;
            if (deserialize(msg.payload, payload)) {
                auto res = config_service.load_from_yaml(payload.raw_yaml, payload.description);
                auto ext_res = API_wrapper::to_external_api(res);

                mqtt_abstraction.publish(msg.replyTo, serialize(ext_res));
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize load_from_yaml request.";
        return false;
    });
}

void ConfigServiceAPI::generate_api_cmd_set_config_parameters() {
    subscribe_api_topic("set_config_parameters", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::ConfigurationParameterUpdateRequest payload;
            if (deserialize(msg.payload, payload)) {
                std::vector<Everest::config::ConfigParameterUpdate> updates_internal = srcToTarVec(
                    payload.parameter_updates, [](const API_types_ext::ConfigurationParameterUpdate& update_ext) {
                        return API_wrapper::to_internal_api(update_ext);
                    });

                auto int_res = config_service.set_config_parameters(payload.slot_id, updates_internal);

                API_types_ext::ConfigurationParameterUpdateRequestResult response{};
                response.results =
                    srcToTarVec(int_res, [](const auto& result) { return API_wrapper::to_external_api(result); });
                mqtt_abstraction.publish(msg.replyTo, serialize(response));
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize set_config_parameters request.";
        return false;
    });
}

void ConfigServiceAPI::generate_api_cmd_get_configuration() {
    subscribe_api_topic("get_configuration", [this](std::string const& data) {
        API_generic::RequestReply msg;
        if (deserialize(data, msg)) {
            API_types_ext::GetConfigurationRequest payload;
            if (deserialize(msg.payload, payload)) {
                auto res = config_service.get_configuration(payload.slot_id);
                auto ext_res = API_wrapper::to_external_api(res);

                mqtt_abstraction.publish(msg.replyTo, serialize(ext_res));
                return true;
            }
        }
        EVLOG_warning << "Failed to deserialize get_configuration request.";
        return false;
    });
}

void ConfigServiceAPI::generate_api_var_active_slot() {
    // TODO(CB): The config_service does not offer to query the status of the active slot,
    // so for now we just set it to "Running" here, but we might want to add a query function to the config_service
    API_types_ext::ActiveSlotUpdateNotice initial_update{};
    initial_update.tstamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    initial_update.slot_id = config_service.get_active_slot_id();
    initial_update.status = API_types_ext::ActiveSlotStatusEnum::Running;
    mqtt_abstraction.publish(topics.nonmodule_to_extern("active_slot"), serialize(initial_update));

    config_service.register_active_slot_update_handler([this](const Everest::config::ActiveSlotUpdate& update) {
        auto ext_update = API_wrapper::to_external_api(update);
        mqtt_abstraction.publish(topics.nonmodule_to_extern("active_slot"), serialize(ext_update));
    });
}

void ConfigServiceAPI::generate_api_var_config_updates() {
    config_service.register_config_update_handler([this](const Everest::config::ConfigurationUpdate& update) {
        auto ext_update = API_wrapper::to_external_api(update);
        mqtt_abstraction.publish(topics.nonmodule_to_extern("config_updates"), serialize(ext_update));
    });
}

void ConfigServiceAPI::subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish) {
    auto topic = topics.extern_to_nonmodule(var);
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
    mqtt_abstraction.register_handler(topic, handler, QOS::QOS2);
}
} // namespace Everest::api::config_service