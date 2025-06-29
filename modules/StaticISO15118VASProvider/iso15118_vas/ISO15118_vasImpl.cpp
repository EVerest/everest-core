// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ISO15118_vasImpl.hpp"
#include <utils/yaml_loader.hpp>

namespace module {
namespace iso15118_vas {

/**
 * @brief Convert a single JSON Parameter Object (name and type) to a VAS Parameter object.
 * @param json The JSON object to convert.
 * @return The converted Parameter object.
 */
types::iso15118_vas::Parameter parameter_from_json(const nlohmann::ordered_json& json) {
    auto param_name = json["name"].get<std::string>();
    auto param_type = json["type"].get<std::string>();

    types::iso15118_vas::Parameter parameter;
    parameter.name = param_name;

    EVLOG_debug << "    Parameter name: " << param_name;

    if (param_type == "bool") {
        parameter.value.bool_value = json["value"].get<bool>();
        EVLOG_debug << "      Parameter value: " << (parameter.value.bool_value.value() ? "true" : "false");
    } else if (param_type == "int32") {
        parameter.value.int_value = json["value"].get<int32_t>();
        EVLOG_debug << "      Parameter value: " << std::to_string(parameter.value.int_value.value());
    } else if (param_type == "int16") {
        parameter.value.short_value = json["value"].get<int16_t>();
        EVLOG_debug << "      Parameter value: " << std::to_string(parameter.value.short_value.value());
    } else if (param_type == "int8") {
        parameter.value.byte_value = json["value"].get<int8_t>();
        EVLOG_debug << "      Parameter value: " << std::to_string(parameter.value.byte_value.value());
    } else if (param_type == "string") {
        parameter.value.finite_string = json["value"].get<std::string>();
        EVLOG_debug << "      Parameter value: " << parameter.value.finite_string.value();
    } else if (param_type == "rational") {
        parameter.value.rational_number = json["value"].get<float>();
        EVLOG_debug << "      Parameter value: " << std::to_string(parameter.value.rational_number.value());
    } else {
        EVLOG_AND_THROW(Everest::EverestConfigError("Unknown parameter type: " + param_type));
    }

    return parameter;
}

/**
 * @brief Convert a single JSON ParameterSet Object (id and parameters) to a ParameterSet object. Also checks for
 * duplicate parameter names.
 * @param json The JSON object to convert.
 * @return The converted ParameterSet object.
 */
types::iso15118_vas::ParameterSet parameter_set_from_json(const nlohmann::ordered_json& json) {
    auto set_id = json["id"].get<uint16_t>();
    auto parameters_raw = json["parameters"];

    std::vector<types::iso15118_vas::Parameter> parameters;
    std::set<std::string> parameter_names; // only used for duplicate check

    EVLOG_debug << "  Parameter set ID: " << set_id;
    for (const auto& parameter : parameters_raw) {
        const auto param = parameter_from_json(parameter);
        parameters.push_back(param);

        // check for duplicate parameter names
        if (parameter_names.find(param.name) != parameter_names.end()) {
            EVLOG_AND_THROW(Everest::EverestConfigError(
                "Parameter name " + param.name + " already exists in parameter set with id " + std::to_string(set_id)));
        }

        parameter_names.insert(param.name);
    }

    return types::iso15118_vas::ParameterSet{set_id, parameters};
}

/**
 * @brief Convert a JSON array of ParameterSet objects to a vector of ParameterSet objects. Also checks for duplicate
 * parameter set IDs.
 * @param json The JSON array to convert.
 * @return The converted vector of ParameterSet objects.
 */
std::vector<types::iso15118_vas::ParameterSet> parameter_sets_from_json(const nlohmann::ordered_json& json) {
    std::vector<types::iso15118_vas::ParameterSet> parameter_sets;
    std::set<uint16_t> parameter_set_ids; // only used for duplicate check

    for (const auto& parameter_set_raw : json) {
        const auto parameter_set = parameter_set_from_json(parameter_set_raw);
        parameter_sets.push_back(parameter_set);

        // check for duplicate parameter set IDs
        if (parameter_set_ids.find(parameter_set.set_id) != parameter_set_ids.end()) {
            EVLOG_AND_THROW(Everest::EverestConfigError("Parameter set ID " + std::to_string(parameter_set.set_id) +
                                                        " already exists"));
        }

        parameter_set_ids.insert(parameter_set.set_id);
    }

    return parameter_sets;
}

void ISO15118_vasImpl::init() {
    EVLOG_info << "Loading VAS configuration from " << mod->config.service_file;

    auto yaml = Everest::load_yaml(mod->config.service_file);
    auto raw_services = yaml["services"];

    for (const auto& service : raw_services) {
        auto service_id = service["id"].get<uint16_t>();

        // check for duplicate service IDs
        if (value_added_services.find(service_id) != value_added_services.end()) {
            EVLOG_AND_THROW(
                Everest::EverestConfigError("Service ID " + std::to_string(service_id) + " already exists"));
        }

        EVLOG_debug << "Service ID: " << service_id;
        value_added_services[service_id] = parameter_sets_from_json(service["parameter_sets"]);
    }

    EVLOG_info << "Loaded VAS configuration with " << value_added_services.size() << " services";
}

void ISO15118_vasImpl::ready() {
    json::array_t service_ids;

    for (const auto& [service_id, parameter_sets] : value_added_services) {
        service_ids.push_back(service_id);
    }

    publish_offered_vas(service_ids);
}

std::vector<types::iso15118_vas::ParameterSet> ISO15118_vasImpl::handle_get_service_parameters(int& service_id) {
    return value_added_services.at(service_id);
}

void ISO15118_vasImpl::handle_selected_services(std::vector<types::iso15118_vas::SelectedService>& selected_services) {
    // empty implementation
}

} // namespace iso15118_vas
} // namespace module
