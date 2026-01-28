// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/d20/state/dc_charge_loop.hpp>
#include <iso15118/d20/state/dc_welding_detection.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/dc_charge_loop.hpp>
#include <iso15118/detail/d20/state/power_delivery.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

using Scheduled_DC_Req = dt::Scheduled_DC_CLReqControlMode;
using Scheduled_BPT_DC_Req = dt::BPT_Scheduled_DC_CLReqControlMode;
using Dynamic_DC_Req = dt::Dynamic_DC_CLReqControlMode;
using Dynamic_BPT_DC_Req = dt::BPT_Dynamic_DC_CLReqControlMode;

using Scheduled_DC_Res = dt::Scheduled_DC_CLResControlMode;
using Scheduled_BPT_DC_Res = dt::BPT_Scheduled_DC_CLResControlMode;
using Dynamic_DC_Res = dt::Dynamic_DC_CLResControlMode;
using Dynamic_BPT_DC_Res = dt::BPT_Dynamic_DC_CLResControlMode;

template <typename In, typename Out> void convert(Out& out, const In& in);

template <> void convert(Scheduled_DC_Res& out, const d20::DcTransferLimits& in) {
    out.max_charge_power = in.charge_limits.power.max;
    out.min_charge_power = in.charge_limits.power.min;
    out.max_charge_current = in.charge_limits.current.max;
    out.max_voltage = in.voltage.max;
}

template <> void convert(Scheduled_BPT_DC_Res& out, const d20::DcTransferLimits& in) {
    out.max_charge_power = in.charge_limits.power.max;
    out.min_charge_power = in.charge_limits.power.min;
    out.max_charge_current = in.charge_limits.current.max;
    out.max_voltage = in.voltage.max;
    out.min_voltage = in.voltage.min;

    if (in.discharge_limits.has_value()) {
        auto& discharge_limits = in.discharge_limits.value();
        out.max_discharge_power = discharge_limits.power.max;
        out.min_discharge_power = discharge_limits.power.min;
        out.max_discharge_current = discharge_limits.current.max;
    }
}

template <> void convert(Dynamic_DC_Res& out, const d20::DcTransferLimits& in) {
    out.max_charge_power = in.charge_limits.power.max;
    out.min_charge_power = in.charge_limits.power.min;
    out.max_charge_current = in.charge_limits.current.max;
    out.max_voltage = in.voltage.max;
}

template <> void convert(Dynamic_BPT_DC_Res& out, const d20::DcTransferLimits& in) {
    out.max_charge_power = in.charge_limits.power.max;
    out.min_charge_power = in.charge_limits.power.min;
    out.max_charge_current = in.charge_limits.current.max;
    out.max_voltage = in.voltage.max;
    out.min_voltage = in.voltage.min;

    if (in.discharge_limits.has_value()) {
        auto& discharge_limits = in.discharge_limits.value();
        out.max_discharge_power = discharge_limits.power.max;
        out.min_discharge_power = discharge_limits.power.min;
        out.max_discharge_current = discharge_limits.current.max;
    }
}

namespace {
template <typename T>
void set_dynamic_parameters_in_res(T& res_mode, const UpdateDynamicModeParameters& parameters,
                                   uint64_t header_timestamp) {
    if (parameters.departure_time) {
        const auto departure_time = static_cast<uint64_t>(parameters.departure_time.value());
        if (departure_time > header_timestamp) {
            res_mode.departure_time = static_cast<uint32_t>(departure_time - header_timestamp);
        }
    }
    res_mode.target_soc = parameters.target_soc;
    res_mode.minimum_soc = parameters.min_soc;
    res_mode.ack_max_delay = 30; // TODO(sl) what to send here and define 30 seconds as const
}
} // namespace

