// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <fstream>

#include <ocpp/common/schemas.hpp>
#include <ocpp/v201/device_model_management.hpp>

namespace ocpp {
namespace v201 {
DeviceModelManager::DeviceModelManager(const json& config, const std::filesystem::path& ocpp_main_path) {
    auto json_config = config;

    // validate config entries
    Schemas schemas = Schemas(ocpp_main_path / "component_schemas");
    try {
        const auto patch = schemas.get_validator()->validate(json_config);
        if (!patch.is_null()) {
            // extend config with default values
            EVLOG_debug << "Adding the following default values to the charge point device_tree_manager: " << patch;
            json_config = json_config.patch(patch);
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error while validating OCPP config against schemas: " << e.what();
        EVLOG_AND_THROW(e);
    }

    std::set<std::filesystem::path> available_schemas_paths;
    const auto component_schemas_path = ocpp_main_path / "component_schemas";

    // iterating over schemas to initialize standardized components and variables
    for (auto file : std::filesystem::directory_iterator(component_schemas_path)) {
        if (file.path().filename() != "Config.json") {

            auto component_name = file.path().filename().replace_extension("").string();
            this->components.insert(std::pair<StandardizedComponent, EnhancedComponent>(
                conversions::string_to_standardized_component(component_name), EnhancedComponent(component_name)));

            std::ifstream ifs(file.path().c_str());
            std::string schema_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
            const auto schema = json::parse(schema_file);

            json properties = schema["properties"];
            for (auto property_it = properties.begin(); property_it != properties.end(); ++property_it) {
                VariableCharacteristics characteristics(property_it.value()["characteristics"]);
                EnhancedVariable enhanced_variable;
                enhanced_variable.name = property_it.key();
                enhanced_variable.characteristics.emplace(characteristics);
                for (const auto& att : property_it.value()["attributes"]) {
                    VariableAttribute attribute(att);
                    enhanced_variable.attributes[attribute.type.value()] = attribute;
                }
                this->components.at(conversions::string_to_standardized_component(component_name))
                    .variables[property_it.key()] = enhanced_variable;
            }
        }
    }

    // components and respective variables are initialized
    // insert variables of components using the given config file
    for (auto component_it = config.begin(); component_it != config.end(); ++component_it) {
        auto component_name = component_it.key();
        auto component_obj = component_it.value();

        for (auto variable_it = component_obj.begin(); variable_it != component_obj.end(); ++variable_it) {
            const auto variable_name = variable_it.key();
            CiString<2500> value;

            if (variable_it.value().type() == json::value_t::boolean) {
                value.set(ocpp::conversions::bool_to_string(variable_it.value()));
            } else if (variable_it.value().type() == json::value_t::number_unsigned) {
                value.set(std::to_string(variable_it.value().get<int>()));
            } else if (variable_it.value().type() == json::value_t::number_integer) {
                value.set(std::to_string(variable_it.value().get<int>()));
            } else if (variable_it.value().type() == json::value_t::number_float) {
                value.set(ocpp::conversions::double_to_string(variable_it.value()));
            } else if (variable_it.value().type() == json::value_t::array) {
                value.set("[]");
            } else if (variable_it.value().type() == json::value_t::string) {
                value.set(variable_it.value());
            } else {
                EVLOG_AND_THROW(std::runtime_error("Could not initialize enhanced variable with name"));
            }

            this->components.at(conversions::string_to_standardized_component(component_name))
                .variables.at(variable_name)
                .attributes.at(AttributeEnum::Actual)
                .value.emplace(value);
        }
    }
}

SetVariableStatusEnum DeviceModelManager::set_variable(const SetVariableData& set_variable_data) {
    // FIXME(piet): Consider RebootRequired, etc. ; propably needs to be reworked in general
    try {
        const auto standardized_component =
            conversions::string_to_standardized_component(set_variable_data.component.name.get());
        if (this->components.find(standardized_component) == this->components.end()) {
            return SetVariableStatusEnum::UnknownComponent;
        } else {
            auto component = this->components.at(standardized_component);
            if (component.variables.find(set_variable_data.variable.name.get()) == component.variables.end()) {
                return SetVariableStatusEnum::UnknownVariable;
            } else {
                auto variable = component.variables.at(set_variable_data.variable.name.get());
                if (variable.attributes.find(set_variable_data.attributeType.value_or(AttributeEnum::Actual)) ==
                    variable.attributes.end()) {
                    return SetVariableStatusEnum::NotSupportedAttributeType;
                } else {
                    // FIXME(piet): Setting this value for the component does not mean that it is set within the module
                    // configuration (e.g. AuthCtrlr / Auth Module)

                    // FIXME(piet): add handling for B05.FR.07
                    // FIXME(piet): add handling for B05.FR.08
                    // FIXME(piet): add handling for B05.FR.09
                    // FIXME(piet): add handling for B05.FR.11
                    this->components.at(standardized_component)
                        .variables.at(set_variable_data.variable.name.get())
                        .attributes.at(set_variable_data.attributeType.value_or(AttributeEnum::Actual))
                        .value.emplace(set_variable_data.attributeValue.get());
                    return SetVariableStatusEnum::Accepted;
                }
            }
        }
    } catch (const std::exception& e) {
        return SetVariableStatusEnum::UnknownComponent;
    }
}

std::pair<GetVariableStatusEnum, std::optional<CiString<2500>>>
DeviceModelManager::get_variable(const GetVariableData& get_variable_data) {

    std::pair<GetVariableStatusEnum, std::optional<CiString<2500>>> status_value_pair;

    try {
        const auto standardized_component =
            conversions::string_to_standardized_component(get_variable_data.component.name.get());
        if (this->components.find(standardized_component) == this->components.end()) {
            // this is executed when the component is part of the StandardizedComponents enum, but was not configured in
            // the config file
            status_value_pair.first = GetVariableStatusEnum::UnknownComponent;
            return status_value_pair;
        } else {
            auto component = this->components.at(standardized_component);
            if (component.variables.find(get_variable_data.variable.name.get()) == component.variables.end()) {
                status_value_pair.first = GetVariableStatusEnum::UnknownVariable;
                return status_value_pair;
            } else {
                auto variable = component.variables.at(get_variable_data.variable.name.get());
                if (variable.attributes.find(get_variable_data.attributeType.value_or(AttributeEnum::Actual)) ==
                    variable.attributes.end()) {
                    status_value_pair.first = GetVariableStatusEnum::NotSupportedAttributeType;
                    return status_value_pair;
                } else {
                    // FIXME(piet): add handling for B06.FR.09
                    // FIXME(piet): add handling for B06.FR.14
                    // FIXME(piet): add handling for B06.FR.15

                    status_value_pair.second.emplace(
                        this->components.at(standardized_component)
                            .variables.at(get_variable_data.variable.name.get())
                            .attributes.at(get_variable_data.attributeType.value_or(AttributeEnum::Actual))
                            .value.value_or("")); // B06.FR.13
                    status_value_pair.first = GetVariableStatusEnum::Accepted;
                    return status_value_pair;
                }
            }
        }
    } catch (const std::out_of_range& ex) {
        // this catches when the requested component is not part of StandardizedComponent enum
        // FIXME(piet): Consider more than just standardized components
        status_value_pair.first = GetVariableStatusEnum::UnknownComponent;
        return status_value_pair;
    }
}

static bool component_criteria_match(const EnhancedComponent& enhanced_component,
                                     const std::vector<ComponentCriterionEnum>& component_criteria) {
    for (const auto& criteria : component_criteria) {
        const auto variable_name = conversions::component_criterion_enum_to_string(criteria);
        if (!enhanced_component.variables.count(variable_name) or
            (enhanced_component.variables.at(variable_name).attributes.at(AttributeEnum::Actual).value.has_value() and
             ocpp::conversions::string_to_bool(enhanced_component.variables.at(variable_name)
                                                   .attributes.at(AttributeEnum::Actual)
                                                   .value.value()
                                                   .get()))) {
            return true;
        }
    }
    return false;
}

std::vector<ReportData>
DeviceModelManager::get_report_data(const std::optional<ReportBaseEnum>& report_base,
                                    const std::optional<std::vector<ComponentVariable>>& component_variables,
                                    const std::optional<std::vector<ComponentCriterionEnum>>& component_criteria) {
    std::vector<ReportData> report_data_vec;

    for (const auto& component_entry : this->components) {
        if (!component_criteria.has_value() or
            component_criteria_match(component_entry.second, component_criteria.value())) {
            for (const auto& variable_entry : component_entry.second.variables) {
                const auto variable = variable_entry.second;
                if (!component_variables.has_value() or
                    std::find_if(component_variables.value().begin(), component_variables.value().end(),
                                 [variable, component_entry](ComponentVariable v) {
                                     return component_entry.second == v.component and v.variable.has_value() and
                                            variable == v.variable.value();
                                 }) != component_variables.value().end()) {
                    ReportData report_data;
                    report_data.component = component_entry.second;
                    report_data.variable = variable;
                    for (const auto& attribute_entry : variable.attributes) {
                        const auto attribute = attribute_entry.second;
                        if (report_base == ReportBaseEnum::FullInventory or
                            attribute.mutability == MutabilityEnum::ReadWrite or
                            attribute.mutability == MutabilityEnum::WriteOnly) {
                            report_data.variableAttribute.push_back(attribute);
                            if (variable.characteristics.has_value()) {
                                report_data.variableCharacteristics.emplace(variable.characteristics.value());
                            }
                        }
                    }
                    if (!report_data.variableAttribute.empty()) {
                        report_data_vec.push_back(report_data);
                    }
                }
            }
        }
    }
    return report_data_vec;
}

} // namespace v201
} // namespace ocpp
