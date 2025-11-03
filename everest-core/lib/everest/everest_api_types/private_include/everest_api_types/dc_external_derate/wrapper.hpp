// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/dc_external_derate/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/dc_external_derate.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::dc_external_derate {

using ExternalDerating_Internal = ::types::dc_external_derate::ExternalDerating;
using ExternalDerating_External = ExternalDerating;

ExternalDerating_Internal to_internal_api(ExternalDerating_External const& val);
ExternalDerating_External to_external_api(ExternalDerating_Internal const& val);

} // namespace everest::lib::API::V1_0::types::dc_external_derate
