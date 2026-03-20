// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <algorithm>

#include <iso15118/d20/state/dc_charge_parameter_discovery.hpp>
#include <iso15118/d20/state/schedule_exchange.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/dc_charge_parameter_discovery.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

using DC_ModeReq = dt::DC_CPDReqEnergyTransferMode;
using BPT_DC_ModeReq = dt::BPT_DC_CPDReqEnergyTransferMode;

using DC_ModeRes = dt::DC_CPDResEnergyTransferMode;
using BPT_DC_ModeRes = dt::BPT_DC_CPDResEnergyTransferMode;

template <typename In, typename Out> void convert(Out& out, const In& in);

template <> void convert(DC_ModeRes& out, const d20::DcTransferLimits& in) {
    out.max_charge_power = in.charge_limits.power.max;
    out.min_charge_power = in.charge_limits.power.min;
    out.max_charge_current = in.charge_limits.current.max;
    out.min_charge_current = in.charge_limits.current.min;
    out.max_voltage = in.voltage.max;
    out.min_voltage = in.voltage.min;
    out.power_ramp_limit = in.power_ramp_limit;
}

template <> void convert(BPT_DC_ModeRes& out, const d20::DcTransferLimits& in) {
    convert(static_cast<DC_ModeRes&>(out), in);

    if (in.discharge_limits.has_value()) {
        auto& discharge_limits = in.discharge_limits.value();
        out.max_discharge_power = discharge_limits.power.max;
        out.min_discharge_power = discharge_limits.power.min;
        out.max_discharge_current = discharge_limits.current.max;
        out.min_discharge_current = discharge_limits.current.min;
    }
}

