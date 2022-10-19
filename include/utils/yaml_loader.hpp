// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_YAML_LOADER_HPP
#define UTILS_YAML_LOADER_HPP

#include <boost/filesystem.hpp>
#include <nlohmann/json.hpp>

namespace Everest {

nlohmann::ordered_json load_yaml(const boost::filesystem::path& path);

}

#endif // UTILS_YAML_LOADER_HPP
