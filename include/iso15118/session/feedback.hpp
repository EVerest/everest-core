// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>

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

struct Callbacks {
    std::function<void(Signal)> signal;
    std::function<void(const DcChargeTarget&)> dc_charge_target;
    std::function<void(const DcMaximumLimits&)> dc_max_limits;
};

} // namespace feedback

class Feedback {
public:
    Feedback(feedback::Callbacks);

    void signal(feedback::Signal) const;
    void dc_charge_target(const feedback::DcChargeTarget&) const;
    void dc_max_limits(const feedback::DcMaximumLimits&) const;

private:
    feedback::Callbacks callbacks;
};

} // namespace iso15118::session
