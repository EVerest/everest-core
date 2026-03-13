// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/charger_information/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/charger_information.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::charger_information {

using ChargerInformation_Internal = ::types::charger_information::ChargerInformation;
using ChargerInformation_External = ChargerInformation;

ChargerInformation_Internal to_internal_api(ChargerInformation_External const& val);
ChargerInformation_External to_external_api(ChargerInformation_Internal const& val);

} // namespace everest::lib::API::V1_0::types::charger_information
