// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <future>

#include <fmt/core.h>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <utils/config_service.hpp>
#include <utils/module_config.hpp>
#include <utils/types.hpp>

namespace Everest {
using json = nlohmann::json;

namespace {
// TODO: make this use the ConfigServiceClient as well?
// TODO: needs changes in the bindings to support a new get_module_config call
json get_definitions(std::shared_ptr<MQTTAbstraction> mqtt) {
    const auto& everest_prefix = mqtt->get_everest_prefix();

    json result;
    const auto interface_names_topic = fmt::format("{}interfaces", everest_prefix);
    const auto interface_names = mqtt->get(interface_names_topic, QOS::QOS2, config::mqtt_get_config_retries);
    auto interface_definitions = json::object();
    for (const auto& interface : interface_names) {
        auto interface_topic = fmt::format("{}interface_definitions/{}", everest_prefix, interface.get<std::string>());
        auto interface_definition = mqtt->get(interface_topic, QOS::QOS2, config::mqtt_get_config_retries);
        interface_definitions[interface] = interface_definition;
    }

    result["interface_definitions"] = interface_definitions;

    const auto type_names_topic = fmt::format("{}types", everest_prefix);
    const auto type_names = mqtt->get(type_names_topic, QOS::QOS2, config::mqtt_get_config_retries);
    auto type_definitions = json::object();
    for (const auto& type_name : type_names) {
        // type_definition keys already start with a / so omit it in the topic name
        auto type_topic = fmt::format("{}type_definitions{}", everest_prefix, type_name.get<std::string>());
        auto type_definition = mqtt->get(type_topic, QOS::QOS2, config::mqtt_get_config_retries);
        type_definitions[type_name] = type_definition;
    }

    result["types"] = type_definitions;

    const auto settings_topic = fmt::format("{}settings", everest_prefix);
    const auto settings = mqtt->get(settings_topic, QOS::QOS2, config::mqtt_get_config_retries);
    result["settings"] = settings;

    const auto validate_schema = settings.value("validate_schema", json(false)).get<bool>();
    if (validate_schema) {
        const auto schemas_topic = fmt::format("{}schemas", everest_prefix);
        const auto schemas = mqtt->get(schemas_topic, QOS::QOS2, config::mqtt_get_config_retries);
        result["schemas"] = schemas;
    }

    const auto module_names_topic = fmt::format("{}module_names", everest_prefix);
    const auto module_names = mqtt->get(module_names_topic, QOS::QOS2, config::mqtt_get_config_retries);
    result["module_names"] = module_names;

    auto manifests = json::object();
    for (const auto& module_name : module_names) {
        auto manifest_topic = fmt::format("{}manifests/{}", everest_prefix, module_name.get<std::string>());
        auto manifest = mqtt->get(manifest_topic, QOS::QOS2, config::mqtt_get_config_retries);
        manifests[module_name] = manifest;
    }

    result["manifests"] = manifests;

    return result;
}
} // namespace

json get_module_config(std::shared_ptr<MQTTAbstraction> mqtt, const std::string& module_id) {
    const auto& everest_prefix = mqtt->get_everest_prefix();

    config::GetRequest get_request;
    get_request.type = config::GetType::Module;
    config::Request request;
    request.request = get_request;
    request.origin = module_id;
    request.type = config::Type::Get;

    MQTTRequest mqtt_request;
    mqtt_request.response_topic = fmt::format("{}modules/{}/response", everest_prefix, module_id);
    mqtt_request.request_topic = fmt::format("{}config/request", everest_prefix);
    mqtt_request.request_data = json(request).dump();

    json result;

    config::Response response = mqtt->get(mqtt_request, config::mqtt_get_config_retries);
    EVLOG_verbose << fmt::format("Incoming config for {}", module_id);
    if (response.status == config::ResponseStatus::Ok and response.type.has_value() and
        response.type.value() == config::Type::Get) {
        auto get_response = std::get<config::GetResponse>(response.response);
        result = get_response.data;
    }
    result.update(get_definitions(mqtt));

    return result;
}

} // namespace Everest
