// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_CONFIG_CACHE_HPP
#define UTILS_CONFIG_CACHE_HPP

#include <set>
#include <string>
#include <unordered_map>

#include <utils/types.hpp>

namespace Everest {
using json = nlohmann::json;

struct ConfigCache {
    std::set<std::string> provides_impl;
    std::unordered_map<std::string, json> cmds;
};

} // namespace Everest

#endif // UTILS_CONFIG_CACHE_HPP
