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
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_internal_api(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return to_internal_api(val); });
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_external_api(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToExternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToInternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_internal_api(val); });
}
} // namespace

NumberWithSource_Internal to_internal_api(NumberWithSource_External const& val) {
    NumberWithSource_Internal result;
    result.source = val.source;
    result.value = val.value;
    return result;
}

NumberWithSource_External to_external_api(NumberWithSource_Internal const& val) {
    NumberWithSource_External result;
    result.source = val.source;
    result.value = val.value;
    return result;
}

IntegerWithSource_Internal to_internal_api(IntegerWithSource_External const& val) {
    IntegerWithSource_Internal result;
    result.source = val.source;
    result.value = val.value;
    return result;
}

IntegerWithSource_External to_external_api(IntegerWithSource_Internal const& val) {
    IntegerWithSource_External result;
    result.source = val.source;
    result.value = val.value;
    return result;
}

FrequencyWattPoint_Internal to_internal_api(FrequencyWattPoint_External const& val) {
    FrequencyWattPoint_Internal result;
    result.frequency_Hz = val.frequency_Hz;
    result.total_power_W = val.total_power_W;
    return result;
}

FrequencyWattPoint_External to_external_api(FrequencyWattPoint_Internal const& val) {
    FrequencyWattPoint_External result;
    result.frequency_Hz = val.frequency_Hz;
    result.total_power_W = val.total_power_W;
    return result;
}

SetpointType_Internal to_internal_api(SetpointType_External const& val) {
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

SetpointType_External to_external_api(SetpointType_Internal const& val) {
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

PricePerkWh_Internal to_internal_api(PricePerkWh_External const& val) {
    PricePerkWh_Internal result;
    result.timestamp = val.timestamp;
    result.value = val.value;
    result.currency = val.currency;
    return result;
}

PricePerkWh_External to_external_api(PricePerkWh_Internal const& val) {
    PricePerkWh_External result;
    result.timestamp = val.timestamp;
    result.value = val.value;
    result.currency = val.currency;
    return result;
}

LimitsReq_Internal to_internal_api(LimitsReq_External const& val) {
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

LimitsReq_External to_external_api(LimitsReq_Internal const& val) {
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

LimitsRes_Internal to_internal_api(LimitsRes_External const& val) {
    LimitsRes_Internal result;
    result.total_power_W = optToInternal(val.total_power_W);
    result.ac_max_current_A = optToInternal(val.ac_max_current_A);
    result.ac_max_phase_count = optToInternal(val.ac_max_phase_count);
    return result;
}

LimitsRes_External to_external_api(LimitsRes_Internal const& val) {
    LimitsRes_External result;
    result.total_power_W = optToExternal(val.total_power_W);
    result.ac_max_current_A = optToExternal(val.ac_max_current_A);
    result.ac_max_phase_count = optToExternal(val.ac_max_phase_count);
    return result;
}

ScheduleReqEntry_Internal to_internal_api(ScheduleReqEntry_External const& val) {
    ScheduleReqEntry_Internal result;
    result.timestamp = val.timestamp;
    result.limits_to_root = to_internal_api(val.limits_to_root);
    result.limits_to_leaves = to_internal_api(val.limits_to_leaves);
    result.conversion_efficiency = val.conversion_efficiency;
    result.price_per_kwh = optToInternal(val.price_per_kwh);
    return result;
}

ScheduleReqEntry_External to_external_api(ScheduleReqEntry_Internal const& val) {
    ScheduleReqEntry_External result;
    result.timestamp = val.timestamp;
    result.limits_to_root = to_external_api(val.limits_to_root);
    result.limits_to_leaves = to_external_api(val.limits_to_leaves);
    result.conversion_efficiency = val.conversion_efficiency;
    result.price_per_kwh = optToExternal(val.price_per_kwh);
    return result;
}

ScheduleResEntry_Internal to_internal_api(ScheduleResEntry_External const& val) {
    ScheduleResEntry_Internal result;
    result.timestamp = val.timestamp;
    result.limits_to_root = to_internal_api(val.limits_to_root);
    result.price_per_kwh = optToInternal(val.price_per_kwh);
    return result;
}

ScheduleResEntry_External to_external_api(ScheduleResEntry_Internal const& val) {
    ScheduleResEntry_External result;
    result.timestamp = val.timestamp;
    result.limits_to_root = to_external_api(val.limits_to_root);
    result.price_per_kwh = optToExternal(val.price_per_kwh);
    return result;
}

ScheduleSetpointEntry_Internal to_internal_api(ScheduleSetpointEntry_External const& val) {
    ScheduleSetpointEntry_Internal result;
    result.setpoint = optToInternal(val.setpoint);
    result.timestamp = val.timestamp;
    return result;
}

ScheduleSetpointEntry_External to_external_api(ScheduleSetpointEntry_Internal const& val) {
    ScheduleSetpointEntry_External result;
    result.setpoint = optToExternal(val.setpoint);
    result.timestamp = val.timestamp;
    return result;
}

ExternalLimits_Internal to_internal_api(ExternalLimits_External const& val) {
    ExternalLimits_Internal result;
    result.schedule_import = vecToInternal(val.schedule_import);
    result.schedule_export = vecToInternal(val.schedule_export);
    result.schedule_setpoints = vecToInternal(val.schedule_setpoints);
    return result;
}

ExternalLimits_External to_external_api(ExternalLimits_Internal const& val) {
    ExternalLimits_External result;
    result.schedule_import = vecToExternal(val.schedule_import);
    result.schedule_export = vecToExternal(val.schedule_export);
    return result;
}

EnforcedLimits_Internal to_internal_api(EnforcedLimits_External const& val) {
    EnforcedLimits_Internal result;
    result.uuid = val.uuid;
    result.valid_for = val.valid_for;
    result.limits_root_side = to_internal_api(val.limits_root_side);
    result.schedule = vecToInternal(val.schedule);
    return result;
}

EnforcedLimits_External to_external_api(EnforcedLimits_Internal const& val) {
    EnforcedLimits_External result;
    result.uuid = val.uuid;
    result.valid_for = val.valid_for;
    result.limits_root_side = to_external_api(val.limits_root_side);
    result.schedule = vecToExternal(val.schedule);
    return result;
}

} // namespace everest::lib::API::V1_0::types::energy
