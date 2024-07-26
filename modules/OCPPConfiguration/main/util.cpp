// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "util.hpp"
#include <fstream>
#include <sstream>

namespace module {

ryml::Tree Util::load_yaml_file(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + file_path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    ryml::Tree tree;
    //    ryml::parse_in_place(ryml::to_substr(buffer.str()), &tree);
    return tree;
}

void Util::save_yaml_file(const std::string& file_path, ryml::Tree& tree) {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + file_path);
    }
    file << tree;
}

} // namespace module