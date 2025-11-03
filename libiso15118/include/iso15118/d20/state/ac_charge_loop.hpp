// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"

#include <iso15118/d20/ac_powers.hpp>
#include <iso15118/d20/dynamic_mode_parameters.hpp>

namespace iso15118::d20::state {
struct AC_ChargeLoop : public StateBase {
    AC_ChargeLoop(Context& ctx) : StateBase(ctx, StateID::AC_ChargeLoop) {
    }

    void enter() final;

    Result feed(Event) final;

private:
    float target_frequency{0};
    bool stop{false};
    bool pause{false};

    UpdateDynamicModeParameters dynamic_parameters{};
    AcTargetPower target_powers{};
    AcPresentPower present_powers{};

    bool first_entry_in_charge_loop{false};
};

} // namespace iso15118::d20::state
