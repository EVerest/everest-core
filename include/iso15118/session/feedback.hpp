// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <optional>
#include <string>

#include <iso15118/message/type.hpp>

namespace iso15118::session {

namespace feedback {

enum class Signal {
    REQUIRE_AUTH_EIM,
    START_CABLE_CHECK,
    SETUP_FINISHED,
    CHARGE_LOOP_STARTED,
    CHARGE_LOOP_FINISHED,
    DC_OPEN_CONTACTOR,
    DLINK_TERMINATE,
    DLINK_ERROR,
    DLINK_PAUSE,
};

struct DcChargeTarget {
    float voltage{-1};
    float current{-1};
};

struct DcMaximumLimits {
    float voltage{-1};
    float current{-1};
    float power{-1};
};

struct DisplayParameters {
    std::optional<float> present_soc;
    std::optional<float> minimum_soc;
    std::optional<float> target_soc;
    std::optional<float> maximum_soc;
    std::optional<float> remaining_time_to_minimum_soc;
    std::optional<float> remaining_time_to_target_soc;
    std::optional<float> remaining_time_to_maximum_soc;
    std::optional<float> battery_energy_capacity;
    std::optional<bool> inlet_hot;
};

struct Callbacks {
    std::function<void(Signal)> signal;
    std::function<void(const DcChargeTarget&)> dc_charge_target;
    std::function<void(const DcMaximumLimits&)> dc_max_limits;
    std::function<void(const message_20::Type&)> v2g_message;
    std::function<void(const std::string&)> evccid;
    std::function<void(const std::string&)> selected_protocol;
    std::function<void(const DisplayParameters&)> display_parameters;
};

} // namespace feedback

class Feedback {
public:
    Feedback(feedback::Callbacks);

    void signal(feedback::Signal) const;
    void dc_charge_target(const feedback::DcChargeTarget&) const;
    void dc_max_limits(const feedback::DcMaximumLimits&) const;
    void v2g_message(const message_20::Type&) const;
    void evcc_id(const std::string&) const;
    void selected_protocol(const std::string&) const;
    void display_parameters(const feedback::DisplayParameters&) const;

private:
    feedback::Callbacks callbacks;
};

} // namespace iso15118::session
