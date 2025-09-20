// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "dc_external_derate/json_codec.hpp"
#include "dc_external_derate/API.hpp"
#include "dc_external_derate/codec.hpp"

#include "nlohmann/json.hpp"

namespace everest::lib::API::V1_0::types::dc_external_derate {

void to_json(json& j, ExternalDerating const& k) noexcept {
    j = json({});
    if (k.max_export_current_A) {
        j["max_export_current_A"] = k.max_export_current_A.value();
    }
    if (k.max_import_current_A) {
        j["max_import_current_A"] = k.max_import_current_A.value();
    }
    if (k.max_export_power_W) {
        j["max_export_power_W"] = k.max_export_power_W.value();
    }
    if (k.max_import_power_W) {
        j["max_import_power_W"] = k.max_import_power_W.value();
    }
}

void from_json(const json& j, ExternalDerating& k) {
    if (j.contains("max_export_current_A")) {
        k.max_export_current_A.emplace(j.at("max_export_current_A"));
    }
    if (j.contains("max_import_current_A")) {
        k.max_import_current_A.emplace(j.at("max_import_current_A"));
    }
    if (j.contains("max_export_power_W")) {
        k.max_export_power_W.emplace(j.at("max_export_power_W"));
    }
    if (j.contains("max_import_power_W")) {
        k.max_import_power_W.emplace(j.at("max_import_power_W"));
    }
}

} // namespace everest::lib::API::V1_0::types::dc_external_derate
