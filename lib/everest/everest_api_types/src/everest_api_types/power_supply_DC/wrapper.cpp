// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "power_supply_DC/wrapper.hpp"
#include "power_supply_DC/API.hpp"
#include <stdexcept>

namespace everest::lib::API::V1_0::types::power_supply_DC {

Capabilities_External to_external_api(Capabilities_Internal const& val) {
    Capabilities_External r;
    r.bidirectional = val.bidirectional;
    r.current_regulation_tolerance_A = val.current_regulation_tolerance_A;
    r.peak_current_ripple_A = val.peak_current_ripple_A;
    r.max_export_voltage_V = val.max_export_voltage_V;
    r.min_export_voltage_V = val.min_export_voltage_V;
    r.max_export_current_A = val.max_export_current_A;
    r.min_export_current_A = val.min_export_current_A;
    r.max_export_power_W = val.max_export_power_W;

    r.max_import_voltage_V = val.max_import_voltage_V;
    r.min_import_voltage_V = val.min_import_voltage_V;
    r.max_import_current_A = val.max_import_current_A;
    r.min_import_current_A = val.min_import_current_A;
    r.max_import_power_W = val.max_import_power_W;
    r.conversion_efficiency_import = val.conversion_efficiency_import;
    r.conversion_efficiency_export = val.conversion_efficiency_export;

    return r;
}

Capabilities_Internal to_internal_api(Capabilities_External const& val) {
    auto internal = Capabilities_Internal();
    internal.bidirectional = val.bidirectional;
    internal.current_regulation_tolerance_A = val.current_regulation_tolerance_A;
    internal.peak_current_ripple_A = val.peak_current_ripple_A;
    internal.max_export_voltage_V = val.max_export_voltage_V;
    internal.min_export_voltage_V = val.min_export_voltage_V;
    internal.max_export_current_A = val.max_export_current_A;
    internal.min_export_current_A = val.min_export_current_A;
    internal.max_export_power_W = val.max_export_power_W;

    internal.max_import_voltage_V = val.max_import_voltage_V;
    internal.min_import_voltage_V = val.min_import_voltage_V;
    internal.max_import_current_A = val.max_import_current_A;
    internal.min_import_current_A = val.min_import_current_A;
    internal.max_import_power_W = val.max_import_power_W;
    internal.conversion_efficiency_import = val.conversion_efficiency_import;
    internal.conversion_efficiency_export = val.conversion_efficiency_export;
    return internal;
}

Mode_External to_external_api(Mode_Internal mode_internal) {
    switch (mode_internal) {
    case Mode_Internal::Off:
        return Mode::Off;
    case Mode_Internal::Export:
        return Mode::Export;
    case Mode_Internal::Import:
        return Mode::Import;
    case Mode_Internal::Fault:
        return Mode::Fault;
    }

    throw std::out_of_range("No know conversion between internal and external mode API");
}

Mode_Internal to_internal_api(Mode_External mode_external) {
    switch (mode_external) {
    case Mode::Off:
        return Mode_Internal::Off;
    case Mode::Export:
        return Mode_Internal::Export;
    case Mode::Import:
        return Mode_Internal::Import;
    case Mode::Fault:
        return Mode_Internal::Fault;
    }

    throw std::out_of_range("No know conversion between internal and external mode API");
}

ChargingPhase_External to_external_api(ChargingPhase_Internal val) {
    switch (val) {
    case ChargingPhase_Internal::Other:
        return ChargingPhase_External::Other;
    case ChargingPhase_Internal::CableCheck:
        return ChargingPhase_External::CableCheck;
    case ChargingPhase_Internal::PreCharge:
        return ChargingPhase_External::PreCharge;
    case ChargingPhase_Internal::Charging:
        return ChargingPhase_External::Charging;
    }

    throw std::out_of_range("No know conversion from internal to external ChargingPhase API");
}

ChargingPhase_Internal to_internal_api(ChargingPhase_External val) {
    switch (val) {
    case ChargingPhase_External::Other:
        return ChargingPhase_Internal::Other;
    case ChargingPhase_External::CableCheck:
        return ChargingPhase_Internal::CableCheck;
    case ChargingPhase_External::PreCharge:
        return ChargingPhase_Internal::PreCharge;
    case ChargingPhase_External::Charging:
        return ChargingPhase_Internal::Charging;
    }

    throw std::out_of_range("No know conversion from external and internal ChargingPhase API");
}

VoltageCurrent_External to_external_api(VoltageCurrent_Internal const& internal) {
    VoltageCurrent_External result;
    result.voltage_V = internal.voltage_V;
    result.current_A = internal.current_A;
    return result;
}

VoltageCurrent_Internal to_internal_api(VoltageCurrent_External const& external) {
    VoltageCurrent_Internal result;
    result.current_A = external.current_A;
    result.voltage_V = external.voltage_V;
    return result;
}

} // namespace everest::lib::API::V1_0::types::power_supply_DC
