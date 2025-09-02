// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "energy/wrapper.hpp"
#include "energy/API.hpp"
#include <vector>

namespace everest::lib::API::V1_0::types::energy {

namespace {
template <class SrcT, class ConvT>
auto srcToTarOpt(std::optional<SrcT> const& src, ConvT const& converter)
    -> std::optional<decltype(converter(src.value()))> {
    if (src) {
        return std::make_optional(converter(src.value()));
    }
    return std::nullopt;
}

template <class SrcT, class ConvT> auto srcToTarVec(std::vector<SrcT> const& src, ConvT const& converter) {
    using TarT = decltype(converter(src[0]));
    std::vector<TarT> result;
    for (SrcT const& elem : src) {
        result.push_back(converter(elem));
    }
    return result;
}

template <class SrcT>
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(toInternalApi(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return toInternalApi(val); });
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(toExternalApi(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return toExternalApi(val); });
}

template <class SrcT> auto vecToExternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return toExternalApi(val); });
}

template <class SrcT> auto vecToInternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return toInternalApi(val); });
}
} // namespace

NumberWithSource_Internal toInternalApi(NumberWithSource_External const& val) {
    NumberWithSource_Internal result;
    result.source = val.source;
    result.value = val.value;
    return result;
}

NumberWithSource_External toExternalApi(NumberWithSource_Internal const& val) {
    NumberWithSource_External result;
    result.source = val.source;
    result.value = val.value;
    return result;
}

IntegerWithSource_Internal toInternalApi(IntegerWithSource_External const& val) {
    IntegerWithSource_Internal result;
    result.source = val.source;
    result.value = val.value;
    return result;
}

IntegerWithSource_External toExternalApi(IntegerWithSource_Internal const& val) {
    IntegerWithSource_External result;
    result.source = val.source;
    result.value = val.value;
    return result;
}

FrequencyWattPoint_Internal toInternalApi(FrequencyWattPoint_External const& val) {
    FrequencyWattPoint_Internal result;
    result.frequency_Hz = val.frequency_Hz;
    result.total_power_W = val.total_power_W;
    return result;
}

FrequencyWattPoint_External toExternalApi(FrequencyWattPoint_Internal const& val) {
    FrequencyWattPoint_External result;
    result.frequency_Hz = val.frequency_Hz;
    result.total_power_W = val.total_power_W;
    return result;
}

SetpointType_Internal toInternalApi(SetpointType_External const& val) {
    SetpointType_Internal result;
    result.priority = val.priority;
    result.source = val.source;
    result.ac_current_A = val.ac_current_A;
    result.total_power_W = val.total_power_W;
    if (val.frequency_table) {
        result.frequency_table = vecToInternal(val.frequency_table.value());
    }
    return result;
}

SetpointType_External toExternalApi(SetpointType_Internal const& val) {
    SetpointType_External result;
    result.priority = val.priority;
    result.source = val.source;
    result.ac_current_A = val.ac_current_A;
    result.total_power_W = val.total_power_W;
    if (val.frequency_table) {
        result.frequency_table = vecToExternal(val.frequency_table.value());
    }
    return result;
}

PricePerkWh_Internal toInternalApi(PricePerkWh_External const& val) {
    PricePerkWh_Internal result;
    result.timestamp = val.timestamp;
    result.value = val.value;
    result.currency = val.currency;
    return result;
}

PricePerkWh_External toExternalApi(PricePerkWh_Internal const& val) {
    PricePerkWh_External result;
    result.timestamp = val.timestamp;
    result.value = val.value;
    result.currency = val.currency;
    return result;
}

LimitsReq_Internal toInternalApi(LimitsReq_External const& val) {
    LimitsReq_Internal result;
    result.total_power_W = optToInternal(val.total_power_W);
    result.ac_max_current_A = optToInternal(val.ac_max_current_A);
    result.ac_min_current_A = optToInternal(val.ac_min_current_A);
    result.ac_max_phase_count = optToInternal(val.ac_max_phase_count);
    result.ac_min_phase_count = optToInternal(val.ac_min_phase_count);
    result.ac_supports_changing_phases_during_charging = val.ac_supports_changing_phases_during_charging;
    result.ac_number_of_active_phases = val.ac_number_of_active_phases;
    return result;
}

