#include "command_handlers.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

namespace everest::lib::API::V1_0::types::configuration {
inline bool operator==(const ConfigurationParameterIdentifier& a, const ConfigurationParameterIdentifier& b) {
    return a.module_id == b.module_id && a.parameter_name == b.parameter_name &&
           a.implementation_id == b.implementation_id;
}
} // namespace everest::lib::API::V1_0::types::configuration

namespace {

using namespace everest::lib::API::V1_0::types::configuration;

struct ParamInfo {
    std::string value;
    ConfigurationParameterDatatype datatype{ConfigurationParameterDatatype::Unknown};
};

ParamInfo lookup_param_in_config(const GetConfigurationResult& cfg,
                                 const ConfigurationParameterIdentifier& id) {
    if (!cfg.module_configurations.has_value()) {
        return {};
    }
    const auto& mods = *cfg.module_configurations;
    auto mod_it =
        std::find_if(mods.begin(), mods.end(), [&id](const auto& m) { return m.module_id == id.module_id; });
    if (mod_it == mods.end()) {
        return {};
    }
    auto find_param = [&id](const auto& params) -> ParamInfo {
        auto it =
            std::find_if(params.begin(), params.end(), [&id](const auto& p) { return p.name == id.parameter_name; });
        return it != params.end() ? ParamInfo{it->value, it->characteristics.datatype} : ParamInfo{};
    };
    if (!id.implementation_id.has_value()) {
        return find_param(mod_it->module_configuration_parameters);
    }
    const auto& impls = mod_it->implementation_configuration_parameters;
    auto impl_it = std::find_if(impls.begin(), impls.end(),
                                [&id](const auto& impl) { return impl.implementation_id == *id.implementation_id; });
    return impl_it != impls.end() ? find_param(impl_it->configuration_parameters) : ParamInfo{};
}

bool param_value_changed(const ParamInfo& before, const std::string& new_value) {
    if (before.datatype == ConfigurationParameterDatatype::Decimal) {
        try {
            return std::stod(before.value) != std::stod(new_value);
        } catch (const std::exception&) {
        }
    }
    return before.value != new_value;
}

struct FilteredUpdates {
    ConfigurationParameterUpdateRequest changed_req;
    std::vector<ParamInfo> before_params;
};

FilteredUpdates filter_changed_updates(const ConfigurationParameterUpdateRequest& req,
                                       const GetConfigurationResult& current_cfg) {
    FilteredUpdates result;
    result.changed_req.slot_id = req.slot_id;
    for (const auto& update : req.parameter_updates) {
        ParamInfo before = lookup_param_in_config(current_cfg, update.cfg_param_id);
        if (param_value_changed(before, update.value)) {
            result.changed_req.parameter_updates.push_back(update);
            result.before_params.push_back(before);
        }
    }
    return result;
}

void print_update_results(const ConfigurationParameterUpdateRequest& changed_req,
                          const ConfigurationParameterUpdateRequestResult& res,
                          const std::vector<ParamInfo>& before_params) {
    const char* GREEN = "\033[32m";
    const char* YELLOW = "\033[33m";
    const char* RED = "\033[31m";
    const char* BOLD = "\033[1m";
    const char* RESET = "\033[0m";

    int applied_count = 0;
    int ok_count = 0;
    for (std::size_t i = 0; i < res.results.size(); ++i) {
        const auto& update = changed_req.parameter_updates.at(i);
        const auto& result = res.results.at(i);
        const auto& id = update.cfg_param_id;

        std::string param_id = id.module_id + "|";
        if (id.implementation_id.has_value()) {
            param_id += *id.implementation_id + "|";
        }
        param_id += id.parameter_name;

        const char* color = GREEN;
        if (result == ConfigurationParameterUpdateResultEnum::WillApplyOnRestart) {
            color = YELLOW;
            ++ok_count;
        } else if (result == ConfigurationParameterUpdateResultEnum::DoesNotExist ||
                   result == ConfigurationParameterUpdateResultEnum::Rejected) {
            color = RED;
        } else {
            ++ok_count;
            ++applied_count;
        }

        const auto& before = before_params.at(i);
        std::cout << BOLD << std::left << std::setw(50) << param_id << RESET << " [" << serialize(before.datatype)
                  << "] : " << before.value << " -> " << update.value << "  " << color << serialize(result) << RESET
                  << "\n";
    }
    std::cout << "\nChanged " << ok_count << " of " << res.results.size() << " parameter(s), " << applied_count
              << " applied (immediately).\n";
}

} // namespace

