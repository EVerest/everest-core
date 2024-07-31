// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "mapping_reader.hpp"
#include <c4/yml/std/map.hpp>
#include <ryml.hpp>
#include <string>
#include <unordered_map>

namespace module {
class ConfigWriter {
public:
    ConfigWriter(const std::string& user_config_file_in);

    void write_to_config(const EverestModuleMapping& module_mapping, const std::string& config_value);
    void save_config();

private:
    ryml::Tree load_existing_user_config(const std::string& user_config_file);

    ryml::Tree config_tree;
    const std::string user_config_file;
};

} // namespace module
