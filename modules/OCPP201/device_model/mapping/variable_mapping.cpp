// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model/mapping/variable_mapping.hpp>
#include <everest/logging.hpp>
#include <ocpp/v2/comparators.hpp>
#include <utils/yaml_loader.hpp>

using namespace ocpp::v2;
using namespace everest::config;

VariableMapping::VariableMapping(const fs::path& mapping_file, const fs::path& schema_file) {
    if (!fs::exists(mapping_file)) {
        EVLOG_warning << "Mapping file does not exist: " << mapping_file;
        return;
    }

    const auto& mapping = Everest::load_yaml(mapping_file);
    const auto& schema = Everest::load_yaml(schema_file);

    auto validator = nlohmann::json_schema::json_validator{};
    validator.set_root_schema(schema);
    validator.validate(mapping);

    for (const auto& entry : mapping["mappings"]) {
        ComponentVariable cv = entry["ocpp"];
        ConfigurationParameterIdentifier cpi = entry["everest"];
        user_mapping[cpi] = cv;
    }
};

void VariableMapping::add_cv_mapping(const ComponentVariable& everest_component_variable,
                                     const ComponentVariable& ocpp_component_variable) {
    this->everest_cv_to_ocpp_cv_mapping[everest_component_variable] = ocpp_component_variable;
    this->ocpp_cv_to_everest_cv_mapping[ocpp_component_variable] = everest_component_variable;
}

std::optional<ComponentVariable> VariableMapping::get_ocpp_cv(const ConfigurationParameterIdentifier& identifier) {
    auto it = user_mapping.find(identifier);
    if (it != user_mapping.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<ocpp::v2::ComponentVariable>
VariableMapping::get_ocpp_cv(const ocpp::v2::ComponentVariable& everest_component_variable) {
    auto it = everest_cv_to_ocpp_cv_mapping.find(everest_component_variable);
    if (it != everest_cv_to_ocpp_cv_mapping.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<ocpp::v2::ComponentVariable>
VariableMapping::get_everest_cv(const ocpp::v2::ComponentVariable& ocpp_component_variable) {
    auto it = ocpp_cv_to_everest_cv_mapping.find(ocpp_component_variable);
    if (it != ocpp_cv_to_everest_cv_mapping.end()) {
        return it->second;
    }
    return std::nullopt;
}