namespace everest::config_cli {

CommandHandlers::CommandHandlers(std::shared_ptr<IConfigServiceClient> client,
                                 std::shared_ptr<IYamlProvider> yaml_provider) :
    m_client(std::move(client)), m_yaml_provider(std::move(yaml_provider)) {
}

void CommandHandlers::list_slots() {
    auto res = m_client->list_all_slots();
    if (!res) {
        std::cerr << "Failed to list slots; API did not respond.\n";
        return;
    }
    std::cout << "Available slots:\n";
    for (const auto& meta : res->slots) {
        std::cout << "  [" << meta.slot_id << "] " << meta.description.value_or("<no description>") << "\n";
    }
}

void CommandHandlers::show_slot_metadata(int slot_id) {
    auto res = m_client->list_all_slots();
    if (!res) {
        std::cerr << "Failed to get slots metadata; API did not respond.\n";
        return;
    }
    if (res->slots.empty()) {
        std::cout << "No slots found.\n";
        return;
    }
    auto it =
        std::find_if(res->slots.begin(), res->slots.end(), [slot_id](const auto& s) { return s.slot_id == slot_id; });
    if (it == res->slots.end()) {
        std::cerr << "Slot [" << slot_id << "] not found.\n";
        return;
    }
    std::cout << "Slot metadata:\n"
              << "  Slot ID      : " << it->slot_id << "\n"
              << "  Description  : " << it->description.value_or("<no description>") << "\n"
              << "  Last Updated : " << it->last_updated << "\n"
              << "  Is Valid     : " << (it->is_valid ? "Yes" : "No") << "\n"
              << "  Config File  : " << it->config_file_path.value_or("<no config file>") << "\n";
}

void CommandHandlers::active_slot() {
    auto res = m_client->get_active_slot();
    if (!res) {
        std::cerr << "Failed to get active slot; API did not respond.\n";
        return;
    }
    if (res->slot_id.has_value()) {
        std::cout << "Active slot: [" << res->slot_id.value() << "]\n";
    } else {
        std::cout << "No active slot configured.\n";
    }
}

void CommandHandlers::mark_active_slot(int slot_id) {
    auto res = m_client->mark_active_slot(slot_id);
    if (!res) {
        std::cerr << "Failed to mark active slot; API did not respond.\n";
        return;
    }
    if (res->result == everest::lib::API::V1_0::types::configuration::MarkActiveSlotResultEnum::Success) {
        std::cout << "Successfully marked slot " << slot_id << " as active.\n";
    } else {
        std::cout << "Failed to mark slot " << slot_id << " as active.\n";
    }
}

void CommandHandlers::delete_slot(int slot_id) {
    auto res = m_client->delete_slot(slot_id);
    if (!res) {
        std::cerr << "Failed to delete slot; API did not respond.\n";
        return;
    }
    if (res->result == everest::lib::API::V1_0::types::configuration::DeleteSlotResultEnum::Success) {
        std::cout << "Successfully deleted slot " << slot_id << ".\n";
    } else {
        std::cout << "Failed to delete slot " << slot_id << "\n";
    }
}

void CommandHandlers::duplicate_slot(int slot_id, const std::string& description) {
    std::string desc = description.empty() ? ("duplicate of slot " + std::to_string(slot_id)) : description;
    auto res = m_client->duplicate_slot(slot_id, desc);
    if (!res) {
        std::cerr << "Failed to duplicate slot; API did not respond.\n";
        return;
    }
    if (res->success && res->slot_id.has_value()) {
        std::cout << "Successfully duplicated to slot " << res->slot_id.value() << " with description: " << desc
                  << "\n";
    } else {
        std::cout << "Failed to duplicate slot " << slot_id << "\n";
    }
}

void CommandHandlers::load_yaml(const std::string& filename, const std::string& description, std::optional<int> slot_id) {
    try {
        std::string raw_yaml = m_yaml_provider->extract_active_modules_string(filename);
        std::string desc = description.empty() ? ("loaded from " + filename) : description;

        auto res = m_client->load_from_yaml(raw_yaml, desc, slot_id);
        if (!res) {
            std::cerr << "Failed to load YAML; API did not respond.\n";
            return;
        }
        if (res->success && res->slot_id.has_value()) {
            std::cout << "Successfully loaded YAML to slot " << res->slot_id.value() << " with description: " << desc
                      << "\n";
        } else {
            std::cout << "Failed to load YAML: " << res->error_message << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing YAML file: " << e.what() << "\n";
    }
}

void CommandHandlers::get_configuration(int slot_id) {
    auto res = m_client->get_configuration(slot_id);
    if (!res) {
        std::cerr << "Failed to get configuration for slot " << slot_id << "; API did not respond.\n";
        return;
    }
    if (res->status == everest::lib::API::V1_0::types::configuration::GetConfigurationStatusEnum::SlotDoesNotExist) {
        std::cout << "Slot " << slot_id << " not found.\n";
        return;
    }

    try {
        std::string yaml_str = m_yaml_provider->format_configuration(*res);
        std::cout << yaml_str << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error formatting configuration: " << e.what() << "\n";
    }
}

void CommandHandlers::set_config_parameter(int slot_id, const std::string& filename) {
    everest::lib::API::V1_0::types::configuration::ConfigurationParameterUpdateRequest req;
    try {
        req = m_yaml_provider->parse_parameter_updates(slot_id, filename);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing update file: " << e.what() << "\n";
        return;
    }

    auto current_cfg = m_client->get_configuration(slot_id);
    if (!current_cfg) {
        std::cerr << "Failed to get configuration for slot " << slot_id << "; API did not respond.\n";
        return;
    }
    if (current_cfg->status ==
        everest::lib::API::V1_0::types::configuration::GetConfigurationStatusEnum::SlotDoesNotExist) {
        std::cerr << "Slot [" << slot_id << "] does not exist.\n";
        return;
    }

    auto [changed_req, before_params] = filter_changed_updates(req, *current_cfg);
    if (changed_req.parameter_updates.empty()) {
        std::cout << "No parameters changed — nothing to update.\n";
        return;
    }

    auto res = m_client->set_config_parameters(changed_req);
    if (!res) {
        std::cerr << "Failed to set config parameters; API did not respond.\n";
        return;
    }

    print_update_results(changed_req, *res, before_params);
}

void CommandHandlers::monitor(bool suppress_parameter_updates) {
    std::cout << "Starting monitor... (Press Ctrl+C to stop)\n";

    auto active_cb = [](const everest::lib::API::V1_0::types::configuration::ActiveSlotUpdateNotice& notice) {
        const char* GREEN = "\033[32m";
        const char* RESET = "\033[0m";
        std::cout << GREEN << "[Active Slot Update]" << RESET << " Active slot is now: " << notice.slot_id << "\n";
    };

    auto config_cb =
        [](const everest::lib::API::V1_0::types::configuration::ConfigurationParameterUpdateNotice& notice) {
            const char* YELLOW = "\033[33m";
            const char* RESET = "\033[0m";
            for (const auto& record : notice.update_results) {
                std::cout << YELLOW << "[Parameter Update]" << RESET << record.update.cfg_param_id.module_id << "|"
                          << record.update.cfg_param_id.implementation_id.value_or("\b") << "|"
                          << record.update.cfg_param_id.parameter_name << " -> " << record.update.value << "\n";
            }
        };

    m_client->subscribe_to_updates(suppress_parameter_updates, active_cb, config_cb);

    // Keep thread alive
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

} // namespace everest::config_cli
