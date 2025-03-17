// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cmath>
#include <functional>
#include <optional>
#include <string>
#include <variant>

#include <iso15118/message/dc_charge_loop.hpp>
#include <iso15118/message/type.hpp>

namespace iso15118::session {

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

using PresentVoltage = message_20::datatypes::RationalNumber;
using MeterInfoRequested = bool;
using DcReqControlMode = std::variant<
    message_20::datatypes::Scheduled_DC_CLReqControlMode, message_20::datatypes::BPT_Scheduled_DC_CLReqControlMode,
    message_20::datatypes::Dynamic_DC_CLReqControlMode, message_20::datatypes::BPT_Dynamic_DC_CLReqControlMode>;

using DcChargeLoopReq =
    std::variant<DcReqControlMode, message_20::datatypes::DisplayParameters, PresentVoltage, MeterInfoRequested>;

struct Callbacks {
    std::function<void(Signal)> signal;
    std::function<void(float)> dc_pre_charge_target_voltage;
    std::function<void(const DcChargeLoopReq&)> dc_charge_loop_req;
    std::function<void(const DcMaximumLimits&)> dc_max_limits;
    std::function<void(const message_20::Type&)> v2g_message;
    std::function<void(const std::string&)> evccid;
    std::function<void(const std::string&)> selected_protocol;
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

private:
    feedback::Callbacks callbacks;
};

} // namespace iso15118::session
