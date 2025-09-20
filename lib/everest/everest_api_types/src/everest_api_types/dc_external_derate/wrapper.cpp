// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "dc_external_derate/wrapper.hpp"

namespace everest::lib::API::V1_0::types::dc_external_derate {

ExternalDerating_Internal to_internal_api(ExternalDerating_External const& val) {
    ExternalDerating_Internal result;
    result.max_export_current_A = val.max_export_current_A;
    result.max_import_current_A = val.max_import_current_A;
    result.max_export_power_W = val.max_export_power_W;
    result.max_import_power_W = val.max_import_power_W;
    return result;
}

ExternalDerating_External to_external_api(ExternalDerating_Internal const& val) {
    ExternalDerating_External result;
    result.max_export_current_A = val.max_export_current_A;
    result.max_import_current_A = val.max_import_current_A;
    result.max_export_power_W = val.max_export_power_W;
    result.max_import_power_W = val.max_import_power_W;
    return result;
}

} // namespace everest::lib::API::V1_0::types::dc_external_derate