message_20::DC_ChargeLoopResponse handle_request(const message_20::DC_ChargeLoopRequest& req,
                                                 const d20::Session& session, const float present_voltage,
                                                 const float present_current, const bool stop, const bool pause,
                                                 const DcTransferLimits& dc_limits,
                                                 const UpdateDynamicModeParameters& dynamic_parameters) {

    message_20::DC_ChargeLoopResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    const auto& selected_services = session.get_selected_services();
    const auto selected_control_mode = selected_services.selected_control_mode;
    const auto selected_energy_service = selected_services.selected_energy_service;
    const auto selected_mobility_needs_mode = selected_services.selected_mobility_needs_mode;

    if (std::holds_alternative<Scheduled_DC_Req>(req.control_mode)) {

        // If the ev sends a false control mode or a false energy service other than the previous selected ones, then
        // the charger should terminate the session
        if (selected_control_mode != dt::ControlMode::Scheduled or
            not(selected_energy_service == dt::ServiceCategory::DC or
                selected_energy_service == dt::ServiceCategory::MCS)) {
            return response_with_code(res, dt::ResponseCode::FAILED);
        }

        auto& res_mode = res.control_mode.emplace<Scheduled_DC_Res>();
        convert(res_mode, dc_limits);

    } else if (std::holds_alternative<Scheduled_BPT_DC_Req>(req.control_mode)) {

        // If the ev sends a false control mode or a false energy service other than the previous selected ones, then
        // the charger should terminate the session
        if (selected_control_mode != dt::ControlMode::Scheduled or
            not(selected_energy_service == dt::ServiceCategory::DC_BPT or
                selected_energy_service == dt::ServiceCategory::MCS_BPT)) {
            return response_with_code(res, dt::ResponseCode::FAILED);
        }

        if (not dc_limits.discharge_limits.has_value()) {
            logf_error("Transfer mode is BPT, but only dc limits without discharge limits are provided!");
            return response_with_code(res, dt::ResponseCode::FAILED);
        }

        auto& res_mode = res.control_mode.emplace<Scheduled_BPT_DC_Res>();
        convert(res_mode, dc_limits);

    } else if (std::holds_alternative<Dynamic_DC_Req>(req.control_mode)) {

        // If the ev sends a false control mode or a false energy service other than the previous selected ones, then
        // the charger should terminate the session
        if (selected_control_mode != dt::ControlMode::Dynamic or
            not(selected_energy_service == dt::ServiceCategory::DC or
                selected_energy_service == dt::ServiceCategory::MCS)) {
            return response_with_code(res, dt::ResponseCode::FAILED);
        }

        auto& res_mode = res.control_mode.emplace<Dynamic_DC_Res>();
        convert(res_mode, dc_limits);

        if (selected_mobility_needs_mode == dt::MobilityNeedsMode::ProvidedBySecc) {
            set_dynamic_parameters_in_res(res_mode, dynamic_parameters, res.header.timestamp);
        }

    } else if (std::holds_alternative<Dynamic_BPT_DC_Req>(req.control_mode)) {

        // If the ev sends a false control mode or a false energy service other than the previous selected ones, then
        // the charger should terminate the session
        if (selected_control_mode != dt::ControlMode::Dynamic or
            not(selected_energy_service == dt::ServiceCategory::DC_BPT or
                selected_energy_service == dt::ServiceCategory::MCS_BPT)) {
            return response_with_code(res, dt::ResponseCode::FAILED);
        }

        if (not dc_limits.discharge_limits.has_value()) {
            logf_error("Transfer mode is BPT, but only dc limits without discharge limits are provided!");
            return response_with_code(res, dt::ResponseCode::FAILED);
        }

        auto& res_mode = res.control_mode.emplace<Dynamic_BPT_DC_Res>();
        convert(res_mode, dc_limits);

        if (selected_mobility_needs_mode == dt::MobilityNeedsMode::ProvidedBySecc) {
            set_dynamic_parameters_in_res(res_mode, dynamic_parameters, res.header.timestamp);
        }
    }

    res.present_voltage = dt::from_float(present_voltage);
    res.present_current = dt::from_float(present_current);

    // TODO(sl): Setting EvseStatus, MeterInfo, Receipt, *_limit_achieved

    if (stop) {
        res.status = {0, dt::EvseNotification::Terminate};
    } else if (pause) {
        const uint16_t notification_max_delay =
            (selected_control_mode == dt::ControlMode::Dynamic) ? 60 : 0; // [V2G20-1850]
        res.status = {notification_max_delay, dt::EvseNotification::Pause};
    }

    return response_with_code(res, dt::ResponseCode::OK);
}

void DC_ChargeLoop::enter() {
    m_ctx.log.enter_state("DC_ChargeLoop");
    dynamic_parameters = m_ctx.cache_dynamic_mode_parameters.value_or(UpdateDynamicModeParameters{});
}

Result DC_ChargeLoop::feed(Event ev) {

    if (ev == Event::CONTROL_MESSAGE) {

        if (const auto* control_data = m_ctx.get_control_event<PresentVoltageCurrent>()) {
            present_voltage = control_data->voltage;
            present_current = control_data->current;
        } else if (const auto* control_data = m_ctx.get_control_event<StopCharging>()) {
            stop = *control_data;
        } else if (const auto* control_data = m_ctx.get_control_event<PauseCharging>()) {
            pause = *control_data;
        } else if (const auto* control_data = m_ctx.get_control_event<UpdateDynamicModeParameters>()) {
            dynamic_parameters = *control_data;
        }

        // Ignore control message
        return {};
    }

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::PowerDeliveryRequest>()) {
        const auto res = handle_request(*req, m_ctx.session, false);

        m_ctx.respond(res);
        m_ctx.feedback.response_code(res.response_code);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        // Reset
        first_entry_in_charge_loop = true;

        // Todo(sl): React properly to Start, Stop, Standby and ScheduleRenegotiation
        // TODO(Sl): How to check if the EV wants do a pause in dynamic mode (This should not happen)
        if (req->charge_progress == dt::Progress::Stop) {
            m_ctx.feedback.signal(session::feedback::Signal::CHARGE_LOOP_FINISHED);
            m_ctx.feedback.signal(session::feedback::Signal::DC_OPEN_CONTACTOR);
            return m_ctx.create_state<DC_WeldingDetection>();
        }

        return {};
    } else if (const auto req = variant->get_if<message_20::DC_ChargeLoopRequest>()) {
        if (first_entry_in_charge_loop) {
            m_ctx.feedback.signal(session::feedback::Signal::CHARGE_LOOP_STARTED);
            first_entry_in_charge_loop = false;
        }

        const auto res = handle_request(*req, m_ctx.session, present_voltage, present_current, stop, pause,
                                        m_ctx.session_config.dc_limits, dynamic_parameters);

        m_ctx.respond(res);
        m_ctx.feedback.response_code(res.response_code);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        m_ctx.feedback.dc_charge_loop_req(req->control_mode);
        m_ctx.feedback.dc_charge_loop_req(req->present_voltage);
        m_ctx.feedback.dc_charge_loop_req(req->meter_info_requested);
        if (req->display_parameters) {
            m_ctx.feedback.dc_charge_loop_req(*req->display_parameters);
        }

        return {};
    } else {
        m_ctx.log("Expected PowerDeliveryReq or DC_ChargeLoopReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.feedback.response_code(dt::ResponseCode::FAILED_SequenceError);
        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
