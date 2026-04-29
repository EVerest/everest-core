// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/// \brief CLI tool for managing EVerest configurations stored in a SQLite database.
///
/// Commands:
///   add       Parse, validate and store a YAML config as a new slot
///   validate  Validate a YAML config without storing it
///   list      List all stored config slots with their metadata
///   delete    Delete a config slot and all its associated data

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <boost/log/core.hpp>
#include <boost/program_options.hpp>
#include <fmt/core.h>

#include <framework/runtime.hpp>
#include <utils/config.hpp>
#include <utils/config/slot_manager.hpp>
#include <utils/config/storage_sqlite.hpp>
#include <utils/config/storage_types.hpp>

namespace po = boost::program_options;
namespace fs = std::filesystem;

using namespace Everest;
using namespace everest::config;

// ---------------------------------------------------------------------------
// Command implementations
// ---------------------------------------------------------------------------

static int cmd_add(const std::string& prefix, const std::string& config_file, const std::string& db_path) {
    fmt::print("Validating config '{}'...\n", config_file);

    ManagerSettings ms;
    ms.init_prefix_and_data_dir(prefix);
    ms.init_config_file(config_file);
    const auto settings = everest::config::parse_settings(ms.config.value("settings", nlohmann::json::object()));
    ms.init_settings(settings);
    ms.db_dir = fs::path(db_path);

    const auto migrations_dir = ms.runtime_settings.data_dir / "migrations";

    SqliteConfigSlotManager slot_mgr(ms.db_dir, migrations_dir);
    const int slot_id = slot_mgr.next_slot_id();

    if (slot_mgr.write_config_slot(slot_id) != GenericResponseStatus::OK) {
        fmt::print(stderr, "Error: Failed to write config slot {}.\n", slot_id);
        return 1;
    }

    auto storage = std::make_unique<SqliteStorage>(ms.db_dir, migrations_dir, slot_id);

    fmt::print("Storing as slot {} in '{}'...\n", slot_id, db_path);
    ManagerConfig cfg(ms);

    const auto& module_configs = cfg.get_module_configurations();
    if (storage->write_module_configs(module_configs) != GenericResponseStatus::OK) {
        fmt::print(stderr, "Error: Failed to write module configs to slot {}.\n", slot_id);
        slot_mgr.delete_slot(slot_id);
        return 1;
    }
    storage->mark_valid(true, nlohmann::json(module_configs).dump(), ms.config_file, std::nullopt);

    fmt::print("Done. Config stored as slot {}.\n", slot_id);
    return 0;
}

static int cmd_validate(const std::string& prefix, const std::string& config_file) {
    fmt::print("Validating config '{}'...\n", config_file);

    // Build a ManagerSettings to resolve all paths from prefix + YAML settings section,
    // then pass the ConfigParseSettings slice to ManagerConfig.
    ManagerSettings ms;
    ms.init_prefix_and_data_dir(prefix);
    ms.init_config_file(config_file);
    const auto settings = everest::config::parse_settings(ms.config.value("settings", nlohmann::json::object()));
    ms.init_settings(settings);
    ManagerConfig cfg(static_cast<ConfigParseSettings&>(ms));

    fmt::print("Config is valid.\n");
    return 0;
}

static int cmd_list(const std::string& prefix, const std::string& db_path) {
    ManagerSettings ms;
    ms.init_prefix_and_data_dir(prefix);
    const auto migrations_dir = ms.runtime_settings.data_dir / "migrations";

    SqliteConfigSlotManager slot_mgr(db_path, migrations_dir);
    const auto slots = slot_mgr.list_slots();

    if (slots.empty()) {
        fmt::print("No config slots found in '{}'.\n", db_path);
        return 0;
    }

    fmt::print("{:<6} {:<32} {:<7} {}\n", "SLOT", "LAST UPDATED", "VALID", "CONFIG FILE");
    fmt::print("{}\n", std::string(80, '-'));
    for (const auto& slot : slots) {
        fmt::print("{:<6} {:<32} {:<7} {}\n", slot.id, slot.last_updated.empty() ? "-" : slot.last_updated,
                   slot.is_valid ? "yes" : "no", slot.config_file_path.value_or("-"));
    }
    return 0;
}

