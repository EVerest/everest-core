// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cmath>
#include <functional>
#include <optional>
#include <string>
#include <variant>

#include <iso15118/d20/ev_information.hpp>
#include <iso15118/d20/limits.hpp>
#include <iso15118/d20/session.hpp>
#include <iso15118/message/dc_charge_loop.hpp>
#include <iso15118/message/dc_charge_parameter_discovery.hpp>
#include <iso15118/message/schedule_exchange.hpp>
#include <iso15118/message/type.hpp>

namespace iso15118::session {

namespace dt = message_20::datatypes;

namespace feedback {

enum class Signal {
    REQUIRE_AUTH_EIM,
    START_CABLE_CHECK,
    SETUP_FINISHED,
    PRE_CHARGE_STARTED,
    CHARGE_LOOP_STARTED,
    CHARGE_LOOP_FINISHED,
    DC_OPEN_CONTACTOR,
    DLINK_TERMINATE,
    DLINK_ERROR,
    DLINK_PAUSE,
};

struct DcMaximumLimits {
    float voltage{NAN};
    float current{NAN};
    float power{NAN};
};

using PresentVoltage = dt::RationalNumber;
using MeterInfoRequested = bool;
using DcReqControlMode = std::variant<dt::Scheduled_DC_CLReqControlMode, dt::BPT_Scheduled_DC_CLReqControlMode,
                                      dt::Dynamic_DC_CLReqControlMode, dt::BPT_Dynamic_DC_CLReqControlMode>;

using DcChargeLoopReq = std::variant<DcReqControlMode, dt::DisplayParameters, PresentVoltage, MeterInfoRequested>;

// TODO(ioan): preparation for AC limits
using EvseTransferLimits = std::variant<d20::DcTransferLimits>;

using EvTransferLimits = std::variant<dt::DC_CPDReqEnergyTransferMode, dt::BPT_DC_CPDReqEnergyTransferMode>;
using EvSEControlMode = std::variant<dt::Dynamic_SEReqControlMode, dt::Scheduled_SEReqControlMode>;

struct Callbacks {
    std::function<void(Signal)> signal;
    std::function<void(float)> dc_pre_charge_target_voltage;
    std::function<void(const DcChargeLoopReq&)> dc_charge_loop_req;
    std::function<void(const DcMaximumLimits&)> dc_max_limits;
    std::function<void(const message_20::Type&)> v2g_message;
    std::function<void(const std::string&)> evccid;
    std::function<void(const std::string&)> selected_protocol;

    std::function<void(const dt::ServiceCategory&, const std::optional<dt::AcConnector>&, const dt::ControlMode&,
                       const dt::MobilityNeedsMode&, const EvseTransferLimits&, const EvTransferLimits&,
                       const EvSEControlMode&)>
        notify_ev_charging_needs;
    std::function<void(const d20::SelectedServiceParameters&)> selected_service_parameters;
    std::function<void(const d20::EVInformation&)> ev_information;
};

} // namespace feedback

class Feedback {
public:
    Feedback(feedback::Callbacks);

    void signal(feedback::Signal) const;
    void dc_pre_charge_target_voltage(float) const;
    void dc_charge_loop_req(const feedback::DcChargeLoopReq&) const;
    void dc_max_limits(const feedback::DcMaximumLimits&) const;
    void v2g_message(const message_20::Type&) const;
    void evcc_id(const std::string&) const;
    void selected_protocol(const std::string&) const;

    void notify_ev_charging_needs(const dt::ServiceCategory&, const std::optional<dt::AcConnector>&,
                                  const dt::ControlMode&, const dt::MobilityNeedsMode&,
                                  const feedback::EvseTransferLimits&, const feedback::EvTransferLimits&,
                                  const feedback::EvSEControlMode&) const;
    void selected_service_parameters(const d20::SelectedServiceParameters&) const;
    void ev_information(const d20::EVInformation&) const;

private:
    feedback::Callbacks callbacks;
};

} // namespace iso15118::session