LimitsReq_External toExternalApi(LimitsReq_Internal const& val) {
    LimitsReq_External result;
    result.total_power_W = optToExternal(val.total_power_W);
    result.ac_max_current_A = optToExternal(val.ac_max_current_A);
    result.ac_min_current_A = optToExternal(val.ac_min_current_A);
    result.ac_max_phase_count = optToExternal(val.ac_max_phase_count);
    result.ac_min_phase_count = optToExternal(val.ac_min_phase_count);
    result.ac_supports_changing_phases_during_charging = val.ac_supports_changing_phases_during_charging;
    result.ac_number_of_active_phases = val.ac_number_of_active_phases;
    return result;
}

LimitsRes_Internal toInternalApi(LimitsRes_External const& val) {
    LimitsRes_Internal result;
    result.total_power_W = optToInternal(val.total_power_W);
    result.ac_max_current_A = optToInternal(val.ac_max_current_A);
    result.ac_max_phase_count = optToInternal(val.ac_max_phase_count);
    return result;
}

LimitsRes_External toExternalApi(LimitsRes_Internal const& val) {
    LimitsRes_External result;
    result.total_power_W = optToExternal(val.total_power_W);
    result.ac_max_current_A = optToExternal(val.ac_max_current_A);
    result.ac_max_phase_count = optToExternal(val.ac_max_phase_count);
    return result;
}

ScheduleReqEntry_Internal toInternalApi(ScheduleReqEntry_External const& val) {
    ScheduleReqEntry_Internal result;
    result.timestamp = val.timestamp;
    result.limits_to_root = toInternalApi(val.limits_to_root);
    result.limits_to_leaves = toInternalApi(val.limits_to_leaves);
    result.conversion_efficiency = val.conversion_efficiency;
    result.price_per_kwh = optToInternal(val.price_per_kwh);
    return result;
}

ScheduleReqEntry_External toExternalApi(ScheduleReqEntry_Internal const& val) {
    ScheduleReqEntry_External result;
    result.timestamp = val.timestamp;
    result.limits_to_root = toExternalApi(val.limits_to_root);
    result.limits_to_leaves = toExternalApi(val.limits_to_leaves);
    result.conversion_efficiency = val.conversion_efficiency;
    result.price_per_kwh = optToExternal(val.price_per_kwh);
    return result;
}

ScheduleResEntry_Internal toInternalApi(ScheduleResEntry_External const& val) {
    ScheduleResEntry_Internal result;
    result.timestamp = val.timestamp;
    result.limits_to_root = toInternalApi(val.limits_to_root);
    result.price_per_kwh = optToInternal(val.price_per_kwh);
    return result;
}

ScheduleResEntry_External toExternalApi(ScheduleResEntry_Internal const& val) {
    ScheduleResEntry_External result;
    result.timestamp = val.timestamp;
    result.limits_to_root = toExternalApi(val.limits_to_root);
    result.price_per_kwh = optToExternal(val.price_per_kwh);
    return result;
}

ScheduleSetpointEntry_Internal toInternalApi(ScheduleSetpointEntry_External const& val) {
    ScheduleSetpointEntry_Internal result;
    result.setpoint = optToInternal(val.setpoint);
    result.timestamp = val.timestamp;
    return result;
}

ScheduleSetpointEntry_External toExternalApi(ScheduleSetpointEntry_Internal const& val) {
    ScheduleSetpointEntry_External result;
    result.setpoint = optToExternal(val.setpoint);
    result.timestamp = val.timestamp;
    return result;
}

ExternalLimits_Internal toInternalApi(ExternalLimits_External const& val) {
    ExternalLimits_Internal result;
    result.schedule_import = vecToInternal(val.schedule_import);
    result.schedule_export = vecToInternal(val.schedule_export);
    result.schedule_setpoints = vecToInternal(val.schedule_setpoints);
    return result;
}

ExternalLimits_External toExternalApi(ExternalLimits_Internal const& val) {
    ExternalLimits_External result;
    result.schedule_import = vecToExternal(val.schedule_import);
    result.schedule_export = vecToExternal(val.schedule_export);
    return result;
}

EnforcedLimits_Internal toInternalApi(EnforcedLimits_External const& val) {
    EnforcedLimits_Internal result;
    result.uuid = val.uuid;
    result.valid_for = val.valid_for;
    result.limits_root_side = toInternalApi(val.limits_root_side);
    result.schedule = vecToInternal(val.schedule);
    return result;
}

EnforcedLimits_External toExternalApi(EnforcedLimits_Internal const& val) {
    EnforcedLimits_External result;
    result.uuid = val.uuid;
    result.valid_for = val.valid_for;
    result.limits_root_side = toExternalApi(val.limits_root_side);
    result.schedule = vecToExternal(val.schedule);
    return result;
}

} // namespace everest::lib::API::V1_0::types::energy
