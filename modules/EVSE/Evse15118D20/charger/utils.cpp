// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "utils.hpp"

#include <iso15118/d20/limits.hpp>
#include <iso15118/message/common_types.hpp>
#include <iso15118/message/dc_charge_parameter_discovery.hpp>
#include <iso15118/message/schedule_exchange.hpp>
#include <iso15118/message/service_detail.hpp>

namespace module::charger {

namespace dt = iso15118::message_20::datatypes;

namespace {
dt::Parameter convert_parameter(const types::iso15118_vas::Parameter& parameter) {
    dt::Parameter out;
    out.name = parameter.name;

    if (parameter.value.bool_value.has_value()) {
        out.value = parameter.value.bool_value.value();
    } else if (parameter.value.int_value.has_value()) {
        out.value = static_cast<int32_t>(parameter.value.int_value.value());
    } else if (parameter.value.short_value.has_value()) {
        out.value = static_cast<int16_t>(parameter.value.short_value.value());
    } else if (parameter.value.byte_value.has_value()) {
        out.value = static_cast<int8_t>(parameter.value.byte_value.value());
    } else if (parameter.value.rational_number.has_value()) {
        out.value = dt::from_float(parameter.value.rational_number.value());
    } else if (parameter.value.finite_string.has_value()) {
        out.value = parameter.value.finite_string.value();
    } else {
        throw std::invalid_argument("Invalid ParameterValue in convert_parameter: " + parameter.name +
                                    " has no value set");
    }

    return out;
}

dt::ParameterSet convert_parameter_set(const types::iso15118_vas::ParameterSet& parameter_set) {
    dt::ParameterSet out;
    out.id = parameter_set.set_id;

    out.parameter.reserve(parameter_set.parameters.size());
    for (const auto& parameter : parameter_set.parameters) {
        out.parameter.push_back(convert_parameter(parameter));
    }

    return out;
}
} // namespace

std::optional<float> convert_from_optional(const std::optional<dt::RationalNumber>& in) {
    return (in.has_value()) ? std::make_optional(dt::from_RationalNumber(*in)) : std::nullopt;
}

std::optional<dt::RationalNumber> convert_from_optional(const std::optional<float>& in) {
    return (in.has_value()) ? std::make_optional(dt::from_float(*in)) : std::nullopt;
}

std::optional<float> convert_from_optional(const std::optional<uint32_t>& in) {
    return (in.has_value()) ? std::make_optional(static_cast<float>(*in)) : std::nullopt;
}

types::iso15118::AppProtocol convert_app_protocol(const iso15118::message_20::SupportedAppProtocol& app_protocol) {
    types::iso15118::AppProtocol result;
    result.protocol_namespace = app_protocol.protocol_namespace;
    result.priority = app_protocol.priority;
    result.schema_id = app_protocol.schema_id;
    result.version_number_major = static_cast<int32_t>(app_protocol.version_number_major);
    result.version_number_minor = static_cast<int32_t>(app_protocol.version_number_minor);
    return result;
}

types::iso15118::EvInformation convert_ev_info(const iso15118::d20::EVInformation& ev_info) {
    types::iso15118::EvInformation result;
    result.evcc_id = ev_info.evcc_id;
    result.selected_protocol = convert_app_protocol(ev_info.selected_app_protocol);
    result.supported_protocols.Protocols.reserve(ev_info.ev_supported_app_protocols.size());
    for (const auto& supported_app : ev_info.ev_supported_app_protocols) {
        result.supported_protocols.Protocols.push_back(convert_app_protocol(supported_app));
    }
    result.tls_leaf_certificate = ev_info.ev_tls_leaf_cert;
    result.tls_sub_ca_1_certificate = ev_info.ev_tls_sub_ca_1_cert;
    result.tls_sub_ca_2_certificate = ev_info.ev_tls_sub_ca_2_cert;
    result.tls_root_certificate = ev_info.ev_tls_root_cert;
    return result;
}

types::iso15118::DcChargeDynamicModeValues convert_dynamic_values(const dt::Dynamic_DC_CLReqControlMode& in) {
    return {dt::from_RationalNumber(in.target_energy_request),
            dt::from_RationalNumber(in.max_energy_request),
            dt::from_RationalNumber(in.min_energy_request),
            dt::from_RationalNumber(in.max_charge_power),
            dt::from_RationalNumber(in.min_charge_power),
            dt::from_RationalNumber(in.max_charge_current),
            dt::from_RationalNumber(in.max_voltage),
            dt::from_RationalNumber(in.min_voltage),
            convert_from_optional(in.departure_time),
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt,
            std::nullopt};
}

types::iso15118::DcChargeDynamicModeValues convert_dynamic_values(const dt::BPT_Dynamic_DC_CLReqControlMode& in) {
    return {dt::from_RationalNumber(in.target_energy_request), dt::from_RationalNumber(in.max_energy_request),
            dt::from_RationalNumber(in.min_energy_request),    dt::from_RationalNumber(in.max_charge_power),
            dt::from_RationalNumber(in.min_charge_power),      dt::from_RationalNumber(in.max_charge_current),
            dt::from_RationalNumber(in.max_voltage),           dt::from_RationalNumber(in.min_voltage),
            convert_from_optional(in.departure_time),          dt::from_RationalNumber(in.max_discharge_power),
            dt::from_RationalNumber(in.min_discharge_power),   dt::from_RationalNumber(in.max_discharge_current),
            convert_from_optional(in.max_v2x_energy_request),  convert_from_optional(in.min_v2x_energy_request)};
}

template <>
void fill_v2x_charging_parameters(types::iso15118::V2XChargingParameters& out_params,
                                  const iso15118::d20::DcTransferLimits& evse_limits,
                                  const dt::DC_CPDReqEnergyTransferMode& ev_limits) {
    // As per the OCPP 2.1 spec (2.109) we should use the MIN/MAX function between EV and EVSE
    out_params.min_charge_power = std::max(dt::from_RationalNumber(evse_limits.charge_limits.power.min),
                                           dt::from_RationalNumber(ev_limits.min_charge_power));
    out_params.max_charge_power = std::min(dt::from_RationalNumber(evse_limits.charge_limits.power.max),
                                           dt::from_RationalNumber(ev_limits.max_charge_power));

    out_params.min_charge_current = std::max(dt::from_RationalNumber(evse_limits.charge_limits.current.min),
                                             dt::from_RationalNumber(ev_limits.min_charge_current));
    out_params.max_charge_current = std::min(dt::from_RationalNumber(evse_limits.charge_limits.current.max),
                                             dt::from_RationalNumber(ev_limits.max_charge_current));

    out_params.min_voltage =
        std::max(dt::from_RationalNumber(evse_limits.voltage.min), dt::from_RationalNumber(ev_limits.min_voltage));
    out_params.max_voltage =
        std::min(dt::from_RationalNumber(evse_limits.voltage.max), dt::from_RationalNumber(ev_limits.max_voltage));

    out_params.target_soc = ev_limits.target_soc;
}

template <>
void fill_v2x_charging_parameters(types::iso15118::V2XChargingParameters& out_params,
                                  const iso15118::d20::DcTransferLimits& evse_limits,
                                  const dt::BPT_DC_CPDReqEnergyTransferMode& ev_limits) {
    // Fill in the common data
    fill_v2x_charging_parameters<iso15118::d20::DcTransferLimits, dt::DC_CPDReqEnergyTransferMode>(
        out_params, evse_limits, static_cast<const dt::DC_CPDReqEnergyTransferMode&>(ev_limits));

    // Fill in the bidi data
    if (evse_limits.discharge_limits.has_value()) {
        const auto& evse_discharge_limits = evse_limits.discharge_limits.value();

        out_params.min_discharge_power = std::max(dt::from_RationalNumber(evse_discharge_limits.power.min),
                                                  dt::from_RationalNumber(ev_limits.min_discharge_power));
        out_params.max_discharge_power = std::min(dt::from_RationalNumber(evse_discharge_limits.power.max),
                                                  dt::from_RationalNumber(ev_limits.max_discharge_power));

        out_params.min_discharge_current = std::max(dt::from_RationalNumber(evse_discharge_limits.current.min),
                                                    dt::from_RationalNumber(ev_limits.min_discharge_current));
        out_params.max_discharge_current = std::min(dt::from_RationalNumber(evse_discharge_limits.current.max),
                                                    dt::from_RationalNumber(ev_limits.max_discharge_current));
    }
}

template <>
void fill_v2x_charging_parameters(types::iso15118::V2XChargingParameters& out_params,
                                  const iso15118::d20::AcTransferLimits& evse_limits,
                                  const dt::AC_CPDReqEnergyTransferMode& ev_limits) {
    // As per the OCPP 2.1 spec (2.109) we should use the MIN/MAX function between EV and EVSE
    out_params.min_charge_power = std::max(dt::from_RationalNumber(evse_limits.charge_power.min),
                                           dt::from_RationalNumber(ev_limits.min_charge_power));
    out_params.max_charge_power = std::min(dt::from_RationalNumber(evse_limits.charge_power.max),
                                           dt::from_RationalNumber(ev_limits.max_charge_power));

    if (evse_limits.charge_power_L2.has_value() and ev_limits.max_charge_power_L2.has_value() and
        ev_limits.min_charge_power_L2.has_value()) {
        out_params.min_charge_power_l2 = std::max(dt::from_RationalNumber(evse_limits.charge_power_L2.value().min),
                                                  dt::from_RationalNumber(ev_limits.min_charge_power_L2.value()));
        out_params.max_charge_power_l2 = std::min(dt::from_RationalNumber(evse_limits.charge_power_L2.value().max),
                                                  dt::from_RationalNumber(ev_limits.max_charge_power_L2.value()));
    }

    if (evse_limits.charge_power_L3.has_value() and ev_limits.max_charge_power_L3.has_value() and
        ev_limits.min_charge_power_L3.has_value()) {
        out_params.min_charge_power_l3 = std::max(dt::from_RationalNumber(evse_limits.charge_power_L3.value().min),
                                                  dt::from_RationalNumber(ev_limits.min_charge_power_L3.value()));
        out_params.max_charge_power_l3 = std::min(dt::from_RationalNumber(evse_limits.charge_power_L3.value().max),
                                                  dt::from_RationalNumber(ev_limits.max_charge_power_L3.value()));
    }
}

template <>
void fill_v2x_charging_parameters(types::iso15118::V2XChargingParameters& out_params,
                                  const iso15118::d20::AcTransferLimits& evse_limits,
                                  const dt::BPT_AC_CPDReqEnergyTransferMode& ev_limits) {
    // Fill in the common data
    fill_v2x_charging_parameters<iso15118::d20::AcTransferLimits, dt::AC_CPDReqEnergyTransferMode>(
        out_params, evse_limits, static_cast<const dt::AC_CPDReqEnergyTransferMode&>(ev_limits));

    if (evse_limits.discharge_power.has_value()) {
        const auto& evse_discharge_limits = evse_limits.discharge_power.value();

        out_params.min_discharge_power = std::max(dt::from_RationalNumber(evse_discharge_limits.min),
                                                  dt::from_RationalNumber(ev_limits.min_discharge_power));
        out_params.max_discharge_power = std::min(dt::from_RationalNumber(evse_discharge_limits.max),
                                                  dt::from_RationalNumber(ev_limits.max_discharge_power));
    }

    if (evse_limits.discharge_power_L2.has_value() && ev_limits.min_discharge_power_L2.has_value() &&
        ev_limits.max_discharge_power_L2.has_value()) {
        const auto& evse_discharge_limits = evse_limits.discharge_power_L2.value();

        out_params.min_discharge_power_l2 = std::max(dt::from_RationalNumber(evse_discharge_limits.min),
                                                     dt::from_RationalNumber(ev_limits.min_discharge_power_L2.value()));
        out_params.max_discharge_power_l2 = std::min(dt::from_RationalNumber(evse_discharge_limits.max),
                                                     dt::from_RationalNumber(ev_limits.max_discharge_power_L2.value()));
    }

    if (evse_limits.discharge_power_L3.has_value() && ev_limits.min_discharge_power_L3.has_value() &&
        ev_limits.max_discharge_power_L3.has_value()) {
        const auto& evse_discharge_limits = evse_limits.discharge_power_L3.value();

        out_params.min_discharge_power_l3 = std::max(dt::from_RationalNumber(evse_discharge_limits.min),
                                                     dt::from_RationalNumber(ev_limits.min_discharge_power_L3.value()));
        out_params.max_discharge_power_l3 = std::min(dt::from_RationalNumber(evse_discharge_limits.max),
                                                     dt::from_RationalNumber(ev_limits.max_discharge_power_L3.value()));
    }
}

template <>
void fill_v2x_charging_parameters(types::iso15118::V2XChargingParameters& out_params,
                                  const dt::Scheduled_SEReqControlMode& ev_control_mode) {
    out_params.ev_target_energy_request = convert_from_optional(ev_control_mode.target_energy);
    out_params.ev_min_energy_request = convert_from_optional(ev_control_mode.min_energy);
    out_params.ev_max_energy_request = convert_from_optional(ev_control_mode.max_energy);
}

template <>
void fill_v2x_charging_parameters(types::iso15118::V2XChargingParameters& out_params,
                                  const dt::Dynamic_SEReqControlMode& ev_control_mode) {
    out_params.ev_target_energy_request = dt::from_RationalNumber(ev_control_mode.target_energy);
    out_params.ev_min_energy_request = dt::from_RationalNumber(ev_control_mode.min_energy);
    out_params.ev_max_energy_request = dt::from_RationalNumber(ev_control_mode.max_energy);

    out_params.ev_min_v2xenergy_request = convert_from_optional(ev_control_mode.min_v2x_energy);
    out_params.ev_max_v2xenergy_request = convert_from_optional(ev_control_mode.max_v2x_energy);
}

std::vector<dt::ParameterSet>
convert_parameter_set_list(const std::vector<types::iso15118_vas::ParameterSet>& parameter_set_list) {
    std::vector<dt::ParameterSet> out;
    out.reserve(parameter_set_list.size());
    for (const auto& parameter_set : parameter_set_list) {
        out.push_back(convert_parameter_set(parameter_set));
    }
    return out;
}

types::iso15118::AcEvPowerLimits fill_ac_ev_power_limits(const dt::AC_CPDReqEnergyTransferMode& mode) {
    const auto max_charge_L1 = dt::from_RationalNumber(mode.max_charge_power);
    const auto max_charge_L2 = convert_from_optional(mode.max_charge_power_L2);
    const auto max_charge_L3 = convert_from_optional(mode.max_charge_power_L3);
    const auto max_charge_total = max_charge_L1 + max_charge_L2.value_or(0.0) + max_charge_L3.value_or(0.0);

    const auto min_charge_L1 = dt::from_RationalNumber(mode.min_charge_power);
    const auto min_charge_L2 = convert_from_optional(mode.min_charge_power_L2);
    const auto min_charge_L3 = convert_from_optional(mode.min_charge_power_L3);
    const auto min_charge_total = min_charge_L1 + min_charge_L2.value_or(0.0) + min_charge_L3.value_or(0.0);

    types::iso15118::AcEvPowerLimits ac_ev_power_limits;

    ac_ev_power_limits.max_charge_power = {max_charge_total, std::make_optional<float>(max_charge_L1), max_charge_L2,
                                           max_charge_L3};
    ac_ev_power_limits.min_charge_power = {min_charge_total, std::make_optional<float>(min_charge_L1), min_charge_L2,
                                           min_charge_L3};

    return ac_ev_power_limits;
}

types::iso15118::AcEvPowerLimits fill_ac_ev_power_limits(const dt::BPT_AC_CPDReqEnergyTransferMode& mode) {
    const auto max_charge_L1 = dt::from_RationalNumber(mode.max_charge_power);
    const auto max_charge_L2 = convert_from_optional(mode.max_charge_power_L2);
    const auto max_charge_L3 = convert_from_optional(mode.max_charge_power_L3);
    const auto max_charge_total = max_charge_L1 + max_charge_L2.value_or(0.0) + max_charge_L3.value_or(0.0);

    const auto min_charge_L1 = dt::from_RationalNumber(mode.min_charge_power);
    const auto min_charge_L2 = convert_from_optional(mode.min_charge_power_L2);
    const auto min_charge_L3 = convert_from_optional(mode.min_charge_power_L3);
    const auto min_charge_total = min_charge_L1 + min_charge_L2.value_or(0.0) + min_charge_L3.value_or(0.0);

    const auto max_discharge_L1 = dt::from_RationalNumber(mode.max_discharge_power);
    const auto max_discharge_L2 = convert_from_optional(mode.max_discharge_power_L2);
    const auto max_discharge_L3 = convert_from_optional(mode.max_discharge_power_L3);
    const auto max_discharge_total = max_discharge_L1 + max_discharge_L2.value_or(0.0) + max_discharge_L3.value_or(0.0);

    const auto min_discharge_L1 = dt::from_RationalNumber(mode.min_discharge_power);
    const auto min_discharge_L2 = convert_from_optional(mode.min_discharge_power_L2);
    const auto min_discharge_L3 = convert_from_optional(mode.min_discharge_power_L3);
    const auto min_discharge_total = min_discharge_L1 + min_discharge_L2.value_or(0.0) + min_discharge_L3.value_or(0.0);

    types::iso15118::AcEvPowerLimits ac_ev_power_limits;

    ac_ev_power_limits.max_charge_power = {max_charge_total, std::make_optional<float>(max_charge_L1), max_charge_L2,
                                           max_charge_L3};
    ac_ev_power_limits.min_charge_power = {min_charge_total, std::make_optional<float>(min_charge_L1), min_charge_L2,
                                           min_charge_L3};
    ac_ev_power_limits.max_discharge_power = {max_discharge_total, std::make_optional<float>(max_discharge_L1),
                                              max_discharge_L2, max_discharge_L3};
    ac_ev_power_limits.min_discharge_power = {min_discharge_total, std::make_optional<float>(min_discharge_L1),
                                              min_discharge_L2, min_discharge_L3};

    return ac_ev_power_limits;
}

types::iso15118::AcEvPowerLimits fill_ac_ev_power_limits(const dt::Dynamic_AC_CLReqControlMode& mode) {
    const auto max_charge_L1 = dt::from_RationalNumber(mode.max_charge_power);
    const auto max_charge_L2 = convert_from_optional(mode.max_charge_power_L2);
    const auto max_charge_L3 = convert_from_optional(mode.max_charge_power_L3);
    const auto max_charge_total = max_charge_L1 + max_charge_L2.value_or(0.0) + max_charge_L3.value_or(0.0);

    const auto min_charge_L1 = dt::from_RationalNumber(mode.min_charge_power);
    const auto min_charge_L2 = convert_from_optional(mode.min_charge_power_L2);
    const auto min_charge_L3 = convert_from_optional(mode.min_charge_power_L3);
    const auto min_charge_total = min_charge_L1 + min_charge_L2.value_or(0.0) + min_charge_L3.value_or(0.0);

    types::iso15118::AcEvPowerLimits ac_ev_power_limits;

    ac_ev_power_limits.max_charge_power = {max_charge_total, std::make_optional<float>(max_charge_L1), max_charge_L2,
                                           max_charge_L3};
    ac_ev_power_limits.min_charge_power = {min_charge_total, std::make_optional<float>(min_charge_L1), min_charge_L2,
                                           min_charge_L3};

    return ac_ev_power_limits;
}

types::iso15118::AcEvPowerLimits fill_ac_ev_power_limits(const dt::BPT_Dynamic_AC_CLReqControlMode& mode) {
    const auto max_charge_L1 = dt::from_RationalNumber(mode.max_charge_power);
    const auto max_charge_L2 = convert_from_optional(mode.max_charge_power_L2);
    const auto max_charge_L3 = convert_from_optional(mode.max_charge_power_L3);
    const auto max_charge_total = max_charge_L1 + max_charge_L2.value_or(0.0) + max_charge_L3.value_or(0.0);

    const auto min_charge_L1 = dt::from_RationalNumber(mode.min_charge_power);
    const auto min_charge_L2 = convert_from_optional(mode.min_charge_power_L2);
    const auto min_charge_L3 = convert_from_optional(mode.min_charge_power_L3);
    const auto min_charge_total = min_charge_L1 + min_charge_L2.value_or(0.0) + min_charge_L3.value_or(0.0);

    const auto max_discharge_L1 = dt::from_RationalNumber(mode.max_discharge_power);
    const auto max_discharge_L2 = convert_from_optional(mode.max_discharge_power_L2);
    const auto max_discharge_L3 = convert_from_optional(mode.max_discharge_power_L3);
    const auto max_discharge_total = max_discharge_L1 + max_discharge_L2.value_or(0.0) + max_discharge_L3.value_or(0.0);

    const auto min_discharge_L1 = dt::from_RationalNumber(mode.min_discharge_power);
    const auto min_discharge_L2 = convert_from_optional(mode.min_discharge_power_L2);
    const auto min_discharge_L3 = convert_from_optional(mode.min_discharge_power_L3);
    const auto min_discharge_total = min_discharge_L1 + min_discharge_L2.value_or(0.0) + min_discharge_L3.value_or(0.0);

    types::iso15118::AcEvPowerLimits ac_ev_power_limits;

    ac_ev_power_limits.max_charge_power = {max_charge_total, std::make_optional<float>(max_charge_L1), max_charge_L2,
                                           max_charge_L3};
    ac_ev_power_limits.min_charge_power = {min_charge_total, std::make_optional<float>(min_charge_L1), min_charge_L2,
                                           min_charge_L3};
    ac_ev_power_limits.max_discharge_power = {max_discharge_total, std::make_optional<float>(max_discharge_L1),
                                              max_discharge_L2, max_discharge_L3};
    ac_ev_power_limits.min_discharge_power = {min_discharge_total, std::make_optional<float>(min_discharge_L1),
                                              min_discharge_L2, min_discharge_L3};

    return ac_ev_power_limits;
}

types::iso15118::AcEvPowerLimits fill_ac_ev_power_limits(const dt::Scheduled_AC_CLReqControlMode& mode) {
    const auto max_charge_L1 = convert_from_optional(mode.max_charge_power);
    const auto max_charge_L2 = convert_from_optional(mode.max_charge_power_L2);
    const auto max_charge_L3 = convert_from_optional(mode.max_charge_power_L3);
    const auto max_charge_total =
        max_charge_L1.value_or(0.0) + max_charge_L2.value_or(0.0) + max_charge_L3.value_or(0.0);

    const auto min_charge_L1 = convert_from_optional(mode.min_charge_power);
    const auto min_charge_L2 = convert_from_optional(mode.min_charge_power_L2);
    const auto min_charge_L3 = convert_from_optional(mode.min_charge_power_L3);
    const auto min_charge_total =
        min_charge_L1.value_or(0.0) + min_charge_L2.value_or(0.0) + min_charge_L3.value_or(0.0);

    types::iso15118::AcEvPowerLimits ac_ev_power_limits;

    ac_ev_power_limits.max_charge_power =
        (max_charge_L1.has_value() or max_charge_L2.has_value() or max_charge_L3.has_value())
            ? std::make_optional<types::units::Power>({max_charge_total, max_charge_L1, max_charge_L2, max_charge_L3})
            : std::nullopt;
    ac_ev_power_limits.min_charge_power =
        (max_charge_L1.has_value() or max_charge_L2.has_value() or max_charge_L3.has_value())
            ? std::make_optional<types::units::Power>({min_charge_total, min_charge_L1, min_charge_L2, min_charge_L3})
            : std::nullopt;

    return ac_ev_power_limits;
}

types::iso15118::AcEvPowerLimits fill_ac_ev_power_limits(const dt::BPT_Scheduled_AC_CLReqControlMode& mode) {
    const auto max_charge_L1 = convert_from_optional(mode.max_charge_power);
    const auto max_charge_L2 = convert_from_optional(mode.max_charge_power_L2);
    const auto max_charge_L3 = convert_from_optional(mode.max_charge_power_L3);
    const auto max_charge_total =
        max_charge_L1.value_or(0.0) + max_charge_L2.value_or(0.0) + max_charge_L3.value_or(0.0);

    const auto min_charge_L1 = convert_from_optional(mode.min_charge_power);
    const auto min_charge_L2 = convert_from_optional(mode.min_charge_power_L2);
    const auto min_charge_L3 = convert_from_optional(mode.min_charge_power_L3);
    const auto min_charge_total =
        min_charge_L1.value_or(0.0) + min_charge_L2.value_or(0.0) + min_charge_L3.value_or(0.0);

    const auto max_discharge_L1 = convert_from_optional(mode.max_discharge_power);
    const auto max_discharge_L2 = convert_from_optional(mode.max_discharge_power_L2);
    const auto max_discharge_L3 = convert_from_optional(mode.max_discharge_power_L3);
    const auto max_discharge_total =
        max_discharge_L1.value_or(0.0) + max_discharge_L2.value_or(0.0) + max_discharge_L3.value_or(0.0);

    const auto min_discharge_L1 = convert_from_optional(mode.min_discharge_power);
    const auto min_discharge_L2 = convert_from_optional(mode.min_discharge_power_L2);
    const auto min_discharge_L3 = convert_from_optional(mode.min_discharge_power_L3);
    const auto min_discharge_total =
        min_discharge_L1.value_or(0.0) + min_discharge_L2.value_or(0.0) + min_discharge_L3.value_or(0.0);

    types::iso15118::AcEvPowerLimits ac_ev_power_limits;

    ac_ev_power_limits.max_charge_power =
        (max_charge_L1.has_value() or max_charge_L2.has_value() or max_charge_L3.has_value())
            ? std::make_optional<types::units::Power>({max_charge_total, max_charge_L1, max_charge_L2, max_charge_L3})
            : std::nullopt;
    ac_ev_power_limits.min_charge_power =
        (min_charge_L1.has_value() or min_charge_L2.has_value() or min_charge_L3.has_value())
            ? std::make_optional<types::units::Power>({min_charge_total, min_charge_L1, min_charge_L2, min_charge_L3})
            : std::nullopt;
    ac_ev_power_limits.max_discharge_power =
        (max_discharge_L1.has_value() or max_discharge_L2.has_value() or max_discharge_L3.has_value())
            ? std::make_optional<types::units::Power>(
                  {max_discharge_total, max_discharge_L1, max_discharge_L2, max_discharge_L3})
            : std::nullopt;
    ac_ev_power_limits.min_discharge_power =
        (min_discharge_L1.has_value() or min_discharge_L2.has_value() or min_discharge_L3.has_value())
            ? std::make_optional<types::units::Power>(
                  {min_discharge_total, min_discharge_L1, min_discharge_L2, min_discharge_L3})
            : std::nullopt;
    return ac_ev_power_limits;
}

types::iso15118::AcEvPresentPowerValues fill_ac_ev_present_power_values(const dt::Dynamic_AC_CLReqControlMode& mode) {
    types::iso15118::AcEvPresentPowerValues present_values{};

    const auto present_active_power_L1 = dt::from_RationalNumber(mode.present_active_power);
    const auto present_active_power_L2 = convert_from_optional(mode.present_active_power_L2);
    const auto present_active_power_L3 = convert_from_optional(mode.present_active_power_L3);
    const auto present_active_power_total =
        present_active_power_L1 + present_active_power_L2.value_or(0.0) + present_active_power_L3.value_or(0.0);

    const auto present_reactive_power_L1 = dt::from_RationalNumber(mode.present_reactive_power);
    const auto present_reactive_power_L2 = convert_from_optional(mode.present_reactive_power_L2);
    const auto present_reactive_power_L3 = convert_from_optional(mode.present_reactive_power_L3);
    const auto present_reactive_power_total =
        present_reactive_power_L1 + present_reactive_power_L2.value_or(0.0) + present_reactive_power_L3.value_or(0.0);

    present_values.present_active_power = {present_active_power_total, present_active_power_L1, present_active_power_L2,
                                           present_active_power_L3};

    present_values.present_reactive_power = {present_reactive_power_total, present_reactive_power_L1,
                                             present_reactive_power_L2, present_reactive_power_L3};

    return present_values;
}

types::iso15118::AcEvPresentPowerValues fill_ac_ev_present_power_values(const dt::Scheduled_AC_CLReqControlMode& mode) {
    types::iso15118::AcEvPresentPowerValues present_values{};

    const auto present_active_power_L1 = dt::from_RationalNumber(mode.present_active_power);
    const auto present_active_power_L2 = convert_from_optional(mode.present_active_power_L2);
    const auto present_active_power_L3 = convert_from_optional(mode.present_active_power_L3);
    const auto present_active_power_total =
        present_active_power_L1 + present_active_power_L2.value_or(0.0) + present_active_power_L3.value_or(0.0);

    const auto present_reactive_power_L1 = convert_from_optional(mode.present_reactive_power);
    const auto present_reactive_power_L2 = convert_from_optional(mode.present_reactive_power_L2);
    const auto present_reactive_power_L3 = convert_from_optional(mode.present_reactive_power_L3);
    const auto present_reactive_power_total = present_reactive_power_L1.value_or(0.0) +
                                              present_reactive_power_L2.value_or(0.0) +
                                              present_reactive_power_L3.value_or(0.0);

    present_values.present_active_power = {present_active_power_total, present_active_power_L1, present_active_power_L2,
                                           present_active_power_L3};

    present_values.present_reactive_power =
        (present_reactive_power_L1.has_value() or present_reactive_power_L2.has_value() or
         present_reactive_power_L3.has_value())
            ? std::make_optional<types::units::Power>({present_reactive_power_total, present_reactive_power_L1,
                                                       present_reactive_power_L2, present_reactive_power_L3})
            : std::nullopt;

    return present_values;
}

} // namespace module::charger
