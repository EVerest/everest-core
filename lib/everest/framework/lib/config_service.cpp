// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <exception>

#include <everest/logging.hpp>

#include <utils/config/storage.hpp>
#include <utils/config/types.hpp>
#include <utils/config_service.hpp>
#include <utils/conversions.hpp>

namespace Everest {
namespace config {

bool ModuleIdType::operator<(const ModuleIdType& rhs) const {
    return (this->module_id < rhs.module_id ||
            (this->module_id == rhs.module_id && this->module_type < rhs.module_type));
}

enum class AccessMethod {
    Read,
    Write
};

namespace {
bool access_allowed(const everest::config::Access& access, const std::string& origin, const std::string& target,
                    AccessMethod method) {
    if (origin == target) {
        // a module can always read and write its own config
        return true;
    }
    if (not access.config.has_value()) {
        return false;
    }
    const auto& config_access = access.config.value();
    const auto& config_access_modules_it = config_access.modules.find(target);
    switch (method) {
    case AccessMethod::Read:
        if (config_access.allow_global_read) {
            return true;
        }
        if (config_access_modules_it != config_access.modules.end()) {
            if (config_access_modules_it->second.allow_read) {
                return true;
            }
        }
        break;
    case AccessMethod::Write:
        if (config_access.allow_global_write) {
            return true;
        }
        if (config_access_modules_it != config_access.modules.end()) {
            if (config_access_modules_it->second.allow_write) {
                return true;
            }
        }
        break;
    }

    return false;
}

nlohmann::json get_module_config(const std::string& module_id, const ManagerConfig& config) {
    const auto& module_configurations = config.get_module_configurations();
    return get_serialized_module_config(module_id, module_configurations);
}

everest::config::ModuleConfigurationParameters
update_mutability(const everest::config::ModuleConfigurationParameters& configuration_parameters,
                  bool allow_set_read_only = false) {
    if (not allow_set_read_only) {
        return configuration_parameters;
    }
    everest::config::ModuleConfigurationParameters updated_configuration_parameters = configuration_parameters;
    for (auto& [impl_id, config_params] : updated_configuration_parameters) {
        for (auto& config_param : config_params) {
            auto& mutability = config_param.characteristics.mutability;
            if (mutability == everest::config::Mutability::ReadOnly) {
                config_param.characteristics.mutability = everest::config::Mutability::ReadWrite;
            }
        }
    }
    return updated_configuration_parameters;
}

GetResponse handle_get_module_config(const std::string& origin, const ManagerConfig& config) {
    GetResponse get_response;
    get_response.type = GetType::Module;
    get_response.data = get_module_config(origin, config);
    return get_response;
}

Response handle_get_config_value(const GetRequest& get_request, const std::string& origin, ManagerConfig& config) {
    Response response;
    response.type = Type::Get;
    GetResponse get_response;
    get_response.type = GetType::Value;

    if (not get_request.identifier.has_value()) {
        response.status = ResponseStatus::Error; // no identifier is always an error in this case
        response.status_info = "No identifier provided";
    } else {
        const auto identifier = get_request.identifier.value();
        const auto& module_configs = config.get_module_configurations();
        const everest::config::Access access = module_configs.at(origin).access;
        if (not access_allowed(access, origin, identifier.module_id, AccessMethod::Read)) {
            response.status = ResponseStatus::AccessDenied;
            response.status_info =
                fmt::format("Access to config item denied: {} cannot access {}", origin, identifier.module_id);
        } else {
            const auto get_config_value_response = config.get_config_value(identifier);
            if (get_config_value_response.status == everest::config::GetSetResponseStatus::OK) {
                if (get_config_value_response.configuration_parameter.has_value()) {
                    get_response.data = get_config_value_response.configuration_parameter.value();
                    response.status = ResponseStatus::Ok;
                }
            }
        }
    }

    return response;
}

GetResponse handle_get_all_configs(const std::string& origin, const ManagerConfig& config) {
    // FIXME: we might run into size limits when the config is really large, split this up in the
    // future!
    GetResponse get_response;
    get_response.type = GetType::All;
    json all_configs = json::object();
    const auto& module_configs = config.get_module_configurations();
    everest::config::Access access = module_configs.at(origin).access;
    for (const auto& [module_id, module_name] : config.get_module_names()) {
        if (not access_allowed(access, origin, module_id, AccessMethod::Read)) {
            // request.origin has no access to module_id.config
            continue;
        }
        auto allow_set_read_only = false;
        if (access.config.has_value()) {
            const auto config_access = access.config.value();
            allow_set_read_only = config_access.allow_set_read_only;

            if (not allow_set_read_only) {
                // check if allow_set_read_only is set for specific modules
                const auto module_config_access_it = config_access.modules.find(module_id);
                if (module_config_access_it != config_access.modules.end()) {
                    allow_set_read_only = module_config_access_it->second.allow_set_read_only;
                }
            }
        }

        all_configs[module_id] =
            update_mutability(module_configs.at(module_id).configuration_parameters, allow_set_read_only);
    }
    get_response.data = all_configs;
    return get_response;
}

GetResponse handle_get_all_mappings(const std::string& origin, const ManagerConfig& config) {
    GetResponse get_response;
    get_response.type = GetType::AllMappings;
    json all_mappings = json::object();
    const auto& module_configs = config.get_module_configurations();
    const everest::config::Access access = module_configs.at(origin).access;
    for (const auto& [module_id, module_name] : config.get_module_names()) {
        if (not access_allowed(access, origin, module_id, AccessMethod::Read)) {
            // request.origin has no access to module_id.mappings
            continue;
        }
        all_mappings[module_id] = module_configs.at(module_id).mapping;
    }

    get_response.data = all_mappings;
    return get_response;
}

Response handle_set_request(const SetRequest& set_request, const std::string& origin, ManagerConfig& config) {
    Response response;
    response.type = Type::Set;
    SetResponse set_response;
    set_response.status = SetResponseStatus::Rejected;

    const auto& module_configs = config.get_module_configurations();
    const everest::config::Access access = module_configs.at(origin).access;
    if (not access_allowed(access, origin, set_request.identifier.module_id, AccessMethod::Write)) {
        set_response.status = SetResponseStatus::Rejected;
        response.status = ResponseStatus::AccessDenied;
        response.status_info =
            fmt::format("Access to config item denied: {} cannot access {}", origin, set_request.identifier.module_id);
    } else {
        // TODO: explicit input validation
        const auto& target_module_config = module_configs.at(set_request.identifier.module_id);
        everest::config::SetConfigStatus status = everest::config::SetConfigStatus::Rejected;
        const auto impl_id = set_request.identifier.module_implementation_id.value_or(MODULE_IMPLEMENTATION_ID);
        for (const auto& config_entry : target_module_config.configuration_parameters.at(impl_id)) {
            if (config_entry.name == set_request.identifier.configuration_parameter_name) {
                try {
                    const auto value = parse_config_value(config_entry.characteristics.datatype, set_request.value);
                    status = config.set_config_value(set_request.identifier, value);
                    response.status = ResponseStatus::Ok;
                } catch (const std::exception& e) {
                    response.status = ResponseStatus::Error;
                    response.status_info = fmt::format("Could not set config entry {} of module {}: {}",
                                                       set_request.identifier.configuration_parameter_name,
                                                       set_request.identifier.module_id, e.what());
                }
                break;
            }
        }
        set_response.status = conversions::set_config_status_to_set_response_status(status);
    }
    response.response = set_response;
    return response;
}
} // namespace

ConfigServiceClient::ConfigServiceClient(std::shared_ptr<MQTTAbstraction> mqtt_abstraction,
                                         const std::string& module_id,
                                         const std::unordered_map<std::string, std::string>& module_names) :
    mqtt_abstraction(mqtt_abstraction), origin(module_id), module_names(module_names) {
}

std::map<ModuleIdType, everest::config::ModuleConfigurationParameters> ConfigServiceClient::get_module_configs() {
    Request get_request;
    get_request.type = Type::Get;
    get_request.request = GetRequest{GetType::All};
    get_request.origin = this->origin;

    MQTTRequest mqtt_request;
    mqtt_request.response_topic =
        fmt::format("{}modules/{}/response", mqtt_abstraction->get_everest_prefix(), get_request.origin);
    mqtt_request.request_topic = fmt::format("{}config/request", mqtt_abstraction->get_everest_prefix());
    mqtt_request.request_data = json(get_request).dump();

    try {
        Response response = mqtt_abstraction->get(mqtt_request);
        if (response.status != ResponseStatus::Ok) {
            EVLOG_error << "Could not get module configs via MQTT";
            return {};
        }

        GetResponse get_response = std::get<GetResponse>(response.response);

        std::map<ModuleIdType, everest::config::ModuleConfigurationParameters> module_configs;
        for (const auto& [module_id, config_maps] : get_response.data.items()) {
            ModuleIdType module_id_type;
            module_id_type.module_id = module_id;
            module_id_type.module_type = module_names.at(module_id);
            module_configs[module_id_type] = config_maps;
        }

        return module_configs;
    } catch (const std::exception& e) {
        EVLOG_error << "Could not get module configs via MQTT: " << e.what();
        return {};
    }
}

std::map<std::string, ModuleTierMappings> ConfigServiceClient::get_mappings() {
    Request get_request;
    get_request.type = Type::Get;
    get_request.request = GetRequest{GetType::AllMappings};
    get_request.origin = this->origin;

    MQTTRequest mqtt_request;
    mqtt_request.response_topic =
        fmt::format("{}modules/{}/response", mqtt_abstraction->get_everest_prefix(), get_request.origin);
    mqtt_request.request_topic = fmt::format("{}config/request", mqtt_abstraction->get_everest_prefix());
    mqtt_request.request_data = json(get_request).dump();

    try {
        Response response = mqtt_abstraction->get(mqtt_request);
        if (response.status != ResponseStatus::Ok) {
            EVLOG_error << "Could not get mappings configs via MQTT";
            return {};
        }

        GetResponse get_response = std::get<GetResponse>(response.response);

        std::map<std::string, ModuleTierMappings> mappings;
        for (const auto& [module_id, mapping] : get_response.data.items()) {
            mappings[module_id] = everest::config::parse_mapping(mapping);
        }

        return mappings;
    } catch (const std::exception& e) {
        EVLOG_error << "Could not get mappings configs via MQTT: " << e.what();
        return {};
    }
}

SetConfigResult
ConfigServiceClient::set_config_value(const everest::config::ConfigurationParameterIdentifier& identifier,
                                      const std::string& value) {
    SetConfigResult result;
    Request request;
    request.type = Type::Set;
    request.origin = this->origin;
    SetRequest set_request;
    set_request.identifier = identifier;
    set_request.value = value;
    request.request = set_request;

    try {
        MQTTRequest mqtt_request;
        mqtt_request.response_topic =
            fmt::format("{}modules/{}/response", mqtt_abstraction->get_everest_prefix(), request.origin);
        mqtt_request.request_topic = fmt::format("{}config/request", mqtt_abstraction->get_everest_prefix());
        mqtt_request.request_data = json(request).dump();

        const Response response = mqtt_abstraction->get(mqtt_request);
        result.status = response.status;
        result.status_info = response.status_info;
        if (response.status == ResponseStatus::Ok) {
            if (response.type.has_value() and response.type.value() == Type::Set) {
                const SetResponse set_response = std::get<SetResponse>(response.response);
                result.set_status = conversions::set_response_status_to_set_config_status(set_response.status);
            }
        }
        return result;
    } catch (const std::exception& e) {
        EVLOG_info << "Could not set config value: " << identifier.module_id << ": "
                   << identifier.module_implementation_id.value_or(MODULE_IMPLEMENTATION_ID) << ": "
                   << identifier.configuration_parameter_name << ": " << e.what();
        result.status = ResponseStatus::Error;
    }
    return result;
}

GetConfigResult
ConfigServiceClient::get_config_value(const everest::config::ConfigurationParameterIdentifier& identifier) {
    GetConfigResult result;
    Request request;
    request.type = Type::Get;
    request.origin = this->origin;
    GetRequest get_request;
    get_request.type = GetType::Value;
    get_request.identifier = identifier;
    request.request = get_request;

    try {
        MQTTRequest mqtt_request;
        mqtt_request.response_topic =
            fmt::format("{}modules/{}/response", mqtt_abstraction->get_everest_prefix(), request.origin);
        mqtt_request.request_topic = fmt::format("{}config/request", mqtt_abstraction->get_everest_prefix());
        mqtt_request.request_data = json(request).dump();
        const Response response = mqtt_abstraction->get(mqtt_request);
        result.status = response.status;
        result.status_info = response.status_info;
        if (response.status == ResponseStatus::Ok) {
            if (response.type.has_value() and response.type.value() == Type::Get) {
                const GetResponse get_response = std::get<GetResponse>(response.response);
                result.configuration_parameter = get_response.data;
            }
        }
    } catch (const std::exception& e) {
        EVLOG_info << "Could not get config value: " << identifier.module_id << ": "
                   << identifier.module_implementation_id.value_or(MODULE_IMPLEMENTATION_ID) << ": "
                   << identifier.configuration_parameter_name << ": " << e.what();
    }

    return result;
}

ConfigService::ConfigService(MQTTAbstraction& mqtt_abstraction, std::shared_ptr<ManagerConfig> config) :
    mqtt_abstraction(mqtt_abstraction), config(config) {

    // TODO: thread-safe?

    const Handler global_config_request_handler = [&mqtt_abstraction, config](const std::string& /*topic*/,
                                                                              const nlohmann::json& data) {
        Response response;
        response.status = ResponseStatus::Error;
        try {
            Request request = data;
            response.type = request.type;
            const auto response_topic =
                fmt::format("{}modules/{}/response", mqtt_abstraction.get_everest_prefix(), request.origin);

            if (request.type == Type::Get) {
                const GetRequest get_request = std::get<GetRequest>(request.request);
                if (get_request.type == GetType::Module) {
                    response.response = handle_get_module_config(request.origin, *config);
                    response.status = ResponseStatus::Ok;
                } else if (get_request.type == GetType::Value) {
                    response = handle_get_config_value(get_request, request.origin, *config);
                } else if (get_request.type == GetType::All) {
                    response.response = handle_get_all_configs(request.origin, *config);
                    response.status = ResponseStatus::Ok;
                } else if (get_request.type == GetType::AllMappings) {
                    response.response = handle_get_all_mappings(request.origin, *config);
                    response.status = ResponseStatus::Ok;
                }
            } else if (request.type == Type::Set) {
                response = handle_set_request(std::get<SetRequest>(request.request), request.origin, *config);
            }

            MqttMessagePayload payload{MqttMessageType::GetConfigResponse, response};
            mqtt_abstraction.publish(response_topic, payload, QOS::QOS2);

        } catch (const std::exception& e) {
            EVLOG_error << "Exception during handling of request: " << e.what();
        } catch (...) {
            EVLOG_error << "Could not parse request: " << data.dump();
        }
    };

    const std::string global_config_request_topic =
        fmt::format("{}config/request", mqtt_abstraction.get_everest_prefix());
    this->get_config_token = std::make_shared<TypedHandler>(HandlerType::GetConfig,
                                                            std::make_shared<Handler>(global_config_request_handler));
    mqtt_abstraction.register_handler(global_config_request_topic, this->get_config_token, QOS::QOS2);
}

namespace conversions {
std::string type_to_string(Type type) {
    switch (type) {
    case Type::Get:
        return "Get";
    case Type::Set:
        return "Set";
    case Type::Unknown:
        return "Unknown";
    }
    throw std::out_of_range("Could not convert Type to string");
}

Type string_to_type(const std::string& type) {
    if (type == "Get") {
        return Type::Get;
    } else if (type == "Set") {
        return Type::Set;
    } else if (type == "Unknown") {
        return Type::Unknown;
    }
    throw std::out_of_range("Could not convert " + type + " to Type");
}

std::string get_type_to_string(GetType type) {
    switch (type) {
    case GetType::All:
        return "All";
    case GetType::Module:
        return "Module";
    case GetType::Value:
        return "Value";
    case GetType::AllMappings:
        return "AllMappings";
    case GetType::Unknown:
        return "Unknown";
    }
    throw std::out_of_range("Could not convert GetType to string");
}

GetType string_to_get_type(const std::string& type) {
    if (type == "All") {
        return GetType::All;
    } else if (type == "Module") {
        return GetType::Module;
    } else if (type == "Value") {
        return GetType::Value;
    } else if (type == "AllMappings") {
        return GetType::AllMappings;
    } else if (type == "Unknown") {
        return GetType::Unknown;
    }
    throw std::out_of_range("Could not convert " + type + " to GetType");
}

std::string response_status_to_string(ResponseStatus status) {
    switch (status) {
    case ResponseStatus::Ok:
        return "Ok";
    case ResponseStatus::Error:
        return "Error";
    case ResponseStatus::AccessDenied:
        return "AccessDenied";
    }
    throw std::out_of_range("Could not convert ResponseStatus to string");
}

ResponseStatus string_to_response_status(const std::string& status) {
    if (status == "Ok") {
        return ResponseStatus::Ok;
    } else if (status == "Error") {
        return ResponseStatus::Error;
    } else if (status == "AccessDenied") {
        return ResponseStatus::AccessDenied;
    }
    throw std::out_of_range("Could not convert " + status + " to ResponseStatus");
}

std::string set_response_status_to_string(SetResponseStatus status) {
    switch (status) {
    case SetResponseStatus::Accepted:
        return "Accepted";
    case SetResponseStatus::Rejected:
        return "Rejected";
    case SetResponseStatus::RebootRequired:
        return "RebootRequired";
    }
    throw std::out_of_range("Could not convert SetResponseStatus to string");
}

SetResponseStatus string_to_set_response_status(const std::string& status) {
    if (status == "Accepted") {
        return SetResponseStatus::Accepted;
    } else if (status == "Rejected") {
        return SetResponseStatus::Rejected;
    } else if (status == "RebootRequired") {
        return SetResponseStatus::RebootRequired;
    }
    throw std::out_of_range("Could not convert " + status + " to SetResponseStatus");
}

everest::config::SetConfigStatus set_response_status_to_set_config_status(SetResponseStatus status) {
    switch (status) {
    case SetResponseStatus::Accepted:
        return everest::config::SetConfigStatus::Accepted;
    case SetResponseStatus::Rejected:
        return everest::config::SetConfigStatus::Rejected;
    case SetResponseStatus::RebootRequired:
        return everest::config::SetConfigStatus::RebootRequired;
    }
    throw std::out_of_range("Could not convert SetResponseStatus to SetConfigStatus");
}

SetResponseStatus set_config_status_to_set_response_status(everest::config::SetConfigStatus status) {
    switch (status) {
    case everest::config::SetConfigStatus::Accepted:
        return SetResponseStatus::Accepted;
    case everest::config::SetConfigStatus::Rejected:
        return SetResponseStatus::Rejected;
    case everest::config::SetConfigStatus::RebootRequired:
        return SetResponseStatus::RebootRequired;
    }
    throw std::out_of_range("Could not convert SetConfigStatus to SetResponseStatus");
}
} // namespace conversions

std::ostream& operator<<(std::ostream& os, const GetType& t) {
    os << conversions::get_type_to_string(t);
    return os;
}

void to_json(nlohmann::json& j, const GetRequest& r) {
    j = {{"type", conversions::get_type_to_string(r.type)}};
    if (r.identifier.has_value()) {
        j["identifier"] = r.identifier.value();
    }
}

void from_json(const nlohmann::json& j, GetRequest& r) {
    r.type = conversions::string_to_get_type(j.at("type"));
    if (j.contains("identifier")) {
        r.identifier = j.at("identifier");
    }
}

void to_json(nlohmann::json& j, const SetRequest& r) {
    j = {{"value", r.value}};
    j["identifier"] = nlohmann::json(r.identifier);
}

void from_json(const nlohmann::json& j, SetRequest& r) {
    r.identifier = j.at("identifier");
    r.value = j.at("value");
}

void to_json(nlohmann::json& j, const GetResponse& r) {
    j = {{"type", conversions::get_type_to_string(r.type)}, {"data", r.data}};
}

void from_json(const nlohmann::json& j, GetResponse& r) {
    r.type = conversions::string_to_get_type(j.at("type"));
    r.data = j.at("data");
}

void to_json(nlohmann::json& j, const SetResponse& r) {
    j = {{"status", conversions::set_response_status_to_string(r.status)}};
}

void from_json(const nlohmann::json& j, SetResponse& r) {
    r.status = conversions::string_to_set_response_status(j.at("status"));
}

void to_json(nlohmann::json& j, const Request& r) {
    j = {{"type", conversions::type_to_string(r.type)}, {"origin", r.origin}};
    j["request"] = Everest::variant_to_json(r.request);
}

void from_json(const nlohmann::json& j, Request& r) {
    r.type = conversions::string_to_type(j.at("type"));
    if (r.type == Type::Get) {
        r.request = j.at("request").get<GetRequest>();
    } else if (r.type == Type::Set) {
        r.request = j.at("request").get<SetRequest>();
    }
    r.origin = j.at("origin");
}

void to_json(nlohmann::json& j, const Response& r) {
    j = {{"status", conversions::response_status_to_string(r.status)}, {"status_info", r.status_info}};
    if (r.type.has_value()) {
        j["type"] = conversions::type_to_string(r.type.value());
    }
    j["response"] = Everest::variant_to_json(r.response);
}

void from_json(const nlohmann::json& j, Response& r) {
    r.status = conversions::string_to_response_status(j.at("status"));
    r.status_info = j.at("status_info");
    if (j.contains("type")) {
        Type type = conversions::string_to_type(j.at("type"));
        r.type = type;

        if (type == Type::Get) {
            r.response = j.at("response").get<GetResponse>();
        } else if (type == Type::Set) {
            r.response = j.at("response").get<SetResponse>();
        }
    }
}

} // namespace config
} // namespace Everest

NLOHMANN_JSON_NAMESPACE_BEGIN
void adl_serializer<everest::config::ConfigurationParameterIdentifier>::to_json(
    nlohmann::json& j, const everest::config::ConfigurationParameterIdentifier& c) {
    j = {{"module_id", c.module_id}, {"configuration_parameter_name", c.configuration_parameter_name}};
    if (c.module_implementation_id.has_value()) {
        j["module_implementation_id"] = c.module_implementation_id.value();
    }
}

void adl_serializer<everest::config::ConfigurationParameterIdentifier>::from_json(
    const nlohmann::json& j, everest::config::ConfigurationParameterIdentifier& c) {
    c.module_id = j.at("module_id");
    c.configuration_parameter_name = j.at("configuration_parameter_name");
    if (j.contains("module_implementation_id")) {
        c.module_implementation_id = j.at("module_implementation_id");
    }
}
NLOHMANN_JSON_NAMESPACE_END
