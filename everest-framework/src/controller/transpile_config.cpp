// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#include <nlohmann/json.hpp>

#include <ryml.hpp>
#include <ryml_std.hpp>

#include "transpile_config.hpp"

namespace {
// NOLINTNEXTLINE(misc-no-recursion)
void clear_quote_flags(ryml::NodeRef& root) {
    if (root.has_key()) {
        // Remove quotes from key
        if (root.tree()->type_has_any(root.id(), ryml::KEYQUO)) {
            root.tree()->_rem_flags(root.id(), ryml::KEYQUO);
        }
        if (root.key().find('-') != ryml::csubstr::npos) {
            root.tree()->_add_flags(root.id(), ryml::KEY_SQUO);
        }
    }
    if (root.has_val()) {
        if (root.val().has_str() && root.val() != "") {
            root.tree()->_rem_flags(root.id(), ryml::VALQUO);
        }
        if (root.val().empty()) {
            root.tree()->_add_flags(root.id(), ryml::VAL_SQUO);
        }
    }
    root.tree()->_rem_flags(root.id(), ryml::NodeType_e::FLOW_SL);

    for (auto child : root.children()) {
        clear_quote_flags(child);
    }
}
} // namespace

c4::yml::Tree transpile_config(const nlohmann::json& config_json) {
    const auto json_serialized = config_json.dump();
    auto ryml_deserialized = ryml::parse_json_in_arena(ryml::to_csubstr(json_serialized));
    auto root = ryml_deserialized.rootref();
    clear_quote_flags(root);
    return ryml_deserialized;
}
