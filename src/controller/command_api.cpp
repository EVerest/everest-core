// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "command_api.hpp"

#include <filesystem>
#include <fstream>

#include <fmt/core.h>

#include <ryml.hpp>
#include <ryml_std.hpp>

#include "rpc.hpp"

#include <utils/formatter.hpp>
#include <utils/yaml_loader.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

static void clear_quote_flags(ryml::NodeRef& root) {
    if (root.has_val() && root.val().is_number()) {
        // we don't want to trim away strings, that are a pure number
        root.tree()->_rem_flags(root.id(), ryml::KEYQUO);
    } else {
        root.tree()->_rem_flags(root.id(), ryml::KEYQUO | ryml::VALQUO);
    }

    for (auto child : root.children()) {
        clear_quote_flags(child);
    }
}

CommandApi::CommandApi(const Config& config, RPC& rpc) : config(config), rpc(rpc) {
}

nlohmann::json CommandApi::handle(const std::string& cmd, const json& params) {
    // fmt::print("Handling command {}\n", cmd);

    if (cmd == "get_modules") {
        auto modules_list = json::object();

        for (auto item : fs::directory_iterator(this->config.module_dir)) {
            if (!fs::is_directory(item)) {
                continue;
            }
            const auto module_path = item.path();
            const auto module_name = module_path.filename().string();

            // fetch the manifest
            const auto manifest_path = module_path / "manifest.yaml";
            if (!fs::is_regular_file(manifest_path)) {
                continue;
            }

            modules_list[module_name] = Everest::load_yaml(manifest_path);
        }

        return modules_list;
    } else if (cmd == "get_configs") {
        auto config_list = json::object();

        for (auto item : fs::directory_iterator(this->config.configs_dir)) {
            if (!fs::is_regular_file(item)) {
                continue;
            }
            if (item.path().extension() != std::string(".yaml")) {
                continue;
            }

            const auto config_name = item.path().stem().string();

            config_list[config_name] = Everest::load_yaml(item.path());
        }

        return config_list;
    } else if (cmd == "get_interfaces") {
        auto interface_list = json::object();

        for (auto item : fs::directory_iterator(this->config.interface_dir)) {

            if (!fs::is_regular_file(item)) {
                continue;
            }
            if (item.path().extension() != std::string(".yaml")) {
                continue;
            }

            const auto interface_name = item.path().stem().string();

            interface_list[interface_name] = Everest::load_yaml(item.path());
        }

        return interface_list;
    } else if (cmd == "save_config") {
        // FIXME (aw): this is quite hacky
        if (!params.contains("name") || !params.at("name").is_string()) {
            throw CommandApiParamsError("The save_config needs a 'name' parameter for the config file of type string");
        }

        auto config_json = params.value("config", json::object());
        const auto configs_path = fs::path(this->config.configs_dir);
        auto check_config_file_path = configs_path / fmt::format("_{}.yaml", params.at("name"));

        const auto json_serialized = config_json.dump();
        auto ryml_deserialized = ryml::parse_in_arena(ryml::to_csubstr(json_serialized));
        auto root = ryml_deserialized.rootref();
        clear_quote_flags(root);

        std::ofstream(check_config_file_path.string()) << ryml_deserialized;

        auto result = this->rpc.ipc_request("check_config", check_config_file_path.string(), false);

        if (result.is_string()) {
            fs::remove(check_config_file_path);
            throw CommandApiParamsError(result);
        }

        fs::rename(check_config_file_path, configs_path / fmt::format("{}.yaml", params.at("name")));

        return true;
    } else if (cmd == "restart_modules") {
        this->rpc.ipc_request("restart_modules", nullptr, true);

        return nullptr;
    } else if (cmd == "get_rpc_timeout") {
        return this->config.controller_rpc_timeout_ms;
    }

    throw CommandApiMethodNotFound(fmt::format("Command '{}' unknown", cmd));
}
