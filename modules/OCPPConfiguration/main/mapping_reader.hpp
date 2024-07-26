// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <c4/yml/std/map.hpp>
#include <ryml.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace module {
class MappingReader {
public:
    using EverestConfigPair = std::tuple<std::string, std::string>;
    using ConfigMap = std::unordered_map<std::string, EverestConfigPair>;

    static ConfigMap readMapping(const std::string& file_path);

private:
    static EverestConfigPair parseMappingNode(const ryml::NodeRef& node);
};
} // namespace module