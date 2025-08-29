// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest_api_types/uk_random_delay/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/uk_random_delay.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::uk_random_delay {

using CountDown_Internal = ::types::uk_random_delay::CountDown;
using CountDown_External = CountDown;

CountDown_Internal to_internal_api(CountDown_External const& val);
CountDown_External to_external_api(CountDown_Internal const& val);

} // namespace everest::lib::API::V1_0::types::uk_random_delay
