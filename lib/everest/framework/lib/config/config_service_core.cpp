// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/config/config_service_core.hpp>

#include <date/tz.h>
#include <everest/logging.hpp>

#include <utils/config.hpp>
#include <utils/config/storage_sqlite.hpp>
#include <utils/config/types.hpp>
#include <utils/date.hpp>
#include <utils/yaml_loader.hpp>

namespace Everest::config {

namespace {
std::string now_rfc3339() {
    return Everest::Date::to_rfc3339(date::utc_clock::now());
}
} // namespace

ConfigServiceCore::ConfigServiceCore(everest::config::ModuleConfigurations initial_module_configs,
                                     const ConfigParseSettings& parse_settings,
                                     std::shared_ptr<everest::db::sqlite::ConnectionInterface> db_connection,
                                     std::optional<int> active_slot_id, std::function<StopModulesResult()> stop_fn,
                                     std::function<RestartModulesResult()> restart_fn) :
    module_configs_(std::move(initial_module_configs)),
    parse_settings_(parse_settings),
    slot_manager_(db_connection),
    db_(std::move(db_connection)),
    stop_fn_(std::move(stop_fn)),
    restart_fn_(std::move(restart_fn)) {
    int active_slot_id_from_db = slot_manager_.get_next_boot_slot_id();
    // use the next_boot_slot_id from the db as active_slot_id if none was provided
    if (active_slot_id.has_value()) {
        // Booting with provided active_slot_id
        active_slot_id_ = active_slot_id.value();
    } else {
        // No active_slot_id provided, using stored id from db
        active_slot_id_ = active_slot_id_from_db;
    }
    if (slot_manager_.is_valid(active_slot_id_)) {
        // TODO(CB): This is arguable - this is usually the reset-from-yaml scenario
        // should the config be stored in the currently active slot, or always the default slot?
        if (active_slot_id_ == active_slot_id_from_db) {
            slot_manager_.set_next_boot_slot_id(active_slot_id_);
        }
        active_storage_ = make_storage(active_slot_id_);
    } else {
        // TODO(CB): What do we do now? (in the end it should end up in a confiuration_API-only mode, IF this API is
        // active, otherwise the manager should quit)
    }
}

std::unique_ptr<everest::config::SqliteStorage> ConfigServiceCore::make_storage(int slot_id) {
    return std::make_unique<everest::config::SqliteStorage>(db_, slot_id);
}

void ConfigServiceCore::publish_active_slot_update(const ActiveSlotUpdate& update) {
    for (const auto& handler : active_slot_handlers_) {
        handler(update);
    }
}

void ConfigServiceCore::publish_config_update(const ConfigurationUpdate& update) {
    for (const auto& handler : config_update_handlers_) {
        handler(update);
    }
}

// --- Active-slot in-memory access ---

const everest::config::ModuleConfigurations& ConfigServiceCore::get_active_module_configurations() const {
    return module_configs_;
}

const everest::config::ModuleConfigurations& ConfigServiceCore::reload_from_storage() {
    const auto resp = active_storage_->get_module_configs();
    if (resp.status == everest::config::GenericResponseStatus::OK) {
        module_configs_ = resp.module_configs;
    }
    return module_configs_;
}

// --- Slot management ---

std::vector<SlotInfo> ConfigServiceCore::list_all_slots() {
    return slot_manager_.list_slots();
}

int ConfigServiceCore::get_active_slot_id() {
    return active_slot_id_;
}

SetActiveSlotStatus ConfigServiceCore::mark_active_slot(int slot_id) {
    if (slot_id == active_slot_id_) {
        return SetActiveSlotStatus::AlreadyActive;
    }
    const auto status = slot_manager_.set_next_boot_slot_id(slot_id);
    if (status != everest::config::GenericResponseStatus::OK) {
        return SetActiveSlotStatus::DoesNotExist;
    }
    publish_active_slot_update({now_rfc3339(), slot_id, ActiveSlotStatus::RestartTriggered});
    return SetActiveSlotStatus::Success;
}

DeleteSlotStatus ConfigServiceCore::delete_slot(int slot_id) {
    if (slot_id == active_slot_id_) {
        return DeleteSlotStatus::CannotDeleteActiveSlot;
    }
    // Verify slot exists first
    const auto slots = slot_manager_.list_slots();
    const bool exists = std::any_of(slots.begin(), slots.end(), [slot_id](const auto& s) { return s.id == slot_id; });
    if (!exists) {
        return DeleteSlotStatus::DoesNotExist;
    }
    const auto status = slot_manager_.delete_slot(slot_id);
    if (status != everest::config::GenericResponseStatus::OK) {
        return DeleteSlotStatus::DoesNotExist;
    }
    return DeleteSlotStatus::Success;
}

DuplicateSlotResult ConfigServiceCore::duplicate_slot(int slot_id, std::optional<std::string> description) {
    return slot_manager_.duplicate_slot(slot_id, description);
}

LoadFromYamlResult ConfigServiceCore::load_from_yaml(int slot_id, const std::string& raw_yaml,
                                                     std::optional<std::string> description) {
    if (slot_id == active_slot_id_) {
        return {false, std::nullopt, "Cannot load YAML into the active slot"};
    }
    try {
        const auto json_config = Everest::load_yaml_from_string(raw_yaml);
        if (!json_config.contains("active_modules")) {
            return {false, std::nullopt, "YAML does not contain an 'active_modules' section"};
        }

        // Validate against manifests, interfaces and requirements; enriches configs with manifest metadata.
        const auto module_configs = Everest::validate_module_configs(parse_settings_, json_config);

        bool into_new_slot = !slot_manager_.is_valid(slot_id);

        // If the slot doesn't exist, create it and write the config
        if (into_new_slot) {
            if (slot_manager_.write_config_slot(slot_id) != everest::config::GenericResponseStatus::OK) {
                return {false, std::nullopt, "Failed to create new config slot"};
            }
        }

        auto storage = make_storage(slot_id);
        if (storage->write_module_configs(module_configs) != everest::config::GenericResponseStatus::OK) {
            if (into_new_slot) {
                slot_manager_.delete_slot(slot_id);
            } else {
                // Do nothing - if writing to an existing slot failed, we don't want to delete it;
            }
            return {false, std::nullopt, "Failed to write module configs to slot"};
        }
        storage->mark_valid(true, nlohmann::json(module_configs).dump(), std::nullopt, description);

        return {true, slot_id, ""};
    } catch (const std::exception& e) {
        return {false, std::nullopt, std::string("Validation failed: ") + e.what()};
    }
}

LoadFromYamlResult ConfigServiceCore::load_from_yaml(const std::string& raw_yaml,
                                                     std::optional<std::string> description) {
    int new_slot_id = slot_manager_.next_slot_id();

    return load_from_yaml(new_slot_id, raw_yaml, description);
}

// --- Slot-scoped configuration ---

GetConfigurationResult ConfigServiceCore::get_configuration(int slot_id) {
    const int resolved_slot_id = (slot_id == ConfigServiceInterface::ACTIVE_SLOT) ? active_slot_id_ : slot_id;
    if (resolved_slot_id == active_slot_id_) {
        return {GetConfigurationStatus::Success, module_configs_};
    }

    // Check slot exists
    const auto slots = slot_manager_.list_slots();
    const bool exists =
        std::any_of(slots.begin(), slots.end(), [resolved_slot_id](const auto& s) { return s.id == resolved_slot_id; });
    if (!exists) {
        return {GetConfigurationStatus::SlotDoesNotExist, {}};
    }

    auto storage = make_storage(resolved_slot_id);
    const auto response = storage->get_module_configs();
    if (response.status != everest::config::GenericResponseStatus::OK) {
        return {GetConfigurationStatus::SlotDoesNotExist, {}};
    }
    return {GetConfigurationStatus::Success, response.module_configs};
}

std::vector<SetConfigParameterResult>
ConfigServiceCore::set_config_parameters(int slot_id, const std::vector<ConfigParameterUpdate>& updates) {
    std::vector<SetConfigParameterResult> results;
    results.reserve(updates.size());

    const int resolved_slot_id = (slot_id == ConfigServiceInterface::ACTIVE_SLOT) ? active_slot_id_ : slot_id;

    ConfigurationUpdate event;
    event.timestamp = now_rfc3339();
    event.slot_id = resolved_slot_id;

    if (resolved_slot_id == active_slot_id_) {
        for (const auto& update : updates) {
            const auto mod_it = module_configs_.find(update.identifier.module_id);
            if (mod_it == module_configs_.end()) {
                results.push_back(SetConfigParameterResult::DoesNotExist);
                continue;
            }

            const auto impl_id = update.identifier.module_implementation_id.value_or("!module");
            auto params_it = mod_it->second.configuration_parameters.find(impl_id);
            if (params_it == mod_it->second.configuration_parameters.end()) {
                results.push_back(SetConfigParameterResult::DoesNotExist);
                continue;
            }

            std::optional<everest::config::ConfigurationParameterCharacteristics> characteristics;
            for (const auto& param : params_it->second) {
                if (param.name == update.identifier.configuration_parameter_name) {
                    characteristics = param.characteristics;
                    break;
                }
            }
            if (!characteristics.has_value()) {
                results.push_back(SetConfigParameterResult::DoesNotExist);
                continue;
            }

            // Persist to storage first.
            const auto write_status =
                active_storage_->write_configuration_parameter(update.identifier, *characteristics, update.value);
            if (write_status != everest::config::GetSetResponseStatus::OK) {
                results.push_back(SetConfigParameterResult::Rejected);
                continue;
            }

            SetConfigParameterResult result;
            if (update.immediately_applied) {
                // Module confirmed it immediately applied the value; mutate the in-memory state
                // so module_configs_ reflect the new runtime value.
                const auto parsed_value = everest::config::parse_config_value(characteristics->datatype, update.value);
                for (auto& param : params_it->second) {
                    if (param.name == update.identifier.configuration_parameter_name) {
                        param.value = parsed_value;
                        break;
                    }
                }
                result = SetConfigParameterResult::Applied;
            } else {
                // Value stored for next boot; module_configs_ intentionally maintains current state
                result = SetConfigParameterResult::WillApplyOnRestart;
            }
            results.push_back(result);
            event.updates.push_back({update.identifier, update.value, result});
        }
    } else {
        // Non-active slot: write directly to storage
        auto storage = make_storage(resolved_slot_id);
        const auto existing = storage->get_module_configs();

        for (const auto& update : updates) {
            // Look up characteristics from the stored config
            if (existing.status != everest::config::GenericResponseStatus::OK) {
                results.push_back(SetConfigParameterResult::Rejected);
                continue;
            }
            const auto mod_it = existing.module_configs.find(update.identifier.module_id);
            if (mod_it == existing.module_configs.end()) {
                results.push_back(SetConfigParameterResult::DoesNotExist);
                continue;
            }
            const auto impl_id = update.identifier.module_implementation_id.value_or("!module");
            const auto params_it = mod_it->second.configuration_parameters.find(impl_id);
            if (params_it == mod_it->second.configuration_parameters.end()) {
                results.push_back(SetConfigParameterResult::DoesNotExist);
                continue;
            }
            std::optional<everest::config::ConfigurationParameterCharacteristics> characteristics;
            for (const auto& param : params_it->second) {
                if (param.name == update.identifier.configuration_parameter_name) {
                    characteristics = param.characteristics;
                    break;
                }
            }
            if (!characteristics.has_value()) {
                results.push_back(SetConfigParameterResult::DoesNotExist);
                continue;
            }

            const auto write_status =
                storage->write_configuration_parameter(update.identifier, *characteristics, update.value);
            if (write_status == everest::config::GetSetResponseStatus::OK) {
                // For non-active slots results are not applied immediately, so we return WillApplyOnRestart
                results.push_back(SetConfigParameterResult::WillApplyOnRestart);
                event.updates.push_back(
                    {update.identifier, update.value, SetConfigParameterResult::WillApplyOnRestart});
            } else {
                results.push_back(SetConfigParameterResult::Rejected);
            }
        }
    }

    if (!event.updates.empty()) {
        publish_config_update(event);
    }

    return results;
}

// --- Module lifecycle ---

StopModulesResult ConfigServiceCore::stop_modules() {
    if (stop_fn_) {
        return stop_fn_();
    }
    return StopModulesResult::Rejected;
}

RestartModulesResult ConfigServiceCore::restart_modules() {
    if (restart_fn_) {
        return restart_fn_();
    }
    return RestartModulesResult::Rejected;
}

// --- Push-event subscriptions ---

void ConfigServiceCore::register_active_slot_update_handler(std::function<void(const ActiveSlotUpdate&)> handler) {
    active_slot_handlers_.push_back(std::move(handler));
}

void ConfigServiceCore::register_config_update_handler(std::function<void(const ConfigurationUpdate&)> handler) {
    config_update_handlers_.push_back(std::move(handler));
}

} // namespace Everest::config
