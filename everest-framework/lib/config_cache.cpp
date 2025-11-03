// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <utils/config_cache.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN
void adl_serializer<Everest::ConfigCache>::to_json(nlohmann::json& j, const Everest::ConfigCache& c) {
    j = {{"provides_impl", c.provides_impl}, {"cmds", c.cmds}};
}

void adl_serializer<Everest::ConfigCache>::from_json(const nlohmann::json& j, Everest::ConfigCache& c) {
    c.provides_impl = j.at("provides_impl");
    c.cmds = j.at("cmds");
}
NLOHMANN_JSON_NAMESPACE_END