static int cmd_delete(const std::string& prefix, const std::string& db_path, int slot_id) {
    ManagerSettings ms;
    ms.init_prefix_and_data_dir(prefix);
    const auto migrations_dir = ms.runtime_settings.data_dir / "migrations";

    SqliteConfigSlotManager slot_mgr(db_path, migrations_dir);

    const auto existing = slot_mgr.list_slots();
    const bool found =
        std::any_of(existing.begin(), existing.end(), [slot_id](const StoredSlotInfo& s) { return s.id == slot_id; });
    if (!found) {
        fmt::print(stderr, "Error: Slot {} does not exist.\n", slot_id);
        return 1;
    }

    if (slot_mgr.delete_slot(slot_id) != GenericResponseStatus::OK) {
        fmt::print(stderr, "Error: Failed to delete slot {}.\n", slot_id);
        return 1;
    }

    fmt::print("Slot {} deleted.\n", slot_id);
    return 0;
}

// ---------------------------------------------------------------------------
// Help text
// ---------------------------------------------------------------------------

static void print_usage(const po::options_description& global_opts) {
    fmt::print("Usage: everest-config-tool [--prefix <path>] <command> [command options]\n\n"
               "Commands:\n"
               "  add       --config <yaml> --db <path>   Validate and store a YAML config as a new slot\n"
               "  validate  --config <yaml>               Validate a YAML config without storing\n"
               "  list      --db <path>                   List all stored config slots\n"
               "  delete    --db <path> --slot <id>       Delete a config slot\n\n");
    std::cout << global_opts << "\n";
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    // Suppress all framework EVLOG_* output
    boost::log::core::get()->set_logging_enabled(false);

    po::options_description global_opts("Global options");
    // clang-format off
    global_opts.add_options()
        ("help,h",   "Show this help and exit")
        ("prefix,p", po::value<std::string>()->default_value(""),
                     "EVerest installation prefix (default: system default)")
        ("command",  po::value<std::string>(), "Command: add | validate | list | delete")
        ("subargs",  po::value<std::vector<std::string>>(), "Arguments forwarded to the command");
    // clang-format on

    po::positional_options_description pos;
    pos.add("command", 1).add("subargs", -1);

    try {
        po::variables_map vm;
        auto parsed =
            po::command_line_parser(argc, argv).options(global_opts).positional(pos).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);

        if (vm.count("help") || !vm.count("command")) {
            print_usage(global_opts);
            return vm.count("help") ? 0 : 1;
        }

        const std::string command = vm["command"].as<std::string>();
        const std::string prefix = vm["prefix"].as<std::string>();
        // Remaining tokens (after the command name) are forwarded to the sub-parser.
        const auto remaining = po::collect_unrecognized(parsed.options, po::include_positional);

        if (command == "add") {
            po::options_description opts("add options");
            // clang-format off
            opts.add_options()
                ("config,c", po::value<std::string>()->required(), "Path to YAML config file")
                ("db,d",     po::value<std::string>()->required(), "Path to SQLite database file");
            // clang-format on
            po::variables_map sub_vm;
            po::store(po::command_line_parser(remaining).options(opts).run(), sub_vm);
            po::notify(sub_vm);
            return cmd_add(prefix, sub_vm["config"].as<std::string>(), sub_vm["db"].as<std::string>());

        } else if (command == "validate") {
            po::options_description opts("validate options");
            // clang-format off
            opts.add_options()
                ("config,c", po::value<std::string>()->required(), "Path to YAML config file");
            // clang-format on
            po::variables_map sub_vm;
            po::store(po::command_line_parser(remaining).options(opts).run(), sub_vm);
            po::notify(sub_vm);
            return cmd_validate(prefix, sub_vm["config"].as<std::string>());

        } else if (command == "list") {
            po::options_description opts("list options");
            // clang-format off
            opts.add_options()
                ("db,d", po::value<std::string>()->required(), "Path to SQLite database file");
            // clang-format on
            po::variables_map sub_vm;
            po::store(po::command_line_parser(remaining).options(opts).run(), sub_vm);
            po::notify(sub_vm);
            return cmd_list(prefix, sub_vm["db"].as<std::string>());

        } else if (command == "delete") {
            po::options_description opts("delete options");
            // clang-format off
            opts.add_options()
                ("db,d",   po::value<std::string>()->required(), "Path to SQLite database file")
                ("slot,s", po::value<int>()->required(),         "Slot ID to delete");
            // clang-format on
            po::variables_map sub_vm;
            po::store(po::command_line_parser(remaining).options(opts).run(), sub_vm);
            po::notify(sub_vm);
            return cmd_delete(prefix, sub_vm["db"].as<std::string>(), sub_vm["slot"].as<int>());

        } else {
            fmt::print(stderr, "Error: Unknown command '{}'. Run with --help for usage.\n", command);
            return 1;
        }

    } catch (const po::error& e) {
        fmt::print(stderr, "Argument error: {}\n", e.what());
        return 1;
    } catch (const std::exception& e) {
        fmt::print(stderr, "Error: {}\n", e.what());
        return 1;
    }
}
