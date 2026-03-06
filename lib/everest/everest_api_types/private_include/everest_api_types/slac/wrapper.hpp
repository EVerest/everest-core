// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest_api_types/slac/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/slac.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::slac {

using SlacState_Internal = ::types::slac::State;
using SlacState_External = State;

SlacState_Internal to_internal_api(SlacState_External const& val);
SlacState_External to_external_api(SlacState_Internal const& val);

} // namespace everest::lib::API::V1_0::types::slac