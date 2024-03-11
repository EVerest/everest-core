// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#include <nlohmann/json.hpp>

#include <ryml.hpp>
#include <ryml_std.hpp>

#include "transpile_config.hpp"

static void clear_quote_flags(ryml::NodeRef& root) {
    if (root.has_key()) {
        // Remove quotes from key
        root.tree()->_rem_flags(root.id(), ryml::KEYQUO);
    }
    if (root.has_val()) {
        if (root.val().has_str() && root.val() != "") {
            root.tree()->_rem_flags(root.id(), ryml::VALQUO);
        }
    }

    for (auto child : root.children()) {
        clear_quote_flags(child);
    }
}

c4::yml::Tree transpile_config(nlohmann::json config_json) {
    const auto json_serialized = config_json.dump();
    auto ryml_deserialized = ryml::parse_in_arena(ryml::to_csubstr(json_serialized));
    auto root = ryml_deserialized.rootref();
    clear_quote_flags(root);
    return ryml_deserialized;
}
