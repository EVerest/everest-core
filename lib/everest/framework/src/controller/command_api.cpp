// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#include "command_api.hpp"

#include <filesystem>
#include <fstream>

#include <fmt/core.h>

#include <ryml.hpp>
#include <ryml_std.hpp>

#include "rpc.hpp"
#include "transpile_config.hpp"

#include <utils/formatter.hpp>
#include <utils/yaml_loader.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

CommandApi::CommandApi(const Config& config, RPC& rpc) : config(config), rpc(rpc) {
}

nlohmann::json CommandApi::handle(const std::string& cmd, const json& params) {
    // fmt::print("Handling command {}\n", cmd);

    if (cmd == "get_modules") {
        auto modules_list = json::object();
        auto ensure_object_field = [](auto& target, const std::string& key) {
            if (!target.contains(key) || !target.at(key).is_object()) {
                target[key] = json::object();
            }
        };

        for (const auto& item : fs::directory_iterator(this->config.module_dir)) {
            if (!fs::is_directory(item)) {
                continue;
            }
            const auto& module_path = item.path();
            const auto module_name = module_path.filename().string();

            // fetch the manifest
            const auto manifest_path = module_path / "manifest.yaml";
            if (!fs::is_regular_file(manifest_path)) {
                continue;
            }

            auto module_manifest = Everest::load_yaml(manifest_path);
            if (!module_manifest.is_object()) {
                module_manifest = json::object();
            }

            // Admin panel expects module interface maps to be object-like.
            ensure_object_field(module_manifest, "provides");
            ensure_object_field(module_manifest, "requires");
            ensure_object_field(module_manifest, "metadata");

            modules_list[module_name] = module_manifest;
        }

        return modules_list;
    } else if (cmd == "get_configs") {
        auto config_list = json::object();
        auto ensure_object_field = [](auto& target, const std::string& key) {
            if (!target.contains(key) || !target.at(key).is_object()) {
                target[key] = json::object();
            }
        };

        for (const auto& item : fs::directory_iterator(this->config.configs_dir)) {
            if (!fs::is_regular_file(item)) {
                continue;
            }
            if (item.path().extension() != std::string(".yaml")) {
                continue;
            }

            const auto config_name = item.path().stem().string();
            auto loaded_config = Everest::load_yaml(item.path());
            if (!loaded_config.is_object()) {
                loaded_config = json::object();
            }

            // Keep response schema stable for admin panel code that iterates over object fields.
            ensure_object_field(loaded_config, "active_modules");
            ensure_object_field(loaded_config, "settings");
            ensure_object_field(loaded_config, "x-module-layout");

            auto& active_modules = loaded_config["active_modules"];
            for (auto& [_, module_entry] : active_modules.items()) {
                if (!module_entry.is_object()) {
                    module_entry = json::object();
                }
                ensure_object_field(module_entry, "connections");
            }

            config_list[config_name] = loaded_config;
        }

        return config_list;
    } else if (cmd == "get_interfaces") {
        auto interface_list = json::object();

        for (const auto& item : fs::directory_iterator(this->config.interface_dir)) {

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

        const auto name = params.at("name").get<std::string>();

        const json config_json = params.value("config", json::object());
        auto ryml_deserialized = transpile_config(config_json);

        const auto configs_path = fs::path(this->config.configs_dir);
        const auto check_config_file_path = configs_path / fmt::format("_{}.yaml", name);

        std::ofstream(check_config_file_path.string()) << ryml_deserialized;

        const auto result = this->rpc.ipc_request("check_config", check_config_file_path.string(), false);

        if (result.is_string()) {
            fs::remove(check_config_file_path);
            throw CommandApiParamsError(result);
        }

        fs::rename(check_config_file_path, configs_path / fmt::format("{}.yaml", name));

        return true;
    } else if (cmd == "restart_modules") {
        this->rpc.ipc_request("restart_modules", nullptr, true);

        return json{{"accepted", true}};
    } else if (cmd == "get_rpc_timeout") {
        return this->config.controller_rpc_timeout_ms;
    }

    throw CommandApiMethodNotFound(fmt::format("Command '{}' unknown", cmd));
}
