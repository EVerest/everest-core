// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../states.hpp"

#include <cstdint>
#include <optional>

#include <iso15118/d20/dynamic_mode_parameters.hpp>

namespace iso15118::d20::state {

struct DC_ChargeLoop : public StateBase {
    DC_ChargeLoop(Context& ctx) : StateBase(ctx, StateID::DC_ChargeLoop) {
    }

    void enter() final;

    Result feed(Event) final;

private:
    float present_voltage{0};
    float present_current{0};
    bool stop{false};

    UpdateDynamicModeParameters dynamic_parameters;

    bool first_entry_in_charge_loop{true};
};

} // namespace iso15118::d20::state
