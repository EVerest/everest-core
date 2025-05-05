// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVERESTPY_MISC_HPP
#define EVERESTPY_MISC_HPP

#include <string>

#include <framework/runtime.hpp>
#include <utils/config/mqtt_settings.hpp>
#include <utils/types.hpp>

const std::string get_variable_from_env(const std::string& variable);
const std::string get_variable_from_env(const std::string& variable, const std::string& default_value);

class RuntimeSession {
public:
    RuntimeSession(const std::string& prefix, const std::string& config_file);

    RuntimeSession();

    const Everest::MQTTSettings& get_mqtt_settings() const {
        return mqtt_settings;
    }

private:
    Everest::MQTTSettings mqtt_settings;
};

struct Interface {
    std::vector<std::string> variables;
    std::vector<std::string> commands;
    std::vector<std::string> errors;
};

Interface create_everest_interface_from_definition(const json& def);

struct ModuleSetup {
    struct Configurations {
        std::map<std::string, json> implementations;
        json module;
    };

    Configurations configs;

    std::map<std::string, std::vector<Fulfillment>> connections;
};

ModuleSetup create_setup_from_config(const std::string& module_id, Everest::Config& config);

#endif // EVERESTPY_MISC_HPP
