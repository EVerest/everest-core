// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <cstddef>

#include <utils/types.hpp>

TypedHandler::TypedHandler(const std::string& name_, const std::string& id_, HandlerType type_,
                           std::shared_ptr<Handler> handler_) :
    name(name_), id(id_), type(type_), handler(std::move(handler_)) {
}

TypedHandler::TypedHandler(const std::string& name_, HandlerType type_, std::shared_ptr<Handler> handler_) :
    TypedHandler(name_, "", type_, std::move(handler_)) {
}

TypedHandler::TypedHandler(HandlerType type_, std::shared_ptr<Handler> handler_) :
    TypedHandler("", "", type_, std::move(handler_)) {
}

ImplementationIdentifier::ImplementationIdentifier(const std::string& module_id_, const std::string& implementation_id_,
                                                   std::optional<Mapping> mapping_) :
    module_id(module_id_), implementation_id(implementation_id_), mapping(mapping_) {
}

std::string ImplementationIdentifier::to_string() const {
    return this->module_id + "->" + this->implementation_id;
}

bool operator==(const ImplementationIdentifier& lhs, const ImplementationIdentifier& rhs) {
    return lhs.module_id == rhs.module_id && lhs.implementation_id == rhs.implementation_id;
}

bool operator!=(const ImplementationIdentifier& lhs, const ImplementationIdentifier& rhs) {
    return !(lhs == rhs);
}

std::string mqtt_message_type_to_string(MqttMessageType type) {
    switch (type) {
    case MqttMessageType::Var:
        return "Var";
    case MqttMessageType::Cmd:
        return "Cmd";
    case MqttMessageType::CmdResult:
        return "CmdResult";
    case MqttMessageType::ExternalMQTT:
        return "ExternalMQTT";
    case MqttMessageType::RaiseError:
        return "RaiseError";
    case MqttMessageType::ClearError:
        return "ClearError";
    case MqttMessageType::GetConfig:
        return "GetConfig";
    case MqttMessageType::GetConfigResponse:
        return "GetConfigResponse";
    case MqttMessageType::Heartbeat:
        return "Heartbeat";
    case MqttMessageType::ModuleReady:
        return "ModuleReady";
    case MqttMessageType::GlobalReady:
        return "GlobalReady";
    default:
        throw std::runtime_error("Unknown MQTT message type");
    }
}

MqttMessageType string_to_mqtt_message_type(const std::string& str) {
    if (str == "Var") {
        return MqttMessageType::Var;
    } else if (str == "Cmd") {
        return MqttMessageType::Cmd;
    } else if (str == "CmdResult") {
        return MqttMessageType::CmdResult;
    } else if (str == "ExternalMQTT") {
        return MqttMessageType::ExternalMQTT;
    } else if (str == "RaiseError") {
        return MqttMessageType::RaiseError;
    } else if (str == "ClearError") {
        return MqttMessageType::ClearError;
    } else if (str == "GetConfig") {
        return MqttMessageType::GetConfig;
    } else if (str == "GetConfigResponse") {
        return MqttMessageType::GetConfigResponse;
    } else if (str == "Heartbeat") {
        return MqttMessageType::Heartbeat;
    } else if (str == "ModuleReady") {
        return MqttMessageType::ModuleReady;
    } else if (str == "GlobalReady") {
        return MqttMessageType::GlobalReady;
    }

    throw std::runtime_error("Unknown MQTT message type string: " + str);
}

NLOHMANN_JSON_NAMESPACE_BEGIN
void adl_serializer<Mapping>::to_json(json& j, const Mapping& m) {
    j = {{"evse", m.evse}};
    if (m.connector.has_value()) {
        j["connector"] = m.connector.value();
    }
}

Mapping adl_serializer<Mapping>::from_json(const json& j) {
    auto m = Mapping(j.at("evse").get<int>());
    if (j.contains("connector")) {
        m.connector = j.at("connector").get<int>();
    }
    return m;
}

void adl_serializer<TelemetryConfig>::to_json(json& j, const TelemetryConfig& t) {
    j = {{"id", t.id}};
}

TelemetryConfig adl_serializer<TelemetryConfig>::from_json(const json& j) {
    auto t = TelemetryConfig(j.at("id").get<int>());
    return t;
}

void adl_serializer<ModuleTierMappings>::to_json(json& j, const ModuleTierMappings& m) {
    if (m.module.has_value()) {
        j = {{"module", m.module.value()}};
    }
    if (m.implementations.size() > 0) {
        j["implementations"] = json::object();
        for (auto& impl_mapping : m.implementations) {
            if (impl_mapping.second.has_value()) {
                j["implementations"][impl_mapping.first] = impl_mapping.second.value();
            }
        }
    }
}

ModuleTierMappings adl_serializer<ModuleTierMappings>::from_json(const json& j) {
    ModuleTierMappings m;
    if (!j.is_null()) {
        if (j.contains("module")) {
            m.module = j.at("module");
        }
        if (j.contains("implementations")) {
            for (auto& impl : j.at("implementations").items()) {
                m.implementations[impl.key()] = impl.value();
            }
        }
    }
    return m;
}

void adl_serializer<Requirement>::to_json(json& j, const Requirement& r) {
    j = {{"id", r.id}};
    if (r.index != 0) {
        j["index"] = r.index;
    }
}

Requirement adl_serializer<Requirement>::from_json(const json& j) {
    Requirement r;
    r.id = j.at("id").get<std::string>();
    if (j.contains("index")) {
        r.index = j.at("index").get<size_t>();
    }
    return r;
}

void adl_serializer<Fulfillment>::to_json(json& j, const Fulfillment& f) {
    j = {{"module_id", f.module_id}, {"implementation_id", f.implementation_id}, {"requirement", f.requirement}};
}

Fulfillment adl_serializer<Fulfillment>::from_json(const json& j) {
    Fulfillment f;
    f.module_id = j.at("module_id").get<std::string>();
    f.implementation_id = j.at("implementation_id").get<std::string>();
    f.requirement = j.at("requirement").get<Requirement>();
    return f;
}

void adl_serializer<MqttMessagePayload>::to_json(json& j, const MqttMessagePayload& m) {
    j = {{"msg_type", mqtt_message_type_to_string(m.type)}, {"data", m.data}};
}

MqttMessagePayload adl_serializer<MqttMessagePayload>::from_json(const json& j) {
    return MqttMessagePayload{string_to_mqtt_message_type(j.at("msg_type").get<std::string>()),
                              j.at("data").get<json>()};
}

NLOHMANN_JSON_NAMESPACE_END
