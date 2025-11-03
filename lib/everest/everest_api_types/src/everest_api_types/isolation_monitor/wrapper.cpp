// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "isolation_monitor/wrapper.hpp"
#include "isolation_monitor/API.hpp"

namespace everest::lib::API::V1_0::types::isolation_monitor {

IsolationMeasurement_Internal to_internal_api(IsolationMeasurement_External const& val) {
    IsolationMeasurement_Internal result;
    result.resistance_F_Ohm = val.resistance_F_Ohm;
    result.voltage_V = val.voltage_V;
    return result;
}
IsolationMeasurement_External to_external_api(IsolationMeasurement_Internal const& val) {
    IsolationMeasurement_External result;
    result.resistance_F_Ohm = val.resistance_F_Ohm;
    result.voltage_V = val.voltage_V;
    return result;
}

} // namespace everest::lib::API::V1_0::types::isolation_monitor