bool handle_compatibility_check(const d20::DcTransferLimits& evse_dc_limits,
                                const std::variant<DC_ModeReq, BPT_DC_ModeReq>& ev_limits,
                                d20::DcTransferLimits& out_limits) {
    // In IEC 61851-23-3 a compatibility check is required
    constexpr float MAX_VOLTAGE_OFFSET = 50.f;
    constexpr float MAX_VOLTAGE_THRESHOLD = 500.f;
    constexpr float MAX_VOLTAGE_FACTOR = 1.1f;
    constexpr float DEFAULT_MAX_POWER_IF_UNSET = 200000.f;
    bool compatibility_flag = true;

    float ev_max_power, ev_max_current, ev_max_voltage;

    // Start with a copy for output
    out_limits = evse_dc_limits;

    // Extract EV values
    if (const auto* mode = std::get_if<DC_ModeReq>(&ev_limits)) {
        ev_max_power = dt::from_RationalNumber(mode->max_charge_power);
        ev_max_current = dt::from_RationalNumber(mode->max_charge_current);
        ev_max_voltage = dt::from_RationalNumber(mode->max_voltage);
    } else if (const auto* mode = std::get_if<BPT_DC_ModeReq>(&ev_limits)) {
        ev_max_power = dt::from_RationalNumber(mode->max_charge_power);
        ev_max_current = dt::from_RationalNumber(mode->max_charge_current);
        ev_max_voltage = dt::from_RationalNumber(mode->max_voltage);
    } else {
        return false;
    }

    // CC.5.6 f-h) Checks are handled by the EV - not relevant here
    // CC.5.6 1) not relevant here is just a note that CPD and RATED limits are the same sent by EV

    // CC.5.6.2 a) Max voltage
    auto& out_max_voltage = out_limits.voltage.max;
    float evse_max_voltage = dt::from_RationalNumber(out_max_voltage);
    if (ev_max_voltage <= MAX_VOLTAGE_THRESHOLD) {
        float new_evse_max_voltage = std::min({ev_max_voltage + MAX_VOLTAGE_OFFSET, MAX_VOLTAGE_THRESHOLD});
        if (evse_max_voltage > new_evse_max_voltage) {
            logf_info("Compatibility check: max voltage changed (EVSE: %.1f V → adjusted value: %.1f V)",
                      evse_max_voltage, new_evse_max_voltage);
            out_max_voltage = dt::from_float(new_evse_max_voltage);
        }
    } else {
        float new_evse_max_voltage = ev_max_voltage * MAX_VOLTAGE_FACTOR;
        if (evse_max_voltage > new_evse_max_voltage) {
            logf_info("Compatibility check: max voltage changed (EVSE: %.1f V → adjusted value: %.1f V)",
                      evse_max_voltage, new_evse_max_voltage);
            out_max_voltage = dt::from_float(new_evse_max_voltage);
        }
    }

    // CC.5.6.2 b) Max current
    auto& out_max_current = out_limits.charge_limits.current.max;
    float evse_max_current = dt::from_RationalNumber(out_max_current);
    if (evse_max_current > ev_max_current) {
        logf_info("Compatibility check: max current changed (EVSE: %.1f A → adjusted value: %.1f A)", evse_max_current,
                  ev_max_current);
        out_max_current = dt::from_float(ev_max_current);
    }

    // CC.5.6.2 c) Max power
    auto& out_max_power = out_limits.charge_limits.power.max;
    float evse_max_power = dt::from_RationalNumber(out_max_power);
    float ev_power_max =
        ev_max_power > 0 ? ev_max_power : std::max(ev_max_voltage * ev_max_current, DEFAULT_MAX_POWER_IF_UNSET);
    if (evse_max_power > ev_power_max) {
        logf_info("Compatibility check: max power changed (EVSE: %.1f W → adjusted value: %.1f W)", evse_max_power,
                  ev_power_max);
        out_max_power = dt::from_float(ev_power_max);
    }

    // CC.5.6.2 d-f) Is not relevant here because EVerest makes no different between RATED and CPD up to now

    // CC.5.6.2 g.i) If any of the below checks fail, the EVSE shall reject the request with response code
    // FAILED_WrongChargeParameter CC.5.6.2 g) Relation between EVSE min and EV max voltage
    auto& out_min_voltage = out_limits.voltage.min;
    float evse_min_voltage = dt::from_RationalNumber(out_min_voltage);
    if (evse_min_voltage >= ev_max_voltage) {
        logf_error("EVSE min voltage %.1f V >= EV max voltage %.1f V", evse_min_voltage, ev_max_voltage);
        compatibility_flag = false;
    }

    // CC.5.6 g) Relation between EVSE min current and EV max current
    auto& out_min_current = out_limits.charge_limits.current.min;
    float evse_min_current = dt::from_RationalNumber(out_min_current);
    if (evse_min_current >= ev_max_current) {
        logf_error("EVSE min current %.1f A >= EV max current %.1f A", evse_min_current, ev_max_current);
        compatibility_flag = false;
    }

    // CC.5.6 g) Relation between EVSE min power and EV max power
    auto& out_min_power = out_limits.charge_limits.power.min;
    float evse_min_power = dt::from_RationalNumber(out_min_power);
    if (evse_min_power >= ev_power_max) {
        logf_error("EVSE min power %.1f W >= EV max power %.1f W", evse_min_power, ev_power_max);
        compatibility_flag = false;
    }

    // CC.5.6 3-5) Are EV checks and not relevant here

    return compatibility_flag;
}

