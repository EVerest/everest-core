// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "everest_api_types/isolation_monitor/API.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/isolation_monitor.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::isolation_monitor {

using IsolationMeasurement_Internal = ::types::isolation_monitor::IsolationMeasurement;
using IsolationMeasurement_External = IsolationMeasurement;

IsolationMeasurement_Internal to_internal_api(IsolationMeasurement_External const& val);
IsolationMeasurement_External to_external_api(IsolationMeasurement_Internal const& val);

} // namespace everest::lib::API::V1_0::types::isolation_monitor
