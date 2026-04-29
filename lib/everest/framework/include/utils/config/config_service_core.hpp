// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <utils/config/settings.hpp>
#include <utils/config/slot_manager.hpp>
#include <utils/config/storage_sqlite.hpp>
#include <utils/config/types.hpp>
#include <utils/config_service_interface.hpp>

namespace Everest::config {

/// \brief Core framework implementation of ConfigServiceInterface.
///
/// Owns a SqliteConfigSlotManager and uses a SqliteStorage factory for per-slot storage access.
/// Owns the active-slot ModuleConfigurations as the single in-memory runtime authority.
class ConfigServiceCore : public ConfigServiceInterface {
public:
    /// \param initial_module_configs Initial module configurations for the active slot.
    /// \param parse_settings Parse settings used to validate incoming YAML configs (paths to schemas, modules, etc.).
    /// \param db_connection  Shared, already-migrated SQLite connection (from open_config_database()).
    /// \param active_slot_id Slot ID that is currently booted.
    /// \param stop_fn        Callback to stop running modules (optional stub).
    /// \param restart_fn     Callback to restart modules (optional stub).
    ConfigServiceCore(everest::config::ModuleConfigurations initial_module_configs,
                      const ConfigParseSettings& parse_settings,
                      std::shared_ptr<everest::db::sqlite::ConnectionInterface> db_connection, std::optional<int> active_slot_id,
                      std::function<StopModulesResult()> stop_fn = {},
                      std::function<RestartModulesResult()> restart_fn = {});

    // --- Active-slot in-memory access (zero-copy) ---
    const everest::config::ModuleConfigurations& get_active_module_configurations() const override;
    const everest::config::ModuleConfigurations& reload_from_storage() override;

    // --- Slot management ---
    std::vector<SlotInfo> list_all_slots() override;
    int get_active_slot_id() override;
    SetActiveSlotStatus mark_active_slot(int slot_id) override;
    DeleteSlotStatus delete_slot(int slot_id) override;
    DuplicateSlotResult duplicate_slot(int slot_id, std::optional<std::string> description) override;
    LoadFromYamlResult load_from_yaml(const std::string& raw_yaml) override;

    // --- Slot-scoped configuration ---
    GetConfigurationResult get_configuration(int slot_id) override;
    std::vector<SetConfigParameterResult>
    set_config_parameters(int slot_id, const std::vector<ConfigParameterUpdate>& updates) override;

    // --- Module lifecycle ---
    StopModulesResult stop_modules() override;
    RestartModulesResult restart_modules() override;

    // --- Push-event subscriptions ---
    void register_active_slot_update_handler(std::function<void(const ActiveSlotUpdate&)> handler) override;
    void register_config_update_handler(std::function<void(const ConfigurationUpdate&)> handler) override;

private:
    everest::config::ModuleConfigurations module_configs_;
    ConfigParseSettings parse_settings_;
    everest::config::SqliteConfigSlotManager slot_manager_;
    /// \brief Keepalive for the shared connection
    std::shared_ptr<everest::db::sqlite::ConnectionInterface> db_;
    int active_slot_id_;
    std::function<StopModulesResult()> stop_fn_;
    std::function<RestartModulesResult()> restart_fn_;

    std::vector<std::function<void(const ActiveSlotUpdate&)>> active_slot_handlers_;
    std::vector<std::function<void(const ConfigurationUpdate&)>> config_update_handlers_;

    std::unique_ptr<everest::config::SqliteStorage> make_storage(int slot_id);
    void publish_active_slot_update(const ActiveSlotUpdate& update);
    void publish_config_update(const ConfigurationUpdate& update);

    /// \brief Storage handle for the currently active slot, used to persist runtime config writes.
    std::unique_ptr<everest::config::SqliteStorage> active_storage_;
};

} // namespace Everest::config
