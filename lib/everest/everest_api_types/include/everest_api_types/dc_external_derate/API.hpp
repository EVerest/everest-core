// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once
#include <optional>

namespace everest::lib::API::V1_0::types::dc_external_derate {

struct ExternalDerating {
    std::optional<float> max_export_current_A;
    std::optional<float> max_import_current_A;
    std::optional<float> max_export_power_W;
    std::optional<float> max_import_power_W;
};

} // namespace everest::lib::API::V1_0::types::dc_external_derate
