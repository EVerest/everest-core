// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef EVERESTPY_MISC_HPP
#define EVERESTPY_MISC_HPP

#include <string>

#include <framework/runtime.hpp>

class RuntimeSession {
public:
    RuntimeSession(const std::string& prefix, const std::string& config_file);

    RuntimeSession();

    std::shared_ptr<Everest::RuntimeSettings> get_runtime_settings() const {
        return rs;
    }

    Everest::Config& get_config() const {
        return *config;
    }

private:
    std::shared_ptr<Everest::RuntimeSettings> rs;
    std::unique_ptr<Everest::Config> config;

    static std::unique_ptr<Everest::Config> create_config_instance(std::shared_ptr<Everest::RuntimeSettings> rs);
};

struct Fulfillment {
    std::string module_id;
    std::string implementation_id;
    Requirement requirement;
};

struct Interface {
    std::vector<std::string> variables;
    std::vector<std::string> commands;
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