message_20::DC_ChargeParameterDiscoveryResponse
handle_request(const message_20::DC_ChargeParameterDiscoveryRequest& req, const d20::Session& session,
               const d20::DcTransferLimits& dc_limits) {

    message_20::DC_ChargeParameterDiscoveryResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    const auto selected_energy_service = session.get_selected_services().selected_energy_service;

    if (std::holds_alternative<DC_ModeReq>(req.transfer_mode)) {
        if (not(selected_energy_service == dt::ServiceCategory::DC or
                selected_energy_service == dt::ServiceCategory::MCS)) {
            return response_with_code(res, dt::ResponseCode::FAILED_WrongChargeParameter);
        }

        auto& mode = res.transfer_mode.emplace<DC_ModeRes>();
        convert(mode, dc_limits);

    } else if (std::holds_alternative<BPT_DC_ModeReq>(req.transfer_mode)) {
        if (not(selected_energy_service == dt::ServiceCategory::DC_BPT or
                selected_energy_service == dt::ServiceCategory::MCS_BPT)) {
            return response_with_code(res, dt::ResponseCode::FAILED_WrongChargeParameter);
        }

        if (not dc_limits.discharge_limits.has_value()) {
            logf_error("Transfer mode is BPT, but only dc limits without discharge limits are provided!");
            return response_with_code(res, dt::ResponseCode::FAILED);
        }

        auto& mode = res.transfer_mode.emplace<BPT_DC_ModeRes>();
        convert(mode, dc_limits);

    } else {
        // Not supported transfer_mode
        return response_with_code(res, dt::ResponseCode::FAILED_WrongChargeParameter);
    }

    return response_with_code(res, dt::ResponseCode::OK);
}

void DC_ChargeParameterDiscovery::enter() {
    m_ctx.log.enter_state("DC_ChargeParameterDiscovery");
}

Result DC_ChargeParameterDiscovery::feed(Event ev) {

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::DC_ChargeParameterDiscoveryRequest>()) {

        auto dc_max_limits = session::feedback::DcMaximumLimits{};

        if (const auto* mode = std::get_if<DC_ModeReq>(&req->transfer_mode)) {
            dc_max_limits.current = dt::from_RationalNumber(mode->max_charge_current);
            dc_max_limits.voltage = dt::from_RationalNumber(mode->max_voltage);
            dc_max_limits.power = dt::from_RationalNumber(mode->max_charge_power);

            logf_info("Max charge current %fA", dt::from_RationalNumber(mode->max_charge_current));

            // Set EV transfer limits
            m_ctx.session_ev_info.ev_transfer_limits.emplace<DC_ModeReq>(*mode);
        } else if (const auto* mode = std::get_if<BPT_DC_ModeReq>(&req->transfer_mode)) {
            dc_max_limits.current = dt::from_RationalNumber(mode->max_charge_current);
            dc_max_limits.voltage = dt::from_RationalNumber(mode->max_voltage);
            dc_max_limits.power = dt::from_RationalNumber(mode->max_charge_power);

            logf_info("Max charge current %fA", dt::from_RationalNumber(mode->max_charge_current));
            logf_info("Max discharge current %fA", dt::from_RationalNumber(mode->max_discharge_current));

            // Set EV transfer limits
            m_ctx.session_ev_info.ev_transfer_limits.emplace<BPT_DC_ModeReq>(*mode);
        }

        // Compatibility check and use of new limits
        d20::DcTransferLimits checked_limits;
        bool compatible =
            handle_compatibility_check(m_ctx.session_config.powersupply_limits, req->transfer_mode, checked_limits);
        message_20::DC_ChargeParameterDiscoveryResponse res;
        if (!compatible) {
            res = handle_request(*req, m_ctx.session, checked_limits);
            res.response_code = dt::ResponseCode::FAILED_WrongChargeParameter;
            m_ctx.respond(res);
            m_ctx.feedback.dc_max_limits(dc_max_limits);
            m_ctx.session_stopped = true;
            return {};
        }
        // Save adapted limits for later states (e.g. charge loop)
        m_ctx.session_config.dc_limits = checked_limits;
        m_ctx.dc_limits_locked_after_charge_param = true;
        m_ctx.dc_limits_after_charge_param_bounds = checked_limits;
        res = handle_request(*req, m_ctx.session, checked_limits);
        m_ctx.respond(res);

        m_ctx.feedback.dc_max_limits(dc_max_limits);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        return m_ctx.create_state<ScheduleExchange>();
    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, m_ctx.session);

        m_ctx.respond(res);
        m_ctx.session_stopped = true;

        return {};
    } else {
        m_ctx.log("expected DC_ChargeParameterDiscovery! But code type id: %d", variant->get_type());
        m_ctx.session_stopped = true;

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        return {};
    }
}

} // namespace iso15118::d20::state
