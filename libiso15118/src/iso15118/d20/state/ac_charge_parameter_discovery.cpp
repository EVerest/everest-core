// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/ac_charge_parameter_discovery.hpp>
#include <iso15118/d20/state/schedule_exchange.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/ac_charge_parameter_discovery.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

using AC_ModeReq = dt::AC_CPDReqEnergyTransferMode;
using BPT_AC_ModeReq = dt::BPT_AC_CPDReqEnergyTransferMode;

using AC_ModeRes = dt::AC_CPDResEnergyTransferMode;
using BPT_AC_ModeRes = dt::BPT_AC_CPDResEnergyTransferMode;

template <typename Out>
void convert(Out& out, const d20::AcTransferLimits& limits, const d20::AcPresentPower& present_power);

template <>
void convert(AC_ModeRes& out, const d20::AcTransferLimits& limits, const d20::AcPresentPower& present_power) {
    out.min_charge_power = limits.charge_power.min;
    out.max_charge_power = limits.charge_power.max;

    if (limits.charge_power_L2.has_value()) {
        out.min_charge_power_L2 = limits.charge_power_L2.value().min;
        out.max_charge_power_L2 = limits.charge_power_L2.value().max;
    }

    if (limits.charge_power_L3.has_value()) {
        out.min_charge_power_L3 = limits.charge_power_L3.value().min;
        out.max_charge_power_L3 = limits.charge_power_L3.value().max;
    }

    out.nominal_frequency = limits.nominal_frequency;
    out.max_power_asymmetry = limits.max_power_asymmetry;
    out.power_ramp_limitation = limits.power_ramp_limitation;
    out.present_active_power = present_power.present_active_power;
    out.present_active_power_L2 = present_power.present_active_power_L2;
    out.present_active_power_L3 = present_power.present_active_power_L3;
}

template <>
void convert(BPT_AC_ModeRes& out, const d20::AcTransferLimits& limits, const d20::AcPresentPower& present_power) {
    convert(static_cast<AC_ModeRes&>(out), limits, present_power);

    if (limits.discharge_power.has_value()) {
        out.min_discharge_power = limits.discharge_power.value().min;
        out.max_discharge_power = limits.discharge_power.value().max;
    }

    if (limits.discharge_power_L2.has_value()) {
        out.min_discharge_power_L2 = limits.discharge_power_L2.value().min;
        out.max_discharge_power_L2 = limits.discharge_power_L2.value().max;
    }

    if (limits.discharge_power_L3.has_value()) {
        out.min_discharge_power_L3 = limits.discharge_power_L3.value().min;
        out.max_discharge_power_L3 = limits.discharge_power_L3.value().max;
    }
}

message_20::AC_ChargeParameterDiscoveryResponse
handle_request(const message_20::AC_ChargeParameterDiscoveryRequest& req, const d20::Session& session,
               const d20::AcTransferLimits& limits, const d20::AcPresentPower& powers) {

    message_20::AC_ChargeParameterDiscoveryResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, message_20::datatypes::ResponseCode::FAILED_UnknownSession);
    }

    const auto selected_energy_service = session.get_selected_services().selected_energy_service;

    if (std::holds_alternative<AC_ModeReq>(req.transfer_mode)) {
        if (selected_energy_service != message_20::datatypes::ServiceCategory::AC) {
            return response_with_code(res, message_20::datatypes::ResponseCode::FAILED_WrongChargeParameter);
        }

        auto& mode = res.transfer_mode.emplace<AC_ModeRes>();
        convert(mode, limits, powers);

    } else if (std::holds_alternative<BPT_AC_ModeReq>(req.transfer_mode)) {
        if (selected_energy_service != message_20::datatypes::ServiceCategory::AC_BPT) {
            return response_with_code(res, message_20::datatypes::ResponseCode::FAILED_WrongChargeParameter);
        }

        auto& mode = res.transfer_mode.emplace<BPT_AC_ModeRes>();
        convert(mode, limits, powers);

    } else {
        return response_with_code(res, message_20::datatypes::ResponseCode::FAILED_WrongChargeParameter);
    }

    return response_with_code(res, message_20::datatypes::ResponseCode::OK);
}

void AC_ChargeParameterDiscovery::enter() {
    m_ctx.log.enter_state("AC_ChargeParameterDiscovery");
    present_powers = m_ctx.cache_ac_present_power.value_or(AcPresentPower{});
}

Result AC_ChargeParameterDiscovery::feed(Event ev) {

    if (ev == Event::CONTROL_MESSAGE) {
        if (const auto* control_data = m_ctx.get_control_event<AcPresentPower>()) {
            present_powers = *control_data;
        }
        return {};
    }

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::AC_ChargeParameterDiscoveryRequest>()) {

        if (const auto* mode = std::get_if<AC_ModeReq>(&req->transfer_mode)) {
            // Set EV transfer limits
            m_ctx.session_ev_info.ev_transfer_limits.emplace<AC_ModeReq>(*mode);
        } else if (const auto* mode = std::get_if<BPT_AC_ModeReq>(&req->transfer_mode)) {
            // Set EV transfer limits
            m_ctx.session_ev_info.ev_transfer_limits.emplace<BPT_AC_ModeReq>(*mode);
        }

        const auto res = handle_request(*req, m_ctx.session, m_ctx.session_config.ac_limits, present_powers);

        m_ctx.respond(res);

        if (res.response_code >= message_20::datatypes::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        m_ctx.feedback.ac_limits(req->transfer_mode);

        return m_ctx.create_state<ScheduleExchange>();

    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, m_ctx.session);

        m_ctx.respond(res);
        m_ctx.session_stopped = true;

        return {};
    } else {
        m_ctx.log("expected AC_ChargeParameterDiscovery! But code type id: %d", variant->get_type());
        m_ctx.session_stopped = true;

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        return {};
    }
}

} // namespace iso15118::d20::state
