// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include "misc.hpp"

#include <cstdlib>
#include <stdexcept>

static std::string get_ev_prefix_from_env() {
    const auto prefix = std::getenv("EV_PREFIX");
    if (prefix == nullptr) {
        throw std::runtime_error("EV_PREFIX needed for everestpy");
    }

    return prefix;
}

static std::string get_ev_conf_file_from_env() {
    const auto config_file = std::getenv("EV_CONF_FILE");
    if (config_file == nullptr) {
        throw std::runtime_error("EV_CONF_FILE needed for everestpy");
    }

    return config_file;
}

RuntimeSession::RuntimeSession(const std::string& prefix, const std::string& config_file) :
    rs(std::make_shared<Everest::RuntimeSettings>(prefix, config_file)), config(create_config_instance(rs)) {
}

RuntimeSession::RuntimeSession() : RuntimeSession(get_ev_prefix_from_env(), get_ev_conf_file_from_env()) {
}

std::unique_ptr<Everest::Config> RuntimeSession::create_config_instance(std::shared_ptr<Everest::RuntimeSettings> rs) {
    // FIXME (aw): where to initialize the logger?
    Everest::Logging::init(rs->logging_config_file);
    return std::make_unique<Everest::Config>(rs);
}

ModuleSetup create_setup_from_config(const std::string& module_id, Everest::Config& config) {
    ModuleSetup setup;

    const std::string& module_name = config.get_main_config().at(module_id).at("module");
    auto module_manifest = config.get_manifests().at(module_name);

    // setup connections
    for (auto& requirement : module_manifest.at("requires").items()) {

        auto const& requirement_id = requirement.key();

        json req_route_list = config.resolve_requirement(module_id, requirement_id);
        // if this was a requirement with min_connections == 1 and max_connections == 1,
        // this will be simply a single connection, but an array of connections otherwise
        // (this array can have only one entry, if only one connection was provided, though)
        const bool is_list = req_route_list.is_array();
        if (!is_list) {
            req_route_list = json::array({req_route_list});
        }

        auto fulfillment_list_it = setup.connections.insert({requirement_id, {}}).first;
        auto& fulfillment_list = fulfillment_list_it->second;
        fulfillment_list.reserve(req_route_list.size());

        for (size_t i = 0; i < req_route_list.size(); i++) {
            const auto& req_route = req_route_list[i];
            auto fulfillment = Fulfillment{req_route["module_id"], req_route["implementation_id"], {requirement_id, i}};
            fulfillment_list.emplace_back(std::move(fulfillment));
        }
    }

    const auto& config_maps = config.get_module_json_config(module_id);

    for (const auto& config_map : config_maps.items()) {
        const auto& impl_id = config_map.key();
        if (impl_id == "!module") {
            setup.configs.module = config_map.value();
            continue;
        }

        setup.configs.implementations.emplace(impl_id, config_map.value());
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

        int errors_size = 0;
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
