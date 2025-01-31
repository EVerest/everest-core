// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <future>

#include <fmt/core.h>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

#include <utils/module_config.hpp>
#include <utils/types.hpp>

namespace Everest {
using json = nlohmann::json;

inline constexpr int mqtt_get_config_timeout_ms = 5000;

json get_module_config(std::shared_ptr<MQTTAbstraction> mqtt, const std::string& module_id) {
    const auto& everest_prefix = mqtt->get_everest_prefix();

    const auto get_config_topic = fmt::format("{}modules/{}/get_config", everest_prefix, module_id);
    const auto config_topic = fmt::format("{}modules/{}/config", everest_prefix, module_id);
    std::promise<json> res_promise;
    std::future<json> res_future = res_promise.get_future();

    const auto res_handler = [module_id, &res_promise](const std::string& topic, json data) {
        EVLOG_verbose << fmt::format("Incoming config for {}", module_id);

        res_promise.set_value(std::move(data));
    };

    const std::shared_ptr<TypedHandler> res_token =
        std::make_shared<TypedHandler>(HandlerType::GetConfig, std::make_shared<Handler>(res_handler));
    mqtt->register_handler(config_topic, res_token, QOS::QOS2);

    const json config_publish_data = json::object({{"type", "full"}});

    mqtt->publish(get_config_topic, config_publish_data, QOS::QOS2);

    // wait for result future
    const std::chrono::time_point<std::chrono::steady_clock> res_wait =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(mqtt_get_config_timeout_ms);
    std::future_status res_future_status;
    do {
        res_future_status = res_future.wait_until(res_wait);
    } while (res_future_status == std::future_status::deferred);

    json result;
    if (res_future_status == std::future_status::timeout) {
        mqtt->unregister_handler(config_topic, res_token);
        EVLOG_AND_THROW(
            EverestTimeoutError(fmt::format("Timeout while waiting for result of get_config of {}", module_id)));
    }
    if (res_future_status == std::future_status::ready) {
        result = res_future.get();
    }
    mqtt->unregister_handler(config_topic, res_token);

    const auto interface_names_topic = fmt::format("{}interfaces", everest_prefix);
    const auto interface_names = mqtt->get(interface_names_topic, QOS::QOS2);
    auto interface_definitions = json::object();
    for (const auto& interface : interface_names) {
        auto interface_topic = fmt::format("{}interface_definitions/{}", everest_prefix, interface.get<std::string>());
        auto interface_definition = mqtt->get(interface_topic, QOS::QOS2);
        interface_definitions[interface] = interface_definition;
    }

    result["interface_definitions"] = interface_definitions;

    const auto type_names_topic = fmt::format("{}types", everest_prefix);
    const auto type_names = mqtt->get(type_names_topic, QOS::QOS2);
    auto type_definitions = json::object();
    for (const auto& type_name : type_names) {
        // type_definition keys already start with a / so omit it in the topic name
        auto type_topic = fmt::format("{}type_definitions{}", everest_prefix, type_name.get<std::string>());
        auto type_definition = mqtt->get(type_topic, QOS::QOS2);
        type_definitions[type_name] = type_definition;
    }

    result["types"] = type_definitions;

    const auto module_provides_topic = fmt::format("{}module_provides", everest_prefix);
    const auto module_provides = mqtt->get(module_provides_topic, QOS::QOS2);
    result["module_provides"] = module_provides;

    const auto settings_topic = fmt::format("{}settings", everest_prefix);
    const auto settings = mqtt->get(settings_topic, QOS::QOS2);
    result["settings"] = settings;

    const auto schemas_topic = fmt::format("{}schemas", everest_prefix);
    const auto schemas = mqtt->get(schemas_topic, QOS::QOS2);
    result["schemas"] = schemas;

    const auto module_names_topic = fmt::format("{}module_names", everest_prefix);
    const auto module_names = mqtt->get(module_names_topic, QOS::QOS2);
    result["module_names"] = module_names;

    const auto manifests_topic = fmt::format("{}manifests", everest_prefix);
    auto manifests = json::object();
    for (const auto& module_name : module_names) {
        auto manifest_topic = fmt::format("{}manifests/{}", everest_prefix, module_name.get<std::string>());
        auto manifest = mqtt->get(manifest_topic, QOS::QOS2);
        manifests[module_name] = manifest;
    }

    result["manifests"] = manifests;

    const auto error_types_map_topic = fmt::format("{}error_types_map", everest_prefix);
    const auto error_types_map = mqtt->get(error_types_map_topic, QOS::QOS2);
    result["error_map"] = error_types_map;

    const auto module_config_cache_topic = fmt::format("{}module_config_cache", everest_prefix);
    const auto module_config_cache = mqtt->get(module_config_cache_topic, QOS::QOS2);
    result["module_config_cache"] = module_config_cache;

    return result;
}

} // namespace Everest
