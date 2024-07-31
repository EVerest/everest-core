// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "util.hpp"
#include "everest/logging.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace module {

namespace Util {

ryml::Tree load_yaml_file(const std::string& file_path_string) {
    auto file_path =
        std::filesystem::path{"modules"} / "OCPPConfiguration" / file_path_string; // TODO: this is very hacky
    auto file = std::ifstream{file_path};
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + file_path.string());
    }
    auto buffer = std::stringstream{};
    buffer << file.rdbuf();
    const auto data = buffer.str();
    return ryml::parse_in_arena(ryml::to_csubstr(data));
}

void save_yaml_file(const std::string& file_path, ryml::Tree& tree) {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + file_path);
    }
    file << tree;
}

} // namespace Util
} // namespace module