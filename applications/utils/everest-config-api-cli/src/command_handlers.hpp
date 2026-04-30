#pragma once

#include "i_config_service_client.hpp"
#include "i_yaml_provider.hpp"
#include <memory>
#include <string>

namespace everest::config_cli {

class CommandHandlers {
public:
    CommandHandlers(std::shared_ptr<IConfigServiceClient> client, std::shared_ptr<IYamlProvider> yaml_provider);

    void list_slots();
    void show_slot_metadata(int slot_id);
    void active_slot();
    void mark_active_slot(int slot_id);
    void delete_slot(int slot_id);
    void duplicate_slot(int slot_id, const std::string& description);
    void load_yaml(const std::string& filename, const std::string& description, std::optional<int> slot_id);
    void get_configuration(int slot_id);
    void set_config_parameter(int slot_id, const std::string& filename);
    void monitor(bool suppress_parameter_updates);

private:
    std::shared_ptr<IConfigServiceClient> m_client;
    std::shared_ptr<IYamlProvider> m_yaml_provider;
};

} // namespace everest::config_cli
