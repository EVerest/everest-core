// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "misc.hpp"

#include <cstddef>
#include <cstdlib>
#include <stdexcept>

#include <utils/filesystem.hpp>

const std::string get_variable_from_env(const std::string& variable) {
    const auto value = std::getenv(variable.c_str());
    if (value == nullptr) {
        throw std::runtime_error(variable + " needed for everestpy");
    }

    return value;
}

const std::string get_variable_from_env(const std::string& variable, const std::string& default_value) {
    const auto value = std::getenv(variable.c_str());
    if (value == nullptr) {
        return default_value;
    }

    return value;
}

static Everest::MQTTSettings get_mqtt_settings_from_env() {
    const auto mqtt_everest_prefix =
        get_variable_from_env(Everest::EV_MQTT_EVEREST_PREFIX, Everest::defaults::MQTT_EVEREST_PREFIX);
    const auto mqtt_external_prefix =
        get_variable_from_env(Everest::EV_MQTT_EXTERNAL_PREFIX, Everest::defaults::MQTT_EXTERNAL_PREFIX);
    const auto mqtt_broker_socket_path = std::getenv(Everest::EV_MQTT_BROKER_SOCKET_PATH);
    const auto mqtt_broker_host = std::getenv(Everest::EV_MQTT_BROKER_HOST);
    const auto mqtt_broker_port = std::getenv(Everest::EV_MQTT_BROKER_PORT);

    if (mqtt_broker_socket_path == nullptr) {
        if (mqtt_broker_host == nullptr or mqtt_broker_port == nullptr) {
            throw std::runtime_error("If EV_MQTT_BROKER_SOCKET_PATH is not set EV_MQTT_BROKER_HOST and "
                                     "EV_MQTT_BROKER_PORT are needed for everestpy");
        }
        auto mqtt_broker_port_ = Everest::defaults::MQTT_BROKER_PORT;
        try {
            mqtt_broker_port_ = std::stoi(mqtt_broker_port);
        } catch (...) {
            EVLOG_warning << "Could not parse MQTT broker port, using default: " << mqtt_broker_port_;
        }
        return Everest::create_mqtt_settings(mqtt_broker_host, mqtt_broker_port_, mqtt_everest_prefix,
                                             mqtt_external_prefix);
    } else {
        return Everest::create_mqtt_settings(mqtt_broker_socket_path, mqtt_everest_prefix, mqtt_external_prefix);
    }
}

/// This is just kept for compatibility
RuntimeSession::RuntimeSession(const std::string& prefix, const std::string& config_file) {
    EVLOG_warning
        << "everestpy: Usage of the old RuntimeSession ctor detected, config should be loaded via MQTT not via "
           "the provided config_file. For this please set the appropriate environment variables and call "
           "RuntimeSession()";

    // We extract the settings from the config file so everest-testing doesn't break
    const auto ms = Everest::ManagerSettings(prefix, config_file);

    Everest::Logging::init(ms.runtime_settings.logging_config_file.string());

    this->mqtt_settings = ms.mqtt_settings;
}

RuntimeSession::RuntimeSession() {
    const auto module_id = get_variable_from_env("EV_MODULE");

    namespace fs = std::filesystem;
    const fs::path logging_config_file =
        Everest::assert_file(get_variable_from_env("EV_LOG_CONF_FILE"), "Default logging config");
    Everest::Logging::init(logging_config_file.string(), module_id);

    this->mqtt_settings = get_mqtt_settings_from_env();
}

ModuleSetup create_setup_from_config(const std::string& module_id, Everest::Config& config) {
    ModuleSetup setup;

    const std::string& module_name = config.get_module_name(module_id);
    const auto& module_manifest = config.get_manifests().at(module_name);

    // setup connections
    for (const auto& requirement : module_manifest.at("requires").items()) {
        const auto& requirement_id = requirement.key();
        const auto fulfillments = config.resolve_requirement(module_id, requirement_id);

        // Make sure we store the index information in our Fulfillment structures
        std::vector<Fulfillment> indexed_fulfillments;
        indexed_fulfillments.reserve(fulfillments.size());
        for (size_t ii = 0; ii != fulfillments.size(); ++ii) {
            Fulfillment indexed_fulfillment = fulfillments.at(ii);
            indexed_fulfillment.requirement.index = ii;
            EVLOG_verbose << "Setting up " << ii << " implementation_id=" << indexed_fulfillment.implementation_id
                          << ", module_id=" << indexed_fulfillment.module_id;
            indexed_fulfillments.emplace_back(indexed_fulfillment);
        }

        setup.connections[requirement_id] = indexed_fulfillments;
    }

    const auto& module_config = config.get_module_config();

    for (const auto& [impl_id, configuration_parameters] : module_config.configuration_parameters) {
        json json_config_map;
        for (const auto& config_param : configuration_parameters) {
            json_config_map[config_param.name] = config_param.value; // implicit conversion to json
        }
        if (impl_id == "!module") {
            setup.configs.module = json_config_map;
            continue;
        }

        setup.configs.implementations.emplace(impl_id, json_config_map);
    }

    return setup;
}

Interface create_everest_interface_from_definition(const json& def) {
    Interface intf;
    if (def.contains("cmds")) {
        const auto& cmds = def.at("cmds");
        intf.commands.reserve(cmds.size());

        for (const auto& cmd : cmds.items()) {
            intf.commands.push_back(cmd.key());
        }
    }

    if (def.contains("vars")) {
        const auto& vars = def.at("vars");
        intf.variables.reserve(vars.size());

        for (const auto& var : vars.items()) {
            intf.variables.push_back(var.key());
        }
    }

    if (def.contains("errors")) {
        const auto& errors = def.at("errors");

        std::size_t errors_size = 0;
        for (const auto& error_namespace_it : errors.items()) {
            errors_size += error_namespace_it.value().size();
        }
        intf.errors.reserve(errors_size);

        for (const auto& error_namespace_it : errors.items()) {
            for (const auto& error_name_it : error_namespace_it.value().items()) {
                intf.errors.push_back(error_namespace_it.key() + "/" + error_name_it.key());
            }
        }
    }

    return intf;
}
