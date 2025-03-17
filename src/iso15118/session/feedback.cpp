// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/session/feedback.hpp>

#include <iso15118/detail/helper.hpp>

namespace iso15118::session {

Feedback::Feedback(feedback::Callbacks callbacks_) : callbacks(std::move(callbacks_)) {
}

void Feedback::signal(feedback::Signal signal) const {
    call_if_available(callbacks.signal, signal);
}

void Feedback::dc_pre_charge_target_voltage(float voltage) const {
    call_if_available(callbacks.dc_pre_charge_target_voltage, voltage);
}

void Feedback::dc_charge_loop_req(const feedback::DcChargeLoopReq& req_values) const {
    call_if_available(callbacks.dc_charge_loop_req, req_values);
}

void Feedback::dc_max_limits(const feedback::DcMaximumLimits& max_limits) const {
    call_if_available(callbacks.dc_max_limits, max_limits);
}

void Feedback::v2g_message(const message_20::Type& v2g_message) const {
    call_if_available(callbacks.v2g_message, v2g_message);
}

void Feedback::evcc_id(const std::string& evccid) const {
    call_if_available(callbacks.evccid, evccid);
}

void Feedback::selected_protocol(const std::string& selected_protocol) const {
    call_if_available(callbacks.selected_protocol, selected_protocol);
}

void Feedback::notify_ev_charging_needs(const dt::ServiceCategory& service_category,
                                        const std::optional<dt::AcConnector>& ac_connector,
                                        const dt::ControlMode& control_mode,
                                        const dt::MobilityNeedsMode& mobility_needs_mode,
                                        const feedback::EvseTransferLimits& evse_limits,
                                        const feedback::EvTransferLimits& ev_limits,
                                        const feedback::EvSEControlMode& ev_control_mode) const {
    call_if_available(callbacks.notify_ev_charging_needs, service_category, ac_connector, control_mode,
                      mobility_needs_mode, evse_limits, ev_limits, ev_control_mode);
}

} // namespace iso15118::session
