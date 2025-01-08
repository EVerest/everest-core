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

bool operator<(const Requirement& lhs, const Requirement& rhs) {
    if (lhs.id < rhs.id) {
        return true;
    } else if (lhs.id == rhs.id) {
        return (lhs.index < rhs.index);
    } else {
        return false;
    }
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
NLOHMANN_JSON_NAMESPACE_END
