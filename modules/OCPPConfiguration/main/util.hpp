// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <c4/yml/std/map.hpp>
#include <ryml.hpp>

namespace module {
class Util {
public:
    static ryml::Tree load_yaml_file(const std::string& file_path);
    static void save_yaml_file(const std::string& file_path, ryml::Tree& tree);
};

} // namespace module