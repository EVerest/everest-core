// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest_api_types/ev_board_support/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/board_support_common.hpp"
#include "generated/types/ev_board_support.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::ev_board_support {

using EvCpState_Internal = ::types::ev_board_support::EvCpState;
using EvCpState_External = EvCpState;

EvCpState_Internal to_internal_api(EvCpState_External const& val);
EvCpState_External to_external_api(EvCpState_Internal const& val);

using BspMeasurement_Internal = ::types::board_support_common::BspMeasurement;
using BspMeasurement_External = BspMeasurement;

BspMeasurement_Internal to_internal_api(BspMeasurement_External const& val);
BspMeasurement_External to_external_api(BspMeasurement_Internal const& val);

} // namespace everest::lib::API::V1_0::types::ev_board_support
