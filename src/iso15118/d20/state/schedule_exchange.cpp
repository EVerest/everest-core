// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <ctime>

#include <iso15118/d20/state/dc_cable_check.hpp>
#include <iso15118/d20/state/schedule_exchange.hpp>

#include <iso15118/detail/d20/context_helper.hpp>
#include <iso15118/detail/d20/state/schedule_exchange.hpp>
#include <iso15118/detail/d20/state/session_stop.hpp>
#include <iso15118/detail/helper.hpp>

namespace iso15118::d20::state {

namespace dt = message_20::datatypes;

using ScheduledReqControlMode = message_20::datatypes::Scheduled_SEReqControlMode;
using ScheduledResControlMode = message_20::datatypes::Scheduled_SEResControlMode;

using DynamicReqControlMode = message_20::datatypes::Dynamic_SEReqControlMode;
using DynamicResControlMode = message_20::datatypes::Dynamic_SEResControlMode;

namespace {
auto create_default_scheduled_control_mode(const dt::RationalNumber& max_power) {
    dt::ScheduleTuple schedule;
    schedule.schedule_tuple_id = 1;
    schedule.charging_schedule.power_schedule.time_anchor =
        static_cast<uint64_t>(std::time(nullptr)); // PowerSchedule is now active

    dt::PowerScheduleEntry power_schedule;
    power_schedule.power = max_power;
    power_schedule.duration = dt::SCHEDULED_POWER_DURATION_S;
    schedule.charging_schedule.power_schedule.entries.push_back(power_schedule);

    ScheduledResControlMode scheduled_mode{};

    // Providing no price schedule!
    // NOTE: Agreement on iso15118.elaad.io: [V2G20-2176] is not required and should be ignored.
    scheduled_mode.schedule_tuple = {schedule};
    return scheduled_mode;
}

namespace {
void set_dynamic_parameters_in_res(DynamicResControlMode& res_mode, const UpdateDynamicModeParameters& parameters,
                                   uint64_t header_timestamp) {
    if (parameters.departure_time) {
        const auto departure_time = static_cast<uint64_t>(parameters.departure_time.value());
        if (departure_time > header_timestamp) {
            res_mode.departure_time = static_cast<uint32_t>(departure_time - header_timestamp);
        }
    }
    res_mode.target_soc = parameters.target_soc;
    res_mode.minimum_soc = parameters.min_soc;
}
} // namespace
} // namespace

namespace dt = message_20::datatypes;

message_20::ScheduleExchangeResponse handle_request(const message_20::ScheduleExchangeRequest& req,
                                                    const d20::Session& session, const dt::RationalNumber& max_power,
                                                    const UpdateDynamicModeParameters& dynamic_parameters) {

    message_20::ScheduleExchangeResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, dt::ResponseCode::FAILED_UnknownSession);
    }

    const auto selected_services = session.get_selected_services();
    const auto selected_control_mode = selected_services.selected_control_mode;
    const auto selected_mobility_needs_mode = selected_services.selected_mobility_needs_mode;

    // Todo(SL): Publish data from request?

    if (selected_control_mode == dt::ControlMode::Scheduled &&
        std::holds_alternative<dt::Scheduled_SEReqControlMode>(req.control_mode)) {

        res.control_mode.emplace<ScheduledResControlMode>(create_default_scheduled_control_mode(max_power));

        // TODO(sl): Adding price schedule
        // TODO(sl): Adding discharging schedule

    } else if (selected_control_mode == dt::ControlMode::Dynamic &&
               std::holds_alternative<DynamicReqControlMode>(req.control_mode)) {

        // TODO(sl): Publish req dynamic mode parameters
        auto& mode = res.control_mode.emplace<DynamicResControlMode>();

        if (selected_mobility_needs_mode == dt::MobilityNeedsMode::ProvidedBySecc) {
            set_dynamic_parameters_in_res(mode, dynamic_parameters, res.header.timestamp);
        }

    } else {
        logf_error("The control mode of the req message does not match the previously agreed contol mode.");
        return response_with_code(res, dt::ResponseCode::FAILED);
    }

    res.processing = dt::Processing::Finished;

    return response_with_code(res, dt::ResponseCode::OK);
}

void ScheduleExchange::enter() {
    m_ctx.log.enter_state("ScheduleExchange");
}

Result ScheduleExchange::feed(Event ev) {

    if (ev == Event::CONTROL_MESSAGE) {

        // TODO(sl): Not sure if the data comes here just in time?
        if (const auto* control_data = m_ctx.get_control_event<UpdateDynamicModeParameters>()) {
            dynamic_parameters = *control_data;
        }

        // Ignore control message
        return {};
    }

    if (ev != Event::V2GTP_MESSAGE) {
        return {};
    }

    const auto variant = m_ctx.pull_request();

    if (const auto req = variant->get_if<message_20::ScheduleExchangeRequest>()) {

        dt::RationalNumber max_charge_power = {0, 0};

        const auto& selected_services = m_ctx.session.get_selected_services();
        const auto selected_energy_service = selected_services.selected_energy_service;

        if (selected_energy_service == dt::ServiceCategory::DC or
            selected_energy_service == dt::ServiceCategory::DC_BPT or
            selected_energy_service == dt::ServiceCategory::MCS or
            selected_energy_service == dt::ServiceCategory::MCS_BPT) {
            max_charge_power = m_ctx.session_config.dc_limits.charge_limits.power.max;
        }

        // We will pass the raw data to the listener, the
        // listener will construct the full required type
        std::optional<dt::AcConnector> ac_connector{};
        if (std::holds_alternative<dt::AcConnector>(selected_services.selected_connector)) {
            ac_connector = std::get<dt::AcConnector>(selected_services.selected_connector);
        }

        // TODO(ioan): prepare for AC transfer limits
        const session::feedback::EvseTransferLimits& evse_limits = m_ctx.session_config.dc_limits;
        const session::feedback::EvTransferLimits& ev_limits = m_ctx.session_ev_info.ev_transfer_limits;

        const auto& control_mode = req->control_mode;

        // Send the charging feedback
        m_ctx.feedback.notify_ev_charging_needs(
            selected_energy_service, ac_connector, selected_services.selected_control_mode,
            selected_services.selected_mobility_needs_mode, evse_limits, ev_limits, control_mode);

        const auto res = handle_request(*req, m_ctx.session, max_charge_power, dynamic_parameters);

        m_ctx.respond(res);

        if (res.response_code >= dt::ResponseCode::FAILED) {
            m_ctx.session_stopped = true;
            return {};
        }

        if (res.processing == dt::Processing::Ongoing) {
            return {};
        }

        return m_ctx.create_state<DC_CableCheck>();
    } else if (const auto req = variant->get_if<message_20::SessionStopRequest>()) {
        const auto res = handle_request(*req, m_ctx.session);

        m_ctx.respond(res);
        m_ctx.session_stopped = true;

        return {};
    } else {
        m_ctx.log("expected ScheduleExchangeReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type req_type = variant->get_type();
        send_sequence_error(req_type, m_ctx);

        m_ctx.session_stopped = true;
        return {};
    }
}

} // namespace iso15118::d20::state
